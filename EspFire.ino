#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <ESP32Servo.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// ----------------- WIFI CREDENTIALS -----------------
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "Password"

// ----------------- FIREBASE CREDENTIALS -----------------
#define API_KEY "*****************"
#define DATABASE_URL "https://smartbin-1818-default-rtdb.asiasoutheast1.firebasedatabase.app/"

// ----------------- FIREBASE OBJECTS -----------------
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ----------------- GPS SETUP -----------------
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

// ----------------- SERVO -----------------
Servo myServo;

// ----------------- PIN CONFIGURATION -----------------
#define TRIG_PRESENCE 4
#define ECHO_PRESENCE 5
#define TRIG_WET 12
#define ECHO_WET 13
#define TRIG_DRY 14
#define ECHO_DRY 27
#define RAIN_SENSOR 34
#define SERVO_PIN 18

// ----------------- SERVO ANGLES -----------------
#define SERVO_LEFT 0
#define SERVO_CENTER 90
#define SERVO_RIGHT 180

#define RAIN_THRESHOLD 3800
#define BIN_FULL_CM 15

// ----------------- TIMERS -----------------
unsigned long lastGPSUpdate = 0;
unsigned long lastWasteCheck = 0;

// ----------------- BIN LEVELS -----------------
String wetBinLevel = "";
String dryBinLevel = "";

bool signupOK = false;

// ----------------- SETUP -----------------
void setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600, SERIAL_8N1, 16, 17);

    // Servo initialization
    myServo.attach(SERVO_PIN);
    myServo.write(SERVO_CENTER);

    // Pin modes
    pinMode(TRIG_PRESENCE, OUTPUT);
    pinMode(ECHO_PRESENCE, INPUT);
    pinMode(TRIG_WET, OUTPUT);
    pinMode(ECHO_WET, INPUT);
    pinMode(TRIG_DRY, OUTPUT);
    pinMode(ECHO_DRY, INPUT);
    pinMode(RAIN_SENSOR, INPUT);

    // WiFi connection
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());

    // Firebase setup
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    config.token_status_callback = tokenStatusCallback;

    if (Firebase.signUp(&config, &auth, "", "")) {
        Serial.println("Firebase signup successful");
        signupOK = true;
    } else {
        Serial.printf("Signup failed: %s\n", config.signer.signupError.message.c_str());
    }

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

// ----------------- HELPER FUNCTIONS -----------------
long readUltrasonic(int trig, int echo) {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    long duration = pulseIn(echo, HIGH);
    return duration * 0.034 / 2;
}

String getLevelRange(int dist) {
    if (dist <= 10) return "100%";
    else if (dist <= 15) return "70%";
    else if (dist <= 20) return "50%";
    else if (dist <= 25) return "25%";
    else return "0%";
}

void uploadBinLevels(String wetLevel, String dryLevel) {
    if (Firebase.ready() && signupOK) {
        if (Firebase.RTDB.setString(&fbdo, "bin3/binLevels/wet", wetLevel)) {
            Serial.println("Wet level uploaded: " + wetLevel);
        } else {
            Serial.println("Failed to upload wet level: " + fbdo.errorReason());
        }

        if (Firebase.RTDB.setString(&fbdo, "bin3/binLevels/dry", dryLevel)) {
            Serial.println("Dry level uploaded: " + dryLevel);
        } else {
            Serial.println("Failed to upload dry level: " + fbdo.errorReason());
        }
    }
}

void uploadGPS(double lat, double lng) {
    if (Firebase.ready() && signupOK) {
        if (Firebase.RTDB.setFloat(&fbdo, "bin3/location/latitude", lat))
            Serial.println("Latitude uploaded");
        else
            Serial.println("Failed to upload latitude: " + fbdo.errorReason());

        if (Firebase.RTDB.setFloat(&fbdo, "bin3/location/longitude", lng))
            Serial.println("Longitude uploaded");
        else
            Serial.println("Failed to upload longitude: " + fbdo.errorReason());

        if (Firebase.RTDB.setString(&fbdo, "bin3/location/timestamp", String(millis())))
            Serial.println("Timestamp uploaded");
        else
            Serial.println("Failed to upload timestamp: " + fbdo.errorReason());
    }
}

// ----------------- MAIN LOOP -----------------
void loop() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    unsigned long now = millis();

    // Upload GPS location every 1 hour (currently 15 sec for testing)
    if (now - lastGPSUpdate >= 15000UL && gps.location.isValid()) {
        uploadGPS(gps.location.lat(), gps.location.lng());
        lastGPSUpdate = now;
    }

    // Check for waste presence every 3 sec
    if (now - lastWasteCheck >= 3000) {
        lastWasteCheck = now;

        int distance = readUltrasonic(TRIG_PRESENCE, ECHO_PRESENCE);
        Serial.println("Presence: " + String(distance) + " cm");

        if (distance < 15) {
            int rainVal = analogRead(RAIN_SENSOR);
            bool isWet = rainVal < RAIN_THRESHOLD;
            Serial.println("Rain sensor value: " + String(rainVal));

            int wetDist = readUltrasonic(TRIG_WET, ECHO_WET);
            int dryDist = readUltrasonic(TRIG_DRY, ECHO_DRY);

            String newWetLevel = getLevelRange(wetDist);
            String newDryLevel = getLevelRange(dryDist);
            Serial.println("Wet: " + String(wetDist) + " cm, Dry: " + String(dryDist) + " cm");

            if (newWetLevel != wetBinLevel || newDryLevel != dryBinLevel) {
                wetBinLevel = newWetLevel;
                dryBinLevel = newDryLevel;
                uploadBinLevels(wetBinLevel, dryBinLevel);
            }

            if (isWet && wetDist > BIN_FULL_CM) {
                Serial.println("Wet waste detected. Rotating servo right.");
                myServo.write(SERVO_LEFT);
            } else if (!isWet && dryDist > BIN_FULL_CM) {
                Serial.println("Dry waste detected. Rotating servo left.");
                myServo.write(SERVO_RIGHT);
            } else {
                Serial.println("No rotation: Bins full.");
            }

            delay(4000); // Wait before resetting servo
            myServo.write(SERVO_CENTER);
        }
    }
}

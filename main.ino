#include <WiFi.h>
#include "heltec-eink-modules.h"
#include "WiFiHandshake.h"
#include "DisplayManager.h"
#include "FS.h"
#include "SPIFFS.h"

// Create an instance of the display
EInkDisplay_VisionMasterE213 display;

// Define button pin (if applicable)
#define BUTTON_PIN 0 // Adjust this to your actual button pin

void setup() {
    Serial.begin(921600); // Set baud rate for SerialShark
    display.begin();
    display.setRotation(3); // Rotate display 180 degrees
    display.clear();

    // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        updateDisplay("SPIFFS Mount Failed");
        return;
    }

    // Initialize button pin
    pinMode(BUTTON_PIN, INPUT_PULLUP); // Set BUTTON_PIN as input with pull-up resistor

    // Enable Wi-Fi in monitor mode
    WiFi.mode(WIFI_MODE_NULL); // Set to NULL mode for scanning
    esp_wifi_start(); // Start the Wi-Fi driver
    Serial.println("WiFi initialized in monitor mode.");

    updateDisplay("Ready! Press button to start.");
}

void loop() {
    // Check for button presses or other input methods here
    if (digitalRead(BUTTON_PIN) == LOW) { // Adjust BUTTON_PIN as needed (LOW if using pull-up)
        Serial.println("Starting scanning and capturing...");
        updateDisplay("Scanning...");

        while (true) { // Continuous scanning loop
            channelHopper(); // Start channel hopping and capturing handshakes
            delay(500); // Simple debounce delay
        }
    }
}

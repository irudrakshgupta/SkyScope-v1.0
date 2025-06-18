/*
 * ADS-B Flight Tracker Test Sketch
 * 
 * This sketch simulates ADS-B messages for testing the tracker
 * without actual hardware. It generates sample Mode-S messages
 * and sends them to the main tracker sketch.
 * 
 * Use this for:
 * - Testing display functionality
 * - Debugging message parsing
 * - Development without ADS-B receiver
 * - Demonstrating the system
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Create display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Test aircraft data
struct TestAircraft {
  char icao[7];
  int altitude;
  char messageType[20];
  unsigned long lastUpdate;
  bool active;
};

// Sample aircraft data
TestAircraft testAircraft[] = {
  {"A8C92B", 37000, "Position", 0, true},
  {"B12345", 25000, "Identity", 0, true},
  {"C98765", 15000, "Velocity", 0, true},
  {"D55555", 45000, "Altitude", 0, true},
  {"E77777", 32000, "Position", 0, true}
};

int testAircraftCount = 5;
unsigned long messageCount = 0;
unsigned long lastMessageTime = 0;
unsigned long lastDisplayUpdate = 0;

// Sample Mode-S messages (hex format)
char* sampleMessages[] = {
  "8D40621D58C382D690C8AC2863A7",  // Position message
  "8D4840D6202CC371C32CE0576098",  // Identity message
  "8D40621D58C382D690C8AC2863A8",  // Velocity message
  "8D4840D6202CC371C32CE0576099",  // Altitude message
  "8D40621D58C382D690C8AC2863AA"   // Another position message
};

int messageIndex = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("ADS-B Test Sketch Starting...");
  
  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  
  // Initialize test aircraft timestamps
  for (int i = 0; i < testAircraftCount; i++) {
    testAircraft[i].lastUpdate = millis() + (i * 5000); // Stagger updates
  }
  
  // Show startup message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("ADS-B Test Mode"));
  display.println(F("Generating sample"));
  display.println(F("aircraft data..."));
  display.display();
  
  delay(2000);
  
  Serial.println("Test mode ready - generating sample messages...");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Generate test messages periodically
  if (currentTime - lastMessageTime > 3000) { // Every 3 seconds
    generateTestMessage();
    lastMessageTime = currentTime;
  }
  
  // Update display
  if (currentTime - lastDisplayUpdate > 1000) {
    updateTestDisplay();
    lastDisplayUpdate = currentTime;
  }
  
  // Simulate aircraft updates
  updateTestAircraft();
}

void generateTestMessage() {
  // Select a random aircraft
  int aircraftIndex = random(testAircraftCount);
  TestAircraft* aircraft = &testAircraft[aircraftIndex];
  
  // Generate a sample message
  char message[50];
  strcpy(message, sampleMessages[messageIndex % 5]);
  
  // Update aircraft data
  aircraft->lastUpdate = millis();
  aircraft->active = true;
  
  // Randomly change altitude
  if (random(100) < 30) { // 30% chance to change altitude
    aircraft->altitude += random(-2000, 2000);
    if (aircraft->altitude < 5000) aircraft->altitude = 5000;
    if (aircraft->altitude > 50000) aircraft->altitude = 50000;
  }
  
  messageCount++;
  messageIndex++;
  
  // Print to serial for debugging
  Serial.print("Generated message: ");
  Serial.print(message);
  Serial.print(" for aircraft ");
  Serial.println(aircraft->icao);
  
  // Simulate sending to main tracker
  // In a real setup, this would be sent via Serial1
  Serial1.println(message);
}

void updateTestAircraft() {
  unsigned long currentTime = millis();
  
  // Deactivate aircraft that haven't been updated recently
  for (int i = 0; i < testAircraftCount; i++) {
    if (testAircraft[i].active && (currentTime - testAircraft[i].lastUpdate) > 30000) {
      testAircraft[i].active = false;
      Serial.print("Test aircraft timeout: ");
      Serial.println(testAircraft[i].icao);
    }
  }
}

void updateTestDisplay() {
  display.clearDisplay();
  
  // Header
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("ADS-B Test Mode"));
  
  // Message count
  display.setCursor(0, 10);
  display.print(F("Messages: "));
  display.println(messageCount);
  
  // Active aircraft count
  int activeCount = 0;
  for (int i = 0; i < testAircraftCount; i++) {
    if (testAircraft[i].active) activeCount++;
  }
  
  display.setCursor(0, 20);
  display.print(F("Active: "));
  display.println(activeCount);
  
  // Show active aircraft
  int displayCount = 0;
  for (int i = 0; i < testAircraftCount && displayCount < 3; i++) {
    if (testAircraft[i].active) {
      int yPos = 32 + (displayCount * 10);
      display.setCursor(0, yPos);
      display.print(testAircraft[i].icao);
      display.print(F(" "));
      display.print(testAircraft[i].altitude);
      display.print(F("ft"));
      displayCount++;
    }
  }
  
  // Status line
  display.setCursor(0, 56);
  display.print(F("Test Mode Active"));
  
  display.display();
}

// Function to manually trigger a test message
void triggerTestMessage() {
  generateTestMessage();
}

// Function to reset test data
void resetTestData() {
  messageCount = 0;
  messageIndex = 0;
  
  for (int i = 0; i < testAircraftCount; i++) {
    testAircraft[i].active = true;
    testAircraft[i].lastUpdate = millis();
  }
  
  Serial.println("Test data reset");
}

// Serial command handler
void handleSerialCommands() {
  if (Serial.available()) {
    char command = Serial.read();
    
    switch (command) {
      case 't':
        triggerTestMessage();
        break;
      case 'r':
        resetTestData();
        break;
      case 's':
        printStatus();
        break;
      case 'h':
        printHelp();
        break;
    }
  }
}

void printStatus() {
  Serial.println("=== Test Status ===");
  Serial.print("Messages generated: ");
  Serial.println(messageCount);
  Serial.print("Active aircraft: ");
  
  int activeCount = 0;
  for (int i = 0; i < testAircraftCount; i++) {
    if (testAircraft[i].active) {
      activeCount++;
      Serial.print(testAircraft[i].icao);
      Serial.print(" (");
      Serial.print(testAircraft[i].altitude);
      Serial.print("ft) ");
    }
  }
  Serial.println();
  Serial.print("Total active: ");
  Serial.println(activeCount);
}

void printHelp() {
  Serial.println("=== Test Commands ===");
  Serial.println("t - Trigger test message");
  Serial.println("r - Reset test data");
  Serial.println("s - Show status");
  Serial.println("h - Show this help");
} 
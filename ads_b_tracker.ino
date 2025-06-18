/*
 * ADS-B Flight Tracker for Arduino
 * 
 * This sketch receives Mode-S messages from a 1090 MHz ADS-B receiver
 * and displays aircraft information on an OLED screen.
 * 
 * Hardware:
 * - Arduino Mega 2560
 * - 1090 MHz ADS-B Receiver Module (UART output)
 * - 0.96" I2C OLED Display (SSD1306)
 * 
 * Connections:
 * - ADS-B Receiver TX -> Arduino RX1 (Pin 19)
 * - ADS-B Receiver GND -> Arduino GND
 * - ADS-B Receiver VCC -> Arduino 5V
 * - OLED SDA -> Arduino SDA (Pin 20)
 * - OLED SCL -> Arduino SCL (Pin 21)
 * - OLED VCC -> Arduino 5V
 * - OLED GND -> Arduino GND
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

// ADS-B Receiver Configuration
#define ADSB_BAUD_RATE 115200  // Adjust based on your receiver module
#define MAX_MESSAGE_LENGTH 50
#define MAX_AIRCRAFT 10

// Aircraft data structure
struct Aircraft {
  char icao[7];        // ICAO address (6 chars + null terminator)
  int altitude;        // Altitude in feet
  unsigned long lastSeen; // Last time aircraft was detected
  bool active;         // Whether aircraft is currently active
};

// Global variables
Aircraft aircraftList[MAX_AIRCRAFT];
int aircraftCount = 0;
unsigned long messageCount = 0;
unsigned long lastDisplayUpdate = 0;
char messageBuffer[MAX_MESSAGE_LENGTH];
int bufferIndex = 0;

// Message type definitions
#define MSG_TYPE_POSITION 4
#define MSG_TYPE_IDENTITY 5
#define MSG_TYPE_VELOCITY 6
#define MSG_TYPE_ALTITUDE 9

void setup() {
  // Initialize serial communication
  Serial.begin(9600);  // Debug output
  Serial1.begin(ADSB_BAUD_RATE);  // ADS-B receiver
  
  Serial.println("ADS-B Flight Tracker Starting...");
  
  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  
  // Clear display and show startup message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("ADS-B Tracker"));
  display.println(F("Initializing..."));
  display.display();
  
  delay(2000);
  
  // Initialize aircraft list
  for (int i = 0; i < MAX_AIRCRAFT; i++) {
    aircraftList[i].active = false;
    aircraftList[i].altitude = 0;
    aircraftList[i].lastSeen = 0;
  }
  
  Serial.println("System ready - listening for aircraft...");
  
  // Show ready message
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("ADS-B Tracker Ready"));
  display.println(F("Listening for aircraft..."));
  display.display();
}

void loop() {
  // Read incoming ADS-B messages
  while (Serial1.available()) {
    char c = Serial1.read();
    
    // Start of new message (Mode-S messages start with '8D')
    if (c == '8' && bufferIndex == 0) {
      messageBuffer[bufferIndex++] = c;
    }
    // Continue building message
    else if (bufferIndex > 0 && bufferIndex < MAX_MESSAGE_LENGTH - 1) {
      messageBuffer[bufferIndex++] = c;
      
      // Check for end of message (newline or carriage return)
      if (c == '\n' || c == '\r') {
        messageBuffer[bufferIndex - 1] = '\0';  // Null terminate
        processMessage(messageBuffer);
        bufferIndex = 0;  // Reset buffer
      }
    }
    // Reset if buffer overflows
    else if (bufferIndex >= MAX_MESSAGE_LENGTH - 1) {
      bufferIndex = 0;
    }
  }
  
  // Update display periodically
  if (millis() - lastDisplayUpdate > 1000) {
    updateDisplay();
    lastDisplayUpdate = millis();
  }
  
  // Clean up old aircraft entries
  cleanupOldAircraft();
}

void processMessage(char* message) {
  messageCount++;
  
  // Validate message format (should start with 8D and be reasonable length)
  if (strlen(message) < 14 || message[0] != '8' || message[1] != 'D') {
    return;
  }
  
  Serial.print("Received: ");
  Serial.println(message);
  
  // Extract ICAO address (bytes 1-3 after preamble)
  char icao[7];
  strncpy(icao, message + 2, 6);
  icao[6] = '\0';
  
  // Extract message type (byte 4)
  char typeStr[3];
  strncpy(typeStr, message + 8, 2);
  typeStr[2] = '\0';
  int messageType = strtol(typeStr, NULL, 16);
  
  // Process based on message type
  int altitude = 0;
  char messageTypeName[20];
  
  switch (messageType) {
    case MSG_TYPE_POSITION:
      strcpy(messageTypeName, "Position");
      altitude = extractAltitude(message);
      break;
    case MSG_TYPE_IDENTITY:
      strcpy(messageTypeName, "Identity");
      break;
    case MSG_TYPE_VELOCITY:
      strcpy(messageTypeName, "Velocity");
      break;
    case MSG_TYPE_ALTITUDE:
      strcpy(messageTypeName, "Altitude");
      altitude = extractAltitude(message);
      break;
    default:
      sprintf(messageTypeName, "Type %d", messageType);
      break;
  }
  
  // Add or update aircraft in list
  updateAircraft(icao, altitude, messageTypeName);
}

int extractAltitude(char* message) {
  // Basic altitude extraction for Mode-S messages
  // This is a simplified version - real decoding is more complex
  
  if (strlen(message) < 22) return 0;
  
  // Extract altitude field (bytes 9-11)
  char altStr[7];
  strncpy(altStr, message + 18, 6);
  altStr[6] = '\0';
  
  // Convert to integer (simplified)
  int rawAltitude = strtol(altStr, NULL, 16);
  
  // Basic altitude decoding (this is simplified)
  if (rawAltitude > 0) {
    // Convert to feet (approximate)
    return (rawAltitude * 25) - 1000;
  }
  
  return 0;
}

void updateAircraft(char* icao, int altitude, char* messageType) {
  unsigned long currentTime = millis();
  
  // Check if aircraft already exists
  for (int i = 0; i < aircraftCount; i++) {
    if (strcmp(aircraftList[i].icao, icao) == 0) {
      // Update existing aircraft
      aircraftList[i].lastSeen = currentTime;
      aircraftList[i].active = true;
      if (altitude > 0) {
        aircraftList[i].altitude = altitude;
      }
      return;
    }
  }
  
  // Add new aircraft if space available
  if (aircraftCount < MAX_AIRCRAFT) {
    strcpy(aircraftList[aircraftCount].icao, icao);
    aircraftList[aircraftCount].altitude = altitude;
    aircraftList[aircraftCount].lastSeen = currentTime;
    aircraftList[aircraftCount].active = true;
    aircraftCount++;
    
    Serial.print("New aircraft detected: ");
    Serial.println(icao);
  }
}

void updateDisplay() {
  display.clearDisplay();
  
  // Header
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("ADS-B Tracker"));
  
  // Message count
  display.setCursor(0, 10);
  display.print(F("Messages: "));
  display.println(messageCount);
  
  // Aircraft count
  display.setCursor(0, 20);
  display.print(F("Aircraft: "));
  display.println(aircraftCount);
  
  // Show recent aircraft (up to 3)
  int displayCount = 0;
  for (int i = 0; i < aircraftCount && displayCount < 3; i++) {
    if (aircraftList[i].active) {
      display.setCursor(0, 32 + (displayCount * 10));
      display.print(aircraftList[i].icao);
      
      if (aircraftList[i].altitude > 0) {
        display.print(F(" "));
        display.print(aircraftList[i].altitude);
        display.print(F("ft"));
      }
      
      displayCount++;
    }
  }
  
  // Status line
  display.setCursor(0, 56);
  display.print(F("Listening..."));
  
  display.display();
}

void cleanupOldAircraft() {
  unsigned long currentTime = millis();
  unsigned long timeout = 30000; // 30 seconds
  
  for (int i = 0; i < aircraftCount; i++) {
    if (aircraftList[i].active && (currentTime - aircraftList[i].lastSeen) > timeout) {
      aircraftList[i].active = false;
      Serial.print("Aircraft timeout: ");
      Serial.println(aircraftList[i].icao);
    }
  }
}

// Debug function to print aircraft list
void printAircraftList() {
  Serial.println("Current Aircraft:");
  for (int i = 0; i < aircraftCount; i++) {
    if (aircraftList[i].active) {
      Serial.print(aircraftList[i].icao);
      Serial.print(" - Alt: ");
      Serial.print(aircraftList[i].altitude);
      Serial.print("ft - Last seen: ");
      Serial.println(aircraftList[i].lastSeen);
    }
  }
} 
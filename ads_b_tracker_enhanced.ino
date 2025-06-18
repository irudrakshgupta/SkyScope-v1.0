/*
 * Enhanced ADS-B Flight Tracker for Arduino
 * 
 * This enhanced version includes configuration options, better error handling,
 * and optional features like buzzer alerts and SD card logging.
 * 
 * Hardware:
 * - Arduino Mega 2560 (recommended) or Arduino Uno
 * - 1090 MHz ADS-B Receiver Module (UART output)
 * - 0.96" I2C OLED Display (SSD1306)
 * - Optional: Buzzer, LED indicator, SD card module
 * 
 * Features:
 * - Real-time aircraft tracking
 * - Configurable for different receiver types
 * - Optional audio/visual alerts
 * - SD card logging capability
 * - Enhanced error handling and debugging
 */

#include "config.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#if ENABLE_SD_LOGGING
#include <SPI.h>
#include <SD.h>
#endif

// Create display object
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

// Aircraft data structure
struct Aircraft {
  char icao[7];        // ICAO address (6 chars + null terminator)
  int altitude;        // Altitude in feet
  unsigned long lastSeen; // Last time aircraft was detected
  bool active;         // Whether aircraft is currently active
  char messageType[20]; // Last message type received
};

// Global variables
Aircraft aircraftList[MAX_AIRCRAFT];
int aircraftCount = 0;
unsigned long messageCount = 0;
unsigned long lastDisplayUpdate = 0;
char messageBuffer[MAX_MESSAGE_LENGTH];
int bufferIndex = 0;
bool systemReady = false;

// Optional components
#if ENABLE_BUZZER
bool buzzerEnabled = true;
#endif

#if ENABLE_LED_INDICATOR
bool ledEnabled = true;
#endif

void setup() {
  // Initialize debug serial
  #if DEBUG_SERIAL
  Serial.begin(DEBUG_BAUD_RATE);
  Serial.println(F("Enhanced ADS-B Flight Tracker Starting..."));
  #endif
  
  // Initialize ADS-B receiver serial
  ADSB_SERIAL_PORT.begin(ADSB_BAUD_RATE);
  
  // Initialize optional components
  initializeOptionalComponents();
  
  // Initialize OLED display
  if (!initializeDisplay()) {
    #if DEBUG_SERIAL
    Serial.println(F("Display initialization failed!"));
    #endif
    // Continue without display
  }
  
  // Initialize SD card if enabled
  #if ENABLE_SD_LOGGING
  if (!initializeSDCard()) {
    #if DEBUG_SERIAL
    Serial.println(F("SD card initialization failed!"));
    #endif
  }
  #endif
  
  // Initialize aircraft list
  initializeAircraftList();
  
  // Show startup message
  showStartupMessage();
  
  systemReady = true;
  
  #if DEBUG_SERIAL
  Serial.println(F("System ready - listening for aircraft..."));
  #endif
}

void loop() {
  if (!systemReady) {
    delay(1000);
    return;
  }
  
  // Read incoming ADS-B messages
  processIncomingMessages();
  
  // Update display periodically
  if (millis() - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL) {
    updateDisplay();
    lastDisplayUpdate = millis();
  }
  
  // Clean up old aircraft entries
  cleanupOldAircraft();
  
  // Handle optional features
  handleOptionalFeatures();
}

void initializeOptionalComponents() {
  #if ENABLE_BUZZER
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  #endif
  
  #if ENABLE_LED_INDICATOR
  pinMode(LED_INDICATOR_PIN, OUTPUT);
  digitalWrite(LED_INDICATOR_PIN, LOW);
  #endif
}

bool initializeDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    return false;
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  return true;
}

#if ENABLE_SD_LOGGING
bool initializeSDCard() {
  if (!SD.begin(SD_CS_PIN)) {
    return false;
  }
  
  // Create log file with timestamp
  File logFile = SD.open("adsb_log.txt", FILE_WRITE);
  if (logFile) {
    logFile.println("ADS-B Flight Tracker Log");
    logFile.println("Timestamp,ICAO,Altitude,MessageType");
    logFile.close();
    return true;
  }
  
  return false;
}
#endif

void initializeAircraftList() {
  for (int i = 0; i < MAX_AIRCRAFT; i++) {
    aircraftList[i].active = false;
    aircraftList[i].altitude = 0;
    aircraftList[i].lastSeen = 0;
    strcpy(aircraftList[i].messageType, "");
  }
}

void showStartupMessage() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Enhanced ADS-B"));
  display.println(F("Flight Tracker"));
  display.println(F(""));
  display.println(F("Initializing..."));
  display.display();
  
  delay(2000);
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("System Ready"));
  display.println(F("Listening for"));
  display.println(F("aircraft..."));
  display.display();
}

void processIncomingMessages() {
  while (ADSB_SERIAL_PORT.available()) {
    char c = ADSB_SERIAL_PORT.read();
    
    // Start of new message (Mode-S messages start with '8D')
    if (c == '8' && bufferIndex == 0) {
      messageBuffer[bufferIndex++] = c;
    }
    // Continue building message
    else if (bufferIndex > 0 && bufferIndex < MAX_MESSAGE_LENGTH - 1) {
      messageBuffer[bufferIndex++] = c;
      
      // Check for end of message
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
}

void processMessage(char* message) {
  messageCount++;
  
  // Validate message format
  if (!isValidMessage(message)) {
    return;
  }
  
  #if DEBUG_MESSAGES
  Serial.print(F("Received: "));
  Serial.println(message);
  #endif
  
  // Extract message components
  char icao[7];
  int messageType;
  int altitude = 0;
  char messageTypeName[20];
  
  extractMessageComponents(message, icao, &messageType, &altitude, messageTypeName);
  
  // Add or update aircraft in list
  updateAircraft(icao, altitude, messageTypeName);
  
  // Log to SD card if enabled
  #if ENABLE_SD_LOGGING
  logToSDCard(icao, altitude, messageTypeName);
  #endif
  
  // Trigger alerts
  triggerAlerts(icao, altitude);
}

bool isValidMessage(char* message) {
  if (strlen(message) < MIN_MESSAGE_LENGTH) {
    return false;
  }
  
  // Check for Mode-S preamble
  if (message[0] != '8' || message[1] != 'D') {
    return false;
  }
  
  // Basic length validation
  if (strlen(message) > MAX_MESSAGE_LENGTH_HEX) {
    return false;
  }
  
  return true;
}

void extractMessageComponents(char* message, char* icao, int* messageType, int* altitude, char* messageTypeName) {
  // Extract ICAO address (bytes 1-3 after preamble)
  strncpy(icao, message + 2, 6);
  icao[6] = '\0';
  
  // Extract message type (byte 4)
  char typeStr[3];
  strncpy(typeStr, message + 8, 2);
  typeStr[2] = '\0';
  *messageType = strtol(typeStr, NULL, 16);
  
  // Process based on message type
  switch (*messageType) {
    case MSG_TYPE_POSITION:
      strcpy(messageTypeName, "Position");
      *altitude = extractAltitude(message);
      break;
    case MSG_TYPE_IDENTITY:
      strcpy(messageTypeName, "Identity");
      break;
    case MSG_TYPE_VELOCITY:
      strcpy(messageTypeName, "Velocity");
      break;
    case MSG_TYPE_ALTITUDE:
      strcpy(messageTypeName, "Altitude");
      *altitude = extractAltitude(message);
      break;
    case MSG_TYPE_SURFACE_POSITION:
      strcpy(messageTypeName, "Surface");
      break;
    case MSG_TYPE_AIRBORNE_POSITION:
      strcpy(messageTypeName, "Airborne");
      *altitude = extractAltitude(message);
      break;
    case MSG_TYPE_AIRBORNE_VELOCITY:
      strcpy(messageTypeName, "Velocity");
      break;
    default:
      sprintf(messageTypeName, "Type %d", *messageType);
      break;
  }
}

int extractAltitude(char* message) {
  if (strlen(message) < 22) return 0;
  
  // Extract altitude field (bytes 9-11)
  char altStr[7];
  strncpy(altStr, message + 18, 6);
  altStr[6] = '\0';
  
  // Convert to integer
  int rawAltitude = strtol(altStr, NULL, 16);
  
  // Basic altitude decoding
  if (rawAltitude > 0) {
    return (rawAltitude * ALTITUDE_MULTIPLIER) + ALTITUDE_OFFSET;
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
      strcpy(aircraftList[i].messageType, messageType);
      
      #if DEBUG_AIRCRAFT_UPDATES
      Serial.print(F("Updated aircraft: "));
      Serial.println(icao);
      #endif
      
      return;
    }
  }
  
  // Add new aircraft if space available
  if (aircraftCount < MAX_AIRCRAFT) {
    strcpy(aircraftList[aircraftCount].icao, icao);
    aircraftList[aircraftCount].altitude = altitude;
    aircraftList[aircraftCount].lastSeen = currentTime;
    aircraftList[aircraftCount].active = true;
    strcpy(aircraftList[aircraftCount].messageType, messageType);
    aircraftCount++;
    
    #if DEBUG_AIRCRAFT_UPDATES
    Serial.print(F("New aircraft detected: "));
    Serial.println(icao);
    #endif
  }
}

void updateDisplay() {
  display.clearDisplay();
  
  // Header
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("ADS-B Tracker"));
  
  // Statistics
  #if SHOW_MESSAGE_COUNT
  display.setCursor(0, 10);
  display.print(F("Messages: "));
  display.println(messageCount);
  #endif
  
  #if SHOW_AIRCRAFT_COUNT
  display.setCursor(0, 20);
  display.print(F("Aircraft: "));
  display.println(aircraftCount);
  #endif
  
  // Show recent aircraft
  int displayCount = 0;
  for (int i = 0; i < aircraftCount && displayCount < MAX_DISPLAY_AIRCRAFT; i++) {
    if (aircraftList[i].active) {
      int yPos = 32 + (displayCount * 10);
      display.setCursor(0, yPos);
      display.print(aircraftList[i].icao);
      
      #if SHOW_ALTITUDE
      if (aircraftList[i].altitude > 0) {
        display.print(F(" "));
        display.print(aircraftList[i].altitude);
        display.print(F("ft"));
      }
      #endif
      
      displayCount++;
    }
  }
  
  // Status line
  #if STATUS_LINE_ENABLED
  display.setCursor(0, 56);
  display.print(F("Listening..."));
  #endif
  
  display.display();
}

void cleanupOldAircraft() {
  unsigned long currentTime = millis();
  
  for (int i = 0; i < aircraftCount; i++) {
    if (aircraftList[i].active && (currentTime - aircraftList[i].lastSeen) > AIRCRAFT_TIMEOUT) {
      aircraftList[i].active = false;
      
      #if DEBUG_AIRCRAFT_UPDATES
      Serial.print(F("Aircraft timeout: "));
      Serial.println(aircraftList[i].icao);
      #endif
    }
  }
}

void triggerAlerts(char* icao, int altitude) {
  // Simple alert for new aircraft detection
  static char lastIcao[7] = "";
  
  if (strcmp(icao, lastIcao) != 0) {
    strcpy(lastIcao, icao);
    
    #if ENABLE_BUZZER
    if (buzzerEnabled) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN, LOW);
    }
    #endif
    
    #if ENABLE_LED_INDICATOR
    if (ledEnabled) {
      digitalWrite(LED_INDICATOR_PIN, HIGH);
      delay(200);
      digitalWrite(LED_INDICATOR_PIN, LOW);
    }
    #endif
  }
}

#if ENABLE_SD_LOGGING
void logToSDCard(char* icao, int altitude, char* messageType) {
  File logFile = SD.open("adsb_log.txt", FILE_WRITE);
  if (logFile) {
    logFile.print(millis());
    logFile.print(",");
    logFile.print(icao);
    logFile.print(",");
    logFile.print(altitude);
    logFile.print(",");
    logFile.println(messageType);
    logFile.close();
  }
}
#endif

void handleOptionalFeatures() {
  // Handle any periodic optional feature updates
  // This function can be expanded for additional features
}

// Debug function to print aircraft list
void printAircraftList() {
  #if DEBUG_SERIAL
  Serial.println(F("Current Aircraft:"));
  for (int i = 0; i < aircraftCount; i++) {
    if (aircraftList[i].active) {
      Serial.print(aircraftList[i].icao);
      Serial.print(F(" - Alt: "));
      Serial.print(aircraftList[i].altitude);
      Serial.print(F("ft - Type: "));
      Serial.print(aircraftList[i].messageType);
      Serial.print(F(" - Last seen: "));
      Serial.println(aircraftList[i].lastSeen);
    }
  }
  #endif
} 
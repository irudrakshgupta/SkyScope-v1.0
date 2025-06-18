/*
 * ADS-B Flight Tracker Configuration
 * 
 * This file contains all configurable parameters for the ADS-B tracker.
 * Modify these settings based on your hardware and requirements.
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

// Arduino Board Selection
#define BOARD_ARDUINO_MEGA 1
#define BOARD_ARDUINO_UNO 0

// OLED Display Configuration
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C  // Common I2C address for most OLED displays

// ADS-B Receiver Configuration
#define ADSB_BAUD_RATE 115200  // Common baud rates: 9600, 19200, 38400, 57600, 115200
#define ADSB_SERIAL_PORT Serial1  // Use Serial1 for Mega, SoftwareSerial for Uno

// ============================================================================
// MESSAGE PROCESSING CONFIGURATION
// ============================================================================

// Buffer sizes
#define MAX_MESSAGE_LENGTH 50
#define MAX_AIRCRAFT 10
#define MAX_MESSAGE_HISTORY 100

// Timeout settings (in milliseconds)
#define AIRCRAFT_TIMEOUT 30000  // 30 seconds
#define DISPLAY_UPDATE_INTERVAL 1000  // 1 second

// ============================================================================
// MODE-S MESSAGE TYPES
// ============================================================================

// ADS-B Message Type Codes
#define MSG_TYPE_POSITION 4
#define MSG_TYPE_IDENTITY 5
#define MSG_TYPE_VELOCITY 6
#define MSG_TYPE_ALTITUDE 9
#define MSG_TYPE_SURFACE_POSITION 20
#define MSG_TYPE_AIRBORNE_POSITION 21
#define MSG_TYPE_AIRBORNE_VELOCITY 22

// ============================================================================
// DISPLAY CONFIGURATION
// ============================================================================

// Display update settings
#define SHOW_MESSAGE_COUNT 1
#define SHOW_AIRCRAFT_COUNT 1
#define SHOW_ALTITUDE 1
#define SHOW_TIMESTAMP 0

// Display layout
#define MAX_DISPLAY_AIRCRAFT 3
#define STATUS_LINE_ENABLED 1

// ============================================================================
// DEBUG AND LOGGING
// ============================================================================

// Debug output settings
#define DEBUG_SERIAL 1
#define DEBUG_MESSAGES 1
#define DEBUG_AIRCRAFT_UPDATES 1

// Serial debug baud rate
#define DEBUG_BAUD_RATE 9600

// ============================================================================
// OPTIONAL FEATURES
// ============================================================================

// Enable optional components
#define ENABLE_BUZZER 0
#define ENABLE_LED_INDICATOR 0
#define ENABLE_SD_LOGGING 0

// Pin definitions for optional components
#define BUZZER_PIN 8
#define LED_INDICATOR_PIN 13

// SD Card pins (if enabled)
#define SD_CS_PIN 53
#define SD_SCK_PIN 52
#define SD_MOSI_PIN 51
#define SD_MISO_PIN 50

// ============================================================================
// RECEIVER-SPECIFIC SETTINGS
// ============================================================================

// Common ADS-B receiver modules and their settings
#define RECEIVER_GENERIC 0
#define RECEIVER_RADARBOX 1
#define RECEIVER_STRATUX 2
#define RECEIVER_DIY_1090 3

// Select your receiver type
#define RECEIVER_TYPE RECEIVER_GENERIC

// Receiver-specific configurations
#if RECEIVER_TYPE == RECEIVER_RADARBOX
  #define ADSB_BAUD_RATE 115200
  #define MESSAGE_PREFIX "8D"
#elif RECEIVER_TYPE == RECEIVER_STRATUX
  #define ADSB_BAUD_RATE 9600
  #define MESSAGE_PREFIX "8D"
#elif RECEIVER_TYPE == RECEIVER_DIY_1090
  #define ADSB_BAUD_RATE 115200
  #define MESSAGE_PREFIX "8D"
#else
  #define ADSB_BAUD_RATE 115200
  #define MESSAGE_PREFIX "8D"
#endif

// ============================================================================
// ADVANCED SETTINGS
// ============================================================================

// Altitude calculation parameters
#define ALTITUDE_OFFSET -1000
#define ALTITUDE_MULTIPLIER 25

// Message validation
#define MIN_MESSAGE_LENGTH 14
#define MAX_MESSAGE_LENGTH_HEX 50

// Performance settings
#define ENABLE_MESSAGE_FILTERING 1
#define ENABLE_DUPLICATE_DETECTION 1

#endif // CONFIG_H 
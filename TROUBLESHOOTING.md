# ADS-B Flight Tracker Troubleshooting Guide

This guide helps you diagnose and fix common issues when building and using the ADS-B Flight Tracker.

## Table of Contents

1. [Hardware Issues](#hardware-issues)
2. [Software Issues](#software-issues)
3. [Display Problems](#display-problems)
4. [Signal Reception Issues](#signal-reception-issues)
5. [Power Problems](#power-problems)
6. [Performance Issues](#performance-issues)

## Hardware Issues

### Arduino Not Responding

**Symptoms:**
- Arduino doesn't power on
- No response to uploads
- LED doesn't blink

**Solutions:**
1. **Check power supply**: Use a stable 5V 2A power supply
2. **Verify USB cable**: Try a different USB cable
3. **Reset Arduino**: Press the reset button
4. **Check board selection**: Ensure correct board is selected in Arduino IDE
5. **Try different USB port**: Some ports may not provide enough power

### ADS-B Receiver Not Working

**Symptoms:**
- No messages received
- Serial monitor shows no data
- Receiver LED not lit

**Solutions:**
1. **Check connections**:
   - TX from receiver → RX1 on Arduino (Pin 19)
   - GND from receiver → GND on Arduino
   - VCC from receiver → 5V or 3.3V on Arduino (check receiver specs)
2. **Verify baud rate**: Ensure baud rate in code matches receiver (usually 115200 or 9600)
3. **Check antenna connection**: Ensure antenna is properly connected to SMA port
4. **Power supply**: Some receivers need more power than USB can provide
5. **Test with serial monitor**: Connect receiver directly to computer to verify it's working

### OLED Display Not Working

**Symptoms:**
- Display is blank
- Display shows garbled text
- Display doesn't initialize

**Solutions:**
1. **Check I2C connections**:
   - SDA → Arduino SDA (Pin 20 on Mega)
   - SCL → Arduino SCL (Pin 21 on Mega)
   - VCC → Arduino 5V
   - GND → Arduino GND
2. **Verify I2C address**: Most displays use 0x3C, some use 0x3D
3. **Check libraries**: Ensure Adafruit GFX and Adafruit SSD1306 are installed
4. **Test with I2C scanner**: Use Arduino I2C scanner sketch to find correct address
5. **Power supply**: Display may need stable 5V power

## Software Issues

### Compilation Errors

**Common Errors and Solutions:**

1. **"Adafruit_SSD1306.h: No such file or directory"**
   ```
   Solution: Install Adafruit SSD1306 library via Library Manager
   ```

2. **"Adafruit_GFX.h: No such file or directory"**
   ```
   Solution: Install Adafruit GFX library via Library Manager
   ```

3. **"Wire.h: No such file or directory"**
   ```
   Solution: This is a built-in library, check Arduino IDE installation
   ```

4. **Memory issues on Arduino Uno**
   ```
   Solution: Use Arduino Mega 2560 for more memory, or reduce buffer sizes
   ```

### Upload Failures

**Symptoms:**
- "Upload failed" error
- Arduino not in sync
- Port not found

**Solutions:**
1. **Check board selection**: Tools → Board → Arduino Mega 2560
2. **Check port selection**: Tools → Port → Select correct COM port
3. **Reset Arduino**: Press reset button before uploading
4. **Close serial monitor**: Ensure serial monitor is closed
5. **Try different USB cable**: Some cables are charge-only

### Code Not Working as Expected

**Symptoms:**
- No aircraft detected
- Display shows incorrect data
- System freezes

**Solutions:**
1. **Check configuration**: Review `config.h` settings
2. **Enable debug output**: Set `DEBUG_SERIAL 1` in config
3. **Test with test sketch**: Use `test_sketch.ino` to verify basic functionality
4. **Check message format**: Ensure receiver outputs correct Mode-S format
5. **Monitor serial output**: Use Serial Monitor to see debug messages

## Display Problems

### Blank Display

**Solutions:**
1. **Check power**: Ensure display has 5V power
2. **Check I2C address**: Use I2C scanner to find correct address
3. **Check connections**: Verify SDA/SCL connections
4. **Test with simple sketch**: Try basic OLED test sketch
5. **Check contrast**: Some displays need contrast adjustment

### Garbled Text

**Solutions:**
1. **Check library version**: Update to latest Adafruit libraries
2. **Verify display type**: Ensure correct display driver (SSD1306 vs SH1106)
3. **Check text size**: Reduce text size if text is too large
4. **Clear display**: Add `display.clearDisplay()` before writing
5. **Check memory**: Reduce buffer sizes if memory is low

### Display Not Updating

**Solutions:**
1. **Add display.display()**: Ensure `display.display()` is called after writing
2. **Check update frequency**: Reduce update frequency if too fast
3. **Check loop timing**: Ensure loop runs frequently enough
4. **Memory issues**: Reduce complexity if memory is low
5. **Power issues**: Ensure stable power supply

## Signal Reception Issues

### No Aircraft Detected

**Solutions:**
1. **Check antenna**: Ensure antenna is connected and positioned properly
2. **Location**: Move antenna outdoors or near windows
3. **Interference**: Move away from electronic devices
4. **Antenna type**: Use proper 1090 MHz antenna
5. **Range**: Aircraft may be too far away

### Poor Signal Quality

**Solutions:**
1. **Antenna positioning**: Mount antenna higher and outdoors
2. **Antenna type**: Use directional or amplified antenna
3. **Cable quality**: Use low-loss coaxial cable
4. **Interference**: Identify and eliminate RF interference sources
5. **Weather**: Check if weather affects reception

### Intermittent Reception

**Solutions:**
1. **Power supply**: Use stable power supply
2. **Connections**: Check all connections are secure
3. **Antenna mounting**: Ensure antenna is securely mounted
4. **Cable damage**: Check for damaged coaxial cable
5. **Receiver sensitivity**: Some receivers have sensitivity adjustments

## Power Problems

### System Resets Randomly

**Solutions:**
1. **Power supply**: Use 5V 2A or higher power supply
2. **USB power**: Avoid USB power for final setup
3. **Voltage regulator**: Add voltage regulator if needed
4. **Capacitors**: Add decoupling capacitors near power pins
5. **Wire gauge**: Use thicker wires for power connections

### Components Not Powering On

**Solutions:**
1. **Check voltage**: Measure voltage at each component
2. **Power distribution**: Ensure all components get proper voltage
3. **Current draw**: Check if power supply can handle total current
4. **Connections**: Verify all power connections are secure
5. **Faulty components**: Test components individually

### Voltage Drops

**Solutions:**
1. **Power supply capacity**: Use higher capacity power supply
2. **Wire resistance**: Use thicker wires
3. **Voltage regulator**: Add voltage regulator
4. **Capacitors**: Add bulk capacitors
5. **Power distribution**: Use star topology for power distribution

## Performance Issues

### Slow Response

**Solutions:**
1. **Reduce update frequency**: Increase display update interval
2. **Optimize code**: Remove unnecessary operations
3. **Memory usage**: Reduce buffer sizes
4. **Serial speed**: Increase serial baud rate
5. **Display updates**: Update display less frequently

### Memory Issues

**Solutions:**
1. **Use Arduino Mega**: Switch to Arduino Mega 2560
2. **Reduce buffers**: Decrease `MAX_MESSAGE_LENGTH` and `MAX_AIRCRAFT`
3. **Optimize strings**: Use `F()` macro for strings
4. **Remove features**: Disable optional features
5. **Code optimization**: Remove unused variables and functions

### System Freezes

**Solutions:**
1. **Watchdog timer**: Add watchdog timer reset
2. **Error handling**: Add error checking in critical sections
3. **Memory management**: Check for memory leaks
4. **Power supply**: Ensure stable power
5. **Temperature**: Check if components are overheating

## Testing and Diagnostics

### I2C Scanner Sketch

Use this sketch to find the correct I2C address for your display:

```cpp
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("I2C Scanner");
}

void loop() {
  byte error, address;
  int nDevices = 0;
  
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  
  if (nDevices == 0) {
    Serial.println("No I2C devices found");
  }
  
  delay(5000);
}
```

### Serial Monitor Testing

1. **Test ADS-B receiver**: Connect receiver to computer and monitor output
2. **Test Arduino communication**: Use Serial Monitor to see debug messages
3. **Check baud rates**: Ensure baud rates match between devices
4. **Message format**: Verify message format is correct

### Step-by-Step Testing

1. **Test Arduino**: Upload basic blink sketch
2. **Test display**: Upload simple OLED test sketch
3. **Test receiver**: Connect receiver and monitor serial output
4. **Test integration**: Upload main sketch and check all components
5. **Test functionality**: Verify aircraft detection and display

## Getting Help

If you're still having issues:

1. **Check documentation**: Review README.md and this troubleshooting guide
2. **Search issues**: Look for similar issues in project repository
3. **Provide details**: When asking for help, include:
   - Hardware setup
   - Software versions
   - Error messages
   - What you've tried
4. **Test components**: Test each component individually
5. **Simplify**: Start with basic functionality and add features gradually

## Prevention

To avoid issues:

1. **Use quality components**: Invest in reliable hardware
2. **Follow wiring diagram**: Double-check all connections
3. **Test incrementally**: Test each component before proceeding
4. **Use stable power**: Invest in good power supply
5. **Keep backups**: Save working code versions
6. **Document changes**: Keep notes of modifications 
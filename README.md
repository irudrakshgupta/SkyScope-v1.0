# 🛩️ SkyScope - Arduino ADS-B Flight Tracker

<div align="center">

![SkyScope Logo](https://img.shields.io/badge/SkyScope-ADS--B%20Tracker-blue?style=for-the-badge&logo=arduino)
![Arduino](https://img.shields.io/badge/Arduino-Mega%202560-green?style=flat&logo=arduino)
![License](https://img.shields.io/badge/License-MIT-yellow?style=flat)
![Version](https://img.shields.io/badge/Version-1.0.0-orange?style=flat)

**SkyScope is a compact Arduino ADS-B flight tracker that captures 1090 MHz aircraft signals, decodes key data like ICAO and altitude, and displays it on an OLED. A standalone, easy-to-build project perfect for aviation enthusiasts seeking real-time flight tracking without complex setups.**

[🚀 Quick Start](#-quick-start) • [📋 Features](#-features) • [🛠️ Hardware](#️-hardware) • [📖 Documentation](#-documentation) • [🔧 Setup](#-setup) • [📊 Examples](#-examples)

</div>

---

## 🌟 What is SkyScope?

SkyScope transforms your Arduino into a powerful aircraft tracking system that receives and decodes **ADS-B (Automatic Dependent Surveillance-Broadcast)** signals from aircraft flying overhead. Unlike complex SDR-based solutions, SkyScope uses a dedicated 1090 MHz receiver module to capture Mode-S transmissions and display real-time aircraft information on a compact OLED screen.

### ✨ Why Choose SkyScope?

- **🚀 Standalone Operation** - No computer or internet connection required
- **🔧 Easy to Build** - Simple hardware setup with clear instructions
- **💰 Cost Effective** - Uses affordable, readily available components
- **📱 Real-time Display** - Instant aircraft information on OLED screen
- **🔋 Low Power** - Perfect for portable or battery-powered operation
- **🎯 Aviation Focused** - Designed specifically for aircraft tracking

---

## 📋 Features

### 🛩️ Aircraft Detection
- **Real-time Mode-S signal reception** at 1090 MHz
- **ICAO address extraction** - Unique 6-character aircraft identifiers
- **Altitude information** when available in transmissions
- **Message type identification** (Position, Identity, Velocity, etc.)
- **Multiple aircraft tracking** (up to 10 simultaneously)

### 📱 Display & Interface
- **OLED real-time display** with aircraft information
- **Message and aircraft counters** for system monitoring
- **Status indicators** showing system health
- **Clean, readable interface** optimized for aviation data

### ⚙️ Configuration & Customization
- **Modular design** with easy configuration
- **Multiple receiver support** (RadarBox, Stratux, DIY modules)
- **Optional features** (buzzer alerts, LED indicators, SD logging)
- **Debug capabilities** for troubleshooting and development

### 🔧 Technical Features
- **Efficient message parsing** optimized for Arduino
- **Automatic timeout handling** for inactive aircraft
- **Memory management** for stable operation
- **Error handling** and system recovery

---

## 🛠️ Hardware Requirements

### 📦 Essential Components

| Component | Model | Purpose | Approx. Cost |
|-----------|-------|---------|--------------|
| **Arduino Board** | Arduino Mega 2560 R3 | Main controller | $15-25 |
| **ADS-B Receiver** | 1090 MHz UART Module | Signal reception | $30-60 |
| **OLED Display** | 0.96" I2C SSD1306 | Aircraft data display | $5-10 |
| **Antenna** | 1090 MHz SMA Whip | Signal capture | $10-20 |
| **Power Supply** | 5V 2A USB Adapter | Stable power | $8-15 |
| **Connections** | Jumper Wires | Hardware connections | $3-5 |

### 🎯 Recommended Models

#### Arduino Board
- **Primary**: Arduino Mega 2560 R3 (recommended for memory & multiple serial ports)
- **Alternative**: Arduino Uno (limited memory, may need optimization)

#### ADS-B Receiver Options
- **RadarBox Module**: Professional-grade, reliable performance
- **Stratux Compatible**: Open-source, community-supported
- **DIY 1090 MHz**: Cost-effective, requires more setup

#### Display Options
- **SSD1306 0.96"**: Most common, excellent compatibility
- **SH1106 0.96"**: Alternative driver, similar performance
- **1.3" OLED**: Larger display, better readability

### 🔌 Optional Components

| Component | Purpose | Use Case |
|-----------|---------|----------|
| **Buzzer Module** | Audio alerts | New aircraft detection |
| **LED Indicator** | Visual feedback | System status |
| **SD Card Module** | Data logging | Flight recording |
| **GPS Module** | Location tracking | Position-based filtering |
| **Enclosure Box** | Protection | Outdoor/field use |

---

## 🚀 Quick Start

### 1️⃣ Install Libraries
```bash
# In Arduino IDE: Tools > Manage Libraries
# Install these libraries:
- Adafruit GFX Library (v1.11.5+)
- Adafruit SSD1306 Library (v2.5.7+)
```

### 2️⃣ Connect Hardware
```
ADS-B Receiver → Arduino Mega 2560
├── TX → RX1 (Pin 19)
├── GND → GND
└── VCC → 5V

OLED Display → Arduino Mega 2560
├── SDA → SDA (Pin 20)
├── SCL → SCL (Pin 21)
├── VCC → 5V
└── GND → GND

Antenna → ADS-B Receiver SMA Port
```

### 3️⃣ Upload Code
1. Open `ads_b_tracker.ino` in Arduino IDE
2. Select **Arduino Mega 2560** as board
3. Choose correct COM port
4. Click **Upload**

### 4️⃣ Power Up & Test
1. Connect stable 5V power supply
2. Position antenna outdoors or near window
3. Watch for aircraft data on OLED display
4. Monitor Serial output for debugging

---

## 📖 Documentation

### 📚 Project Files

| File | Purpose | Audience |
|------|---------|----------|
| [`ads_b_tracker.ino`](ads_b_tracker.ino) | Basic implementation | Beginners |
| [`ads_b_tracker_enhanced.ino`](ads_b_tracker_enhanced.ino) | Advanced features | Experienced users |
| [`config.h`](config.h) | Configuration settings | Developers |
| [`test_sketch.ino`](test_sketch.ino) | Development testing | Developers |
| [`wiring_diagram.txt`](wiring_diagram.txt) | Hardware connections | All users |
| [`TROUBLESHOOTING.md`](TROUBLESHOOTING.md) | Problem solving | All users |

### 🔧 Configuration Guide

The `config.h` file allows easy customization:

```cpp
// Hardware Configuration
#define BOARD_ARDUINO_MEGA 1
#define OLED_ADDRESS 0x3C
#define ADSB_BAUD_RATE 115200

// Feature Toggles
#define ENABLE_BUZZER 0
#define ENABLE_LED_INDICATOR 0
#define ENABLE_SD_LOGGING 0

// Performance Settings
#define MAX_AIRCRAFT 10
#define AIRCRAFT_TIMEOUT 30000
```

### 📊 Display Examples

```
┌─────────────────────────┐
│     ADS-B Tracker       │
│                         │
│ Messages: 1,247         │
│ Aircraft: 3             │
│                         │
│ A8C92B 37000ft          │
│ B12345 25000ft          │
│ C98765 15000ft          │
│                         │
│ Listening...            │
└─────────────────────────┘
```

---

## 🔧 Setup Instructions

### 📋 Prerequisites

- **Arduino IDE** (v1.8.0 or later)
- **Basic electronics knowledge**
- **1090 MHz ADS-B receiver module**
- **Soldering iron** (for some connections)

### 🛠️ Step-by-Step Build

#### Phase 1: Software Setup
1. **Install Arduino IDE** from [arduino.cc](https://www.arduino.cc/en/software)
2. **Install Required Libraries**:
   - Open Arduino IDE
   - Go to `Tools > Manage Libraries`
   - Search and install:
     - "Adafruit GFX"
     - "Adafruit SSD1306"
3. **Download SkyScope**:
   - Clone or download this repository
   - Extract to Arduino projects folder

#### Phase 2: Hardware Assembly
1. **Prepare Components**:
   - Gather all required hardware
   - Check component specifications
   - Prepare breadboard or PCB

2. **Connect ADS-B Receiver**:
   ```
   Receiver TX → Arduino RX1 (Pin 19)
   Receiver GND → Arduino GND
   Receiver VCC → Arduino 5V (or 3.3V if specified)
   ```

3. **Connect OLED Display**:
   ```
   Display SDA → Arduino SDA (Pin 20)
   Display SCL → Arduino SCL (Pin 21)
   Display VCC → Arduino 5V
   Display GND → Arduino GND
   ```

4. **Connect Antenna**:
   - Attach 1090 MHz antenna to receiver SMA port
   - Position antenna for optimal reception

#### Phase 3: Software Configuration
1. **Open Main Sketch**:
   - Open `ads_b_tracker.ino` in Arduino IDE
   - Review and modify `config.h` if needed

2. **Configure Settings**:
   - Set correct baud rate for your receiver
   - Adjust OLED address if needed
   - Enable/disable optional features

3. **Upload Code**:
   - Select **Arduino Mega 2560** as board
   - Choose correct COM port
   - Click **Upload**

#### Phase 4: Testing & Calibration
1. **Initial Test**:
   - Power on system
   - Check OLED initialization
   - Verify serial communication

2. **Signal Testing**:
   - Position antenna outdoors
   - Monitor for aircraft detection
   - Check display updates

3. **Optimization**:
   - Adjust antenna position
   - Fine-tune receiver settings
   - Optimize display layout

---

## 📊 Examples & Use Cases

### 🏠 Home Aviation Station
Perfect for aviation enthusiasts wanting to track aircraft from home:
- **Setup**: Indoor antenna near window
- **Range**: 50-100 miles depending on location
- **Use**: Monitor local air traffic, identify aircraft types

### 🏕️ Portable Field Unit
Ideal for outdoor aviation events or field trips:
- **Setup**: Battery-powered with portable antenna
- **Range**: 30-60 miles in open areas
- **Use**: Air shows, aviation events, educational demonstrations

### 🎓 Educational Projects
Great for learning about aviation technology:
- **Setup**: Classroom or lab environment
- **Features**: Real-time data, historical logging
- **Use**: STEM education, aviation courses, technology demonstrations

### 🔬 Development Platform
Excellent base for advanced projects:
- **Extensions**: GPS integration, web interface, data analysis
- **Customization**: Multiple receivers, custom displays
- **Use**: Research, advanced tracking systems, commercial applications

---

## 🚨 Limitations & Considerations

### 📡 Technical Limitations
- **Position Decoding**: Cannot decode GPS coordinates (CPR decoding not implemented)
- **Range**: Limited by antenna design and local interference
- **Update Rate**: Depends on aircraft transmission frequency
- **Message Types**: Focuses on basic altitude and identification messages

### 🌍 Environmental Factors
- **Weather**: Rain and storms can affect signal reception
- **Interference**: Electronic devices may cause RF interference
- **Location**: Urban areas may have limited line-of-sight
- **Altitude**: Higher aircraft may be beyond reception range

### 🔋 Power Considerations
- **Current Draw**: System requires stable 5V 2A power supply
- **Battery Life**: Portable operation limited by power consumption
- **Voltage Stability**: Sensitive to power fluctuations

---

## 🛠️ Troubleshooting

### 🔍 Common Issues

| Problem | Symptoms | Solution |
|---------|----------|----------|
| **No Aircraft Detected** | Display shows "Listening..." | Check antenna, move outdoors, verify receiver |
| **Display Issues** | Blank or garbled screen | Check I2C connections, verify library installation |
| **Serial Errors** | Upload failures | Check board selection, close serial monitor |
| **Power Problems** | Random resets | Use stable 5V 2A power supply |

### 📞 Getting Help

1. **Check Documentation**: Review [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
2. **Test Components**: Use [test_sketch.ino](test_sketch.ino) for debugging
3. **Community Support**: Check GitHub issues and discussions
4. **Provide Details**: Include hardware setup, error messages, and steps tried

---

## 🔮 Future Enhancements

### 🚀 Planned Features
- **GPS Integration**: Location-based aircraft filtering
- **Web Interface**: Remote monitoring via WiFi
- **Enhanced Position Decoding**: Full CPR implementation
- **Data Logging**: Comprehensive flight recording
- **Alert System**: Custom aircraft notifications

### 🔧 Technical Improvements
- **Multiple Receiver Support**: Enhanced antenna diversity
- **Advanced Filtering**: Signal quality optimization
- **Memory Optimization**: Better resource management
- **Performance Tuning**: Faster message processing

### 📱 User Experience
- **Mobile App**: Smartphone integration
- **Cloud Storage**: Remote data access
- **Social Features**: Share aircraft sightings
- **Analytics**: Flight pattern analysis

---

## 🤝 Contributing

We welcome contributions to SkyScope! Here's how you can help:

### 🐛 Bug Reports
- Use GitHub issues with detailed descriptions
- Include hardware setup and error messages
- Provide steps to reproduce the problem

### 💡 Feature Requests
- Suggest new features via GitHub issues
- Explain use cases and benefits
- Consider implementation complexity

### 🔧 Code Contributions
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

### 📚 Documentation
- Improve existing documentation
- Add examples and tutorials
- Translate to other languages

---

## 📄 License

SkyScope is released under the **MIT License** - see the [LICENSE](LICENSE) file for details.

This means you can:
- ✅ Use SkyScope for personal and commercial projects
- ✅ Modify and distribute the code
- ✅ Use it in your own projects
- ✅ Sell products based on SkyScope

**Attribution**: Please credit SkyScope in your projects and documentation.

---

## 🙏 Acknowledgments

- **ADS-B Technology**: International aviation standards and protocols
- **Arduino Community**: Open-source hardware and software ecosystem
- **Adafruit**: Excellent display libraries and documentation
- **Aviation Enthusiasts**: Inspiration and real-world testing
- **Open Source Contributors**: Code improvements and bug fixes 
# Rock Band MIDI-to-USB Drum Controller for Nintendo Wii

**Play Rock Band on your Wii using a real electronic drum kit!**

This firmware project enables standard MIDI drum kits (like the **Alesis Nitro Mesh Kit**) to work as authentic Rock Band drum controllers on the **Nintendo Wii**. The device converts MIDI output from electronic drums into the exact USB HID protocol that the Wii expects from official Harmonix Rock Band drum controllers.

## Table of Contents

- [Overview](#overview)
- [Why This Project Exists](#why-this-project-exists)
- [Compatible Drum Kits](#compatible-drum-kits)
- [Quick Start](#quick-start)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Architecture](#architecture)
- [MIDI Note Mapping](#midi-note-mapping)
- [Building the Firmware](#building-the-firmware)
- [Flashing the Device](#flashing-the-device)
- [Testing and Validation](#testing-and-validation)
- [Project Structure](#project-structure)
- [Development Workflow](#development-workflow)
- [How It Works](#how-it-works)
- [Future Improvements](#future-improvements)
- [Frequently Asked Questions](#frequently-asked-questions)
- [License](#license)

## Overview

This project implements a MIDI-to-USB HID converter that allows electronic drum kits with MIDI output to function as Rock Band drum controllers on the Nintendo Wii. It runs on an AVR microcontroller (ATmega32U4) and uses the LUFA USB library to perfectly emulate a Harmonix Rock Band drum controller.

**What It Does:**
- Connects between your electronic drum kit's MIDI OUT and your Wii's USB port
- Translates MIDI notes from your drums into Rock Band controller inputs
- Appears to the Wii as an authentic Harmonix Music drum controller (VID: 0x21BAD, PID: 0x3110)
- Maintains velocity sensitivity for realistic gameplay
- Supports cymbal hits, drum pads, and dual kick pedals

**Key Features:**
- Receives MIDI input via UART at standard MIDI baud rate (31,250 bps)
- Converts MIDI Note On/Off messages to Rock Band HID reports
- Supports velocity-sensitive drum pads (0-255 velocity levels)
- Handles both drum hits and cymbal hits with proper flagging
- Implements dual kick pedal support (orange + black pedals)
- Uses circular buffering for smooth, lag-free real-time performance
- Emulates authentic Harmonix drum controller USB descriptors
- Low latency: < 20ms from drum hit to Wii input

## Why This Project Exists

The Nintendo Wii Rock Band games only recognize official Harmonix drum controllers. Standard electronic drum kits with MIDI output cannot be used directly, even though they're much better quality instruments. This project solves that problem by creating a hardware adapter that:

1. **Speaks MIDI** - Understands the standard MIDI protocol used by electronic drum kits
2. **Speaks Rock Band** - Emulates the exact USB HID protocol the Wii expects
3. **Bridges the Gap** - Allows you to use professional e-drums like the Alesis Nitro Mesh Kit with Rock Band

### Target Use Cases

- **Primary**: Playing Rock Band on Nintendo Wii with Alesis Nitro Mesh Kit
- **General**: Using any MIDI drum kit with Rock Band on Wii
- **Potential**: Adapting other MIDI percussion instruments for rhythm games

## Compatible Drum Kits

### Tested and Working
- ✅ **Alesis Nitro Mesh Kit** - Confirmed working with Arduino Leonardo + MIDI Shield

### Should Work (MIDI-Compatible)
Any electronic drum kit with:
- Standard 5-pin MIDI OUT port
- Configurable MIDI note assignments
- Standard MIDI velocity (0-127)

Examples include:
- Alesis Surge Mesh Kit
- Alesis Command Mesh Kit
- Roland TD-series (TD-1, TD-17, TD-27, etc.)
- Yamaha DTX series
- Simmons SD series
- Behringer XD series
- Any drum kit with MIDI OUT capability

## Quick Start

### What You Need

**Tested and Recommended Setup:**
1. **Arduino Leonardo** board
2. **Arduino MIDI Shield** (e.g., SparkFun MIDI Shield or compatible)
3. **Alesis Nitro Mesh Kit** (or any MIDI drum kit)
4. **Standard MIDI Cable** (5-pin DIN)
5. **USB Cable** to connect to Nintendo Wii

**Alternative Configurations:**
- Other ATmega32U4 boards (Pro Micro, Teensy 2.0, ItsyBitsy)
- Custom MIDI input circuit (6N138 optoisolator + resistors)
- AVR ISP programmer (if not using Arduino bootloader)

### Basic Setup Flow

```
┌─────────────────┐    MIDI Cable    ┌──────────────────┐    USB Cable    ┌─────────┐
│ Alesis Nitro    │───────────────────│  ATmega32U4      │─────────────────│ Wii USB │
│ Mesh Kit        │   (5-pin DIN)     │  with Firmware   │   (Rock Band)   │  Port   │
│ (MIDI OUT)      │                   │  (This Project)  │                 │         │
└─────────────────┘                   └──────────────────┘                 └─────────┘
```

### Steps (Recommended: Arduino Leonardo + MIDI Shield)

1. **Assemble hardware**
   - Stack MIDI Shield on top of Arduino Leonardo
   - No soldering required with shield!

2. **Flash firmware**
   - Connect Leonardo to computer via USB
   - Run `make` to compile firmware
   - Flash using Arduino bootloader: `avrdude -p atmega32u4 -c avr109 -P /dev/ttyACM0 -U flash:w:rockband.hex:i`

3. **Configure Alesis Nitro drum kit**
   - Set MIDI notes on your drum kit to match the mapping table (see MIDI Note Mapping section)
   - Use drum module's MIDI settings menu

4. **Connect to Wii**
   - Plug MIDI cable from drum kit MIDI OUT to shield MIDI IN
   - Plug USB cable from Leonardo into Wii USB port
   - Power LED should light up

5. **Play Rock Band!**
   - Start Rock Band game on Wii
   - Wii should recognize it as a Harmonix drum controller
   - Calibrate in-game and start playing!

## Hardware Requirements

### What You're Building

You're building a MIDI-to-USB adapter that sits between your drum kit and the Wii:
- **Input Side**: Receives MIDI from drum kit
- **Processing**: ATmega32U4 runs this firmware
- **Output Side**: Presents as USB HID device to Wii

### Core Components

#### 1. Microcontroller Board
- **ATmega32U4** running at 16 MHz
  - Native USB 2.0 support (critical for Wii compatibility)
  - Hardware UART for MIDI input
  - 5V operation

**Tested and Working:**
- ✅ **Arduino Leonardo** - Confirmed working with MIDI shield (RECOMMENDED)

**Should Also Work:**
- **SparkFun Pro Micro** - Same ATmega32U4 chip
- **Teensy 2.0** - High quality alternative
- **Adafruit ItsyBitsy 32u4** - Tiny form factor

#### 2. MIDI Input Circuit

**Option A: MIDI Shield (RECOMMENDED - Easiest)**
- ✅ **Arduino MIDI Shield** - Confirmed working with Leonardo
- Examples: SparkFun MIDI Shield (DEV-12898), Seeed Studio MIDI Shield
- Simply stacks on top of Arduino Leonardo
- No soldering required (shield has MIDI jack built-in)
- Connects to UART1 (PD2/RX1, PD3/TX1) automatically
- **This is the easiest and recommended approach**

**Option B: Custom MIDI Input Circuit (Advanced)**
If you prefer to build your own or need a custom solution:
- MIDI IN circuit connected to UART1 (PD2/RX1, PD3/TX1)
- **Standard MIDI optoisolator circuit** (6N138 or H11L1)
  - Provides electrical isolation from drum kit
  - Required for MIDI specification compliance
- **Components needed:**
  - 6N138 optoisolator IC
  - 220Ω resistor (MIDI current limiting)
  - 270Ω resistor (optoisolator output)
  - 10kΩ resistor (pull-up)
  - 5-pin DIN MIDI jack (female)
- Requires soldering and basic electronics knowledge

#### 3. Connections
- **MIDI Cable**: From drum kit MIDI OUT to adapter MIDI IN
- **USB Cable**: From adapter to Wii USB port (front or back)

#### 4. Optional Components
- **LED indicator** on PC7 (lights up when drum pads are hit)
- **Enclosure** to protect the circuit
- **Power indicator LED**

## Software Requirements

### Build Tools
- **avr-gcc**: AVR GCC compiler toolchain
- **avr-libc**: Standard C library for AVR
- **make**: Build automation tool
- **avrdude**: Firmware programming utility (for flashing)

### Installation (macOS)
```bash
brew install avr-gcc avrdude
```

### Installation (Linux/Debian)
```bash
sudo apt-get install gcc-avr avr-libc avrdude make
```

### Dependencies
- **LUFA** (Lightweight USB Framework for AVRs) - included in `vendor/lufa/`
  - Version: Compatible with ATmega32U4
  - Used for USB device enumeration and HID reports

### Testing Tools (Optional)
- **Python 3** with libraries:
  - `scapy` - For USB packet analysis
  - `pyusb` - For USB device interaction
- **Wireshark** - For capturing and analyzing USB traffic

## Architecture

### High-Level Data Flow

```
MIDI Drum Pad → MIDI Cable → UART RX (Interrupt) →
MIDI Parser → Note Mapping → HID Report Builder →
Circular Buffer → USB Endpoint → Host (PC/Console)
```

### Core Components

1. **UART MIDI Receiver** (`rockband.c:145-165`)
   - Interrupt-driven UART reception at 31,250 baud
   - ISR assembles complete MIDI messages
   - Handles running status and multi-byte messages

2. **MIDI Message Parser** (`rockband.c:185-205`)
   - Parses Note On/Off and velocity
   - Extracts channel, note number, and velocity
   - Sets completion flag for main loop processing

3. **Note Mapper** (`rockband.c:100-113`)
   - Maps MIDI note numbers to drum pad positions
   - Distinguishes between drum hits and cymbal hits
   - Supports kick pedals (standard and secondary)

4. **HID Report Generator** (`rockband.c:241-285`)
   - Constructs 28-byte HID reports matching Rock Band format
   - Encodes button states, velocity, and cymbal flags
   - Maintains state between reports

5. **Circular Buffer** (`rockband.c:22-68`)
   - Stores up to 8 HID reports
   - Prevents data loss during USB polling intervals
   - Implements thread-safe FIFO queue

6. **USB HID Interface** (`rockband.c:295-305`)
   - Services USB IN/OUT endpoints
   - Sends buffered reports when host polls
   - Handles USB enumeration and descriptors

## MIDI Note Mapping

### Configuring Your Alesis Nitro Mesh Kit

The Alesis Nitro Mesh Kit needs to be configured to send specific MIDI notes that match Rock Band's drum layout. Use the drum module's settings to assign these notes:

| Your Drum Pad      | MIDI Note | Hex  | Decimal | Rock Band Pad        | Color  | Type   |
|--------------------|-----------|------|---------|----------------------|--------|--------|
| Kick Pedal         | C2        | 0x24 | 36      | Primary Kick         | Orange | Pedal  |
| Hi-Hat Pedal       | G#2       | 0x2C | 44      | Secondary Kick       | Black  | Pedal  |
| Snare (Pad)        | D2        | 0x26 | 38      | Red Pad              | Red    | Drum   |
| Tom 1 (Pad)        | A2        | 0x2D | 45      | Blue Pad             | Blue   | Drum   |
| Tom 2 (Pad)        | G2        | 0x2B | 43      | Green Pad            | Green  | Drum   |
| Tom 3 (Pad)        | C3        | 0x30 | 48      | Yellow Pad           | Yellow | Drum   |
| Crash 1 (Cymbal)   | C#3       | 0x31 | 49      | Blue Cymbal          | Blue   | Cymbal |
| Crash 2 (Cymbal)   | D#3       | 0x33 | 51      | Green Cymbal         | Green  | Cymbal |
| Ride (Cymbal)      | A#2       | 0x2E | 46      | Yellow Cymbal        | Yellow | Cymbal |

### Important Notes

1. **Cymbals vs Pads**: The firmware distinguishes between cymbal hits and pad hits on the same color
   - Example: Blue pad (tom) and Blue cymbal (crash) are different inputs
   - Rock Band games treat these differently for scoring

2. **Dual Kick Support**: Two kick pedals supported
   - Primary (C2): Orange pedal in game
   - Secondary (G#2): Black/Plus pedal in game (often mapped to hi-hat pedal)

3. **Velocity Sensitive**: All pads support full MIDI velocity (0-127)
   - Softer hits register as lower velocity in game
   - Harder hits register as higher velocity

### Complete MIDI Note Reference

| MIDI Note | Hex  | Decimal | Name          | Rock Band Pad        | Cymbal |
|-----------|------|---------|---------------|----------------------|--------|
| G#2       | 0x2C | 44      | Pedal (Hi-Hat)| Secondary Kick (Plus)| No     |
| C2        | 0x24 | 36      | Kick          | Primary Kick (Orange)| No     |
| C#3       | 0x31 | 49      | Crash 1       | Blue Cymbal          | Yes    |
| A#2       | 0x2E | 46      | Ride          | Yellow Cymbal        | Yes    |
| D#3       | 0x33 | 51      | Crash 2       | Green Cymbal         | Yes    |
| A2        | 0x2D | 45      | Tom 1         | Blue Pad             | No     |
| G2        | 0x2B | 43      | Tom 2         | Green Pad            | No     |
| D2        | 0x26 | 38      | Snare         | Red Pad              | No     |
| C3        | 0x30 | 48      | Tom 3         | Yellow Pad           | No     |

### How to Configure Alesis Nitro Mesh Kit

1. **Enter MIDI Settings Mode** on the drum module
2. **For each pad/cymbal**, assign the MIDI note from the table above
3. **Set MIDI Channel** to 1 (or any channel - firmware ignores channel)
4. **Set Velocity Curve** to Linear (for best response)
5. **Save Settings** to drum module memory

Consult your Alesis Nitro Mesh Kit manual for detailed instructions on changing MIDI note assignments.

### Mapping Logic in Code (`rockband.c:100-113`)
- Cymbal hits set cymbal flag (0xB0 | pad_number)
- Drum hits map to pad number (0-3)
- Kick pedals have dedicated encodings (0xA0, 0xA1)

## Building the Firmware

### Quick Start

```bash
# Build the firmware
make

# This produces:
# - rockband.elf (executable with debug symbols)
# - rockband.hex (Intel HEX file for flashing)
# - rockband.o, Descriptors.o (object files)
```

### Build Configuration

The build is configured in `Makefile`:
- **MCU**: atmega32u4
- **F_CPU**: 16000000UL (16 MHz)
- **Optimization**: -Os (optimize for size)
- **LUFA Path**: vendor/lufa/LUFA

### Clean Build

```bash
# Remove all build artifacts
make clean

# Rebuild from scratch
make clean && make
```

## Flashing the Device

### Using avrdude

```bash
# Flash the firmware using ISP programmer
make flash

# Or manually:
avrdude -p atmega32u4 -c avrispmkII -P usb -U flash:w:rockband.hex:i
```

### Programmer Options

Edit `Makefile` if using a different programmer:
```makefile
PROGRAMMER = avrispmkII  # Change to: usbasp, arduino, etc.
PORT = usb               # Change to: /dev/ttyACM0, COM3, etc.
```

### Using Arduino Bootloader

If using an Arduino Leonardo or similar with bootloader:
```bash
avrdude -p atmega32u4 -c avr109 -P /dev/ttyACM0 -U flash:w:rockband.hex:i
```

## Testing and Validation

### Python Test Scripts

The project includes several Python utilities for testing:

#### 1. USB Packet Analyzer (`test01.py`)
Analyzes USB traffic captures to validate HID reports:
```python
# Requires a pcap file from Wireshark USB capture
python3 test01.py
```

Features:
- Decodes USB token packets (IN/OUT/SETUP)
- Parses DATA0/DATA1 packets
- Validates HID report structure
- Filters endpoint 1 traffic (drum reports)

#### 2. CRC Test (`crc_test.py`, `crc_usb.py`)
Validates USB packet CRC calculations

#### 3. Validator Scripts (`validator.py`, `validator2.py`)
Checks HID report format compliance

### Manual Testing

1. **Connect MIDI Source**: Connect electronic drums or MIDI controller
2. **Monitor USB**: Use `lsusb` or Device Manager to verify enumeration
3. **Check Descriptor**: Confirm device appears as "Harmonix Music" vendor
4. **Test Input**: Play drums and verify game response

### USB Descriptor Validation (`negotiation.txt`)

The file `negotiation.txt` contains the captured USB descriptors showing:
- **Vendor ID**: 0x21BAD (Harmonix Music)
- **Product ID**: 0x3110
- **Device Class**: HID (Human Interface Device)
- **Endpoints**: IN (0x81) and OUT (0x02), 64-byte interrupt transfers

## Project Structure

```
rockband/
├── Config/
│   └── LUFAConfig.h          # LUFA library configuration
├── docs/                      # Reference documentation
│   ├── README.md             # Documentation guide
│   ├── usb_descriptor_reference.txt  # Captured USB descriptors
│   └── development_notes.ipynb       # Development notebook
├── tools/                     # Testing and debugging utilities
│   ├── README.md             # Tools documentation
│   ├── requirements.txt      # Python dependencies
│   ├── usb_packet_analyzer.py    # Analyze USB pcap files
│   └── hid_report_monitor.py     # Monitor live HID reports
├── vendor/
│   └── lufa/                 # LUFA USB framework (submodule)
├── rockband.c                # Main firmware source code
├── rockband.h                # Main header file
├── Descriptors.c             # USB descriptors implementation
├── Descriptors.h             # USB descriptors header
├── Makefile                  # Build configuration
├── LICENSE                   # MIT License + LUFA attribution
├── CONTRIBUTING.md           # Contribution guidelines
├── README.md                 # This file
└── .gitignore                # Git ignore rules
```

### Key Files

#### Core Firmware
- **`rockband.c`**: Main firmware logic (370 lines)
  - UART MIDI receiver with ISR
  - MIDI message parser
  - Note-to-HID mapper
  - Circular buffer implementation
  - USB HID endpoint handlers

- **`Descriptors.c/.h`**: USB device descriptors
  - Device descriptor (Harmonix VID/PID)
  - Configuration descriptor
  - HID report descriptor (defines 28-byte report format)
  - String descriptors

- **`rockband.h`**: Main header file
  - Function prototypes
  - USB event handlers
  - LUFA includes

#### Build System
- **`Makefile`**: Build configuration
  - AVR-GCC compilation flags
  - LUFA source file inclusion
  - Flash programming targets

- **`Config/LUFAConfig.h`**: LUFA library settings
  - USB device mode configuration
  - Endpoint sizes and options

#### Documentation
- **`README.md`**: Complete project documentation (this file)
- **`LICENSE`**: MIT License with LUFA attribution
- **`CONTRIBUTING.md`**: Developer contribution guide
- **`docs/README.md`**: Reference documentation guide
- **`tools/README.md`**: Testing tools documentation

#### Testing Tools (Python)
- **`tools/usb_packet_analyzer.py`**: Analyze Wireshark USB captures
- **`tools/hid_report_monitor.py`**: Monitor live HID reports from device
- **`tools/requirements.txt`**: Python package dependencies

#### Reference Materials
- **`docs/usb_descriptor_reference.txt`**: Captured USB descriptors from real hardware
- **`docs/development_notes.ipynb`**: Jupyter notebook with research notes

## Development Workflow

### Setting Up Development Environment

1. **Clone the repository**
```bash
git clone <repository-url>
cd rockband
```

2. **Initialize LUFA submodule** (if not present)
```bash
git submodule update --init --recursive
```

3. **Build the firmware**
```bash
make
```

4. **Connect programmer** and flash
```bash
make flash
```

### Modifying Note Mappings

To change MIDI note mappings, edit the `map_note()` function in `rockband.c:100-113`:

```c
uint8_t map_note(uint8_t x) {
    switch (x) {
        case 0x2C: return PEDAL;      // Change MIDI note for pedal
        case 0x24: return KICK;       // Change MIDI note for kick
        case 0x31: return CYMBAL | 0; // Blue cymbal
        // ... add more mappings
    }
}
```

### Debugging

1. **LED Indicator**: The firmware toggles LED on PC7 when drum pads are hit
2. **USB Capture**: Use Wireshark to capture USB traffic and analyze with `test01.py`
3. **Serial Monitor**: UART TX is available for debug output (currently unused)

### Common Issues

#### Wii-Specific Problems

- **Wii doesn't recognize device**:
  - Verify USB descriptors match Harmonix VID/PID exactly (0x21BAD:0x3110)
  - Try different USB port on Wii (front vs back)
  - Check that device enumerates as "Harmonix Music" in USB descriptor
  - Ensure ATmega32U4 is running at 5V, not 3.3V

- **Game starts but drums don't respond**:
  - Check MIDI cable connection (MIDI OUT from drums to MIDI IN on adapter)
  - Verify MIDI note assignments on drum kit match the mapping table
  - Confirm LED on PC7 blinks when hitting drums (hardware test)
  - Check that drum kit is sending on MIDI channel 1 (or any channel)

- **Some pads work, others don't**:
  - Verify MIDI note mapping in `map_note()` function
  - Use `test01.py` to capture and analyze USB HID reports
  - Check that drum kit is sending correct MIDI notes

#### General Hardware Issues

- **Device not recognized by computer (testing)**:
  - Check USB descriptors match Rock Band format
  - Verify LUFA USB library compiled correctly
  - Test with different USB cable

- **Missed notes / Lag**:
  - Increase circular buffer size (currently 8 entries) in `rockband.c:12`
  - Check MIDI optoisolator circuit for correct resistor values
  - Verify UART baud rate is exactly 31,250

- **Wrong pad triggering**:
  - Verify MIDI note mapping in `map_note()` function
  - Check drum kit MIDI note configuration
  - Review vendor8 array encoding in HID report

- **No velocity sensitivity**:
  - Check vendor8 array encoding in HID report (`rockband.c:276-280`)
  - Verify drum kit is sending MIDI velocity values (not just on/off)
  - Set velocity curve to Linear on drum module

#### Testing Without Wii

To test the device without a Wii:
1. Connect to PC/Mac via USB
2. Check device enumeration: `lsusb` (Linux) or System Information (macOS)
3. Should show: "Harmonix Music" with VID 21BAD and PID 3110
4. Use Python test scripts to analyze HID reports

## How It Works

### USB HID Report Format (28 bytes)

The Rock Band drum controller uses a vendor-specific 28-byte HID report:

```
Byte  0-1:  Button states
Byte  2:    Hat switch (D-pad)
Byte  3-6:  Analog axes (X, Y, Z, Rz) - always 0x7F
Byte  7-18: Vendor-specific (velocity and MIDI data)
Byte 19-26: Vendor-specific 16-bit values
```

#### Button Encoding (rockband.c:115-125)

**Byte 0** (button[0]):
- Bit 0: Blue pad (1)
- Bit 1: Green pad (4)
- Bit 2: Red pad (B)
- Bit 3: Yellow pad (2)
- Bit 4: Orange kick pedal

**Byte 1** (button[1]):
- Bit 1: Plus button / Black kick pedal
- Bit 3: Cymbal flag (0x08)
- Bit 2: Drum hit flag (0x04)

#### Velocity Encoding (rockband.c:276-280)

Velocity values (0-255) are stored in vendor8[5-8]:
- vendor8[5]: Blue velocity
- vendor8[6]: Green velocity
- vendor8[7]: Red velocity
- vendor8[8]: Yellow velocity

### MIDI Processing Pipeline

1. **UART ISR** receives bytes at 31,250 baud
2. **Status byte** (>= 0x80) starts new message
3. **Data bytes** accumulate in `midi_buffer[]`
4. **Complete message** sets `midi_complete` flag
5. **Main loop** processes message and generates HID report
6. **Report pushed** to circular buffer
7. **USB endpoint** sends report when host polls (every 10ms)

### Timing Considerations

- **MIDI Baud**: 31,250 bps = 320 μs per byte
- **3-byte message**: ~1 ms transmission time
- **USB Polling**: 10 ms interval (EP interval = 0x0A)
- **Buffer capacity**: 8 reports = 80 ms max latency
- **Typical latency**: < 20 ms (drum hit to USB report)

## Future Improvements

To prepare this project for open-source release, consider:

### Code Organization

- [ ] Separate MIDI handling into dedicated module
- [ ] Extract HID report generation to separate file
- [ ] Create configuration header for note mappings
- [ ] Add comprehensive inline documentation
- [ ] Implement error handling and status codes

### Features

- [ ] Configurable note mapping via EEPROM
- [ ] Support for other drum controller types (Guitar Hero, etc.)
- [ ] MIDI learn mode for automatic mapping
- [ ] Multiple velocity curve options
- [ ] USB configuration tool for runtime settings

### Documentation

- [x] Complete README with architecture overview
- [x] MIT License with LUFA attribution
- [x] Copyright headers in all source files
- [ ] Contributing guidelines (CONTRIBUTING.md)
- [ ] Code of conduct (CODE_OF_CONDUCT.md)
- [ ] Hardware build guide with schematics and circuit diagrams
- [ ] Troubleshooting guide with photos
- [ ] API documentation (Doxygen)

### Testing

- [ ] Unit tests for circular buffer
- [ ] Integration tests for MIDI parser
- [ ] Hardware-in-the-loop test setup
- [ ] Continuous integration (GitHub Actions)
- [ ] Performance benchmarks
- [ ] Compatibility testing with different drum kits

### Build System

- [ ] Platform-independent build (CMake?)
- [ ] Pre-built firmware releases (.hex files)
- [ ] Automated version numbering
- [ ] Multiple hardware target support (different boards)
- [ ] GitHub Actions for automated builds

### Legal & Compliance

- [x] MIT License selected
- [x] LUFA library attribution in LICENSE file
- [x] Copyright headers added to all source files
- [ ] Document any patent considerations (none known)
- [ ] Verify compliance with USB-IF requirements
- [ ] Document trademark considerations (Harmonix, Rock Band)

## Frequently Asked Questions

### General Questions

**Q: Will this work with my Nintendo Wii?**
A: Yes! This project specifically targets the Nintendo Wii. The firmware emulates the exact USB descriptors and HID protocol that Wii Rock Band games expect.

**Q: Does it work with Xbox 360 or PlayStation?**
A: Not currently. This firmware is specifically designed for Wii. Xbox 360 and PlayStation use different protocols and authentication methods.

**Q: Can I use drum kits other than the Alesis Nitro Mesh Kit?**
A: Yes! Any electronic drum kit with MIDI output will work. You'll just need to configure the MIDI note assignments on your drum module to match the mapping table.

**Q: Does this work with Rock Band 4?**
A: Rock Band 4 is on Xbox One/PS4, which use different protocols. This project is for Wii (Rock Band 1, 2, 3, Beatles, etc.).

### Hardware Questions

**Q: Do I need an Arduino or can I use other boards?**
A: You need an ATmega32U4-based board. The **Arduino Leonardo with a MIDI Shield** is the tested and recommended configuration. Other Arduino boards like Uno/Mega won't work because they lack native USB support.

**Q: Can I build this without soldering?**
A: Yes! Use an **Arduino Leonardo with a MIDI Shield** - this is the recommended configuration and requires no soldering. The MIDI shield simply stacks on top of the Leonardo. This combination is confirmed working.

**Q: Where do I get the MIDI input circuit?**
A: You can either build it yourself (6N138 optoisolator + resistors), buy a MIDI shield/breakout for Arduino, or salvage one from old MIDI equipment.

**Q: How much does it cost to build?**
A: **Recommended Setup (Arduino Leonardo + MIDI Shield):**
- Arduino Leonardo: $15-25
- Arduino MIDI Shield: $15-30
- MIDI cable: $5-10
- USB cable: $3-5
- **Total: $40-70**

**Budget Setup (Custom Circuit):**
- ATmega32U4 board (Pro Micro): $5-10
- MIDI input components: $3-5
- MIDI cable: $5-10
- USB cable: $3-5
- **Total: $15-30** (requires soldering)

**Q: Does the Wii power the adapter?**
A: Yes, the ATmega32U4 is powered via USB from the Wii (5V).

### Configuration Questions

**Q: How do I configure the MIDI notes on my Alesis Nitro?**
A: Access the MIDI settings menu on the drum module (consult your manual), then assign each pad to the specific MIDI note numbers listed in the mapping table above.

**Q: Do I need to configure MIDI channels?**
A: No, the firmware currently ignores MIDI channel and responds to notes on all channels.

**Q: Can I customize the note mapping?**
A: Yes! Edit the `map_note()` function in `rockband.c` to change which MIDI notes trigger which pads.

**Q: My drum kit doesn't have enough pads for all the cymbals. What do I do?**
A: You can configure some or all of your cymbals to trigger drum pads instead. Just map the cymbal MIDI notes to the drum note numbers (0x2D, 0x2B, 0x26, 0x30).

### Troubleshooting

**Q: The Wii doesn't detect the controller at all. What's wrong?**
A: Check these in order:
1. Is the ATmega32U4 powered? (Should have LED or power indicator)
2. Is the firmware flashed correctly?
3. Try a different USB port on the Wii
4. Verify USB descriptors match Harmonix VID/PID (use lsusb on computer)

**Q: Some pads work but others don't. Why?**
A: Usually a MIDI configuration issue:
1. Verify your drum kit is sending the correct MIDI notes (check with MIDI monitor software)
2. Confirm the MIDI cable is good and connected to MIDI OUT (not MIDI IN)
3. Check the note mapping in `map_note()` function

**Q: There's lag between hitting the drum and the game response. How do I fix it?**
A: This is unusual (should be < 20ms). Check:
1. MIDI baud rate is exactly 31,250
2. Circular buffer isn't overflowing
3. MIDI optoisolator circuit has correct resistor values
4. USB cable is good quality

**Q: The velocity doesn't work - all hits are the same volume. Why?**
A: Check:
1. Your drum kit is sending velocity data (not fixed velocity)
2. The velocity curve on your drum module is set to "Linear" or "Dynamic"
3. The vendor8 array in the HID report is being populated correctly

### Software Questions

**Q: Do I need to know C programming to use this?**
A: Not if you use the default configuration. Just compile and flash. Only needed if you want to customize the MIDI mapping.

**Q: Can I test this without a Wii?**
A: Yes! Connect to a PC/Mac and verify:
- Device enumerates as "Harmonix Music"
- USB descriptors are correct (VID: 0x21BAD, PID: 0x3110)
- Use Python test scripts to analyze HID reports

**Q: What's the difference between this and other MIDI-to-USB adapters?**
A: Generic MIDI-to-USB adapters present as MIDI devices. This presents as a Rock Band drum controller, which is what the Wii expects.

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for full details.

### LUFA Library Attribution

This project uses the **LUFA (Lightweight USB Framework for AVRs)** library, which is:
- Copyright (C) Dean Camera, 2021
- Website: [www.lufa-lib.org](https://www.lufa-lib.org)
- Licensed under a permissive license (included in the LICENSE file)

LUFA's permissive license allows free use, modification, and distribution of both the library and derivative works. The MIT license used for this project's original code is fully compatible with LUFA's license terms.

### Third-Party Components

- **LUFA Library**: Located in `vendor/lufa/` - See LUFA license in LICENSE file
- **Original Firmware**: `rockband.c`, `rockband.h`, `Descriptors.c` - MIT Licensed

### Attribution Requirements

When using or distributing this project:
1. Include the LICENSE file with both MIT and LUFA license terms
2. Maintain copyright notices in source files
3. Credit Dean Camera for the LUFA library
4. Credit this project's contributors

### Commercial Use

Both the MIT License and LUFA's license permit commercial use. You are free to:
- Use this project commercially
- Modify and sell derivative products
- Integrate into commercial products

No attribution in end-user documentation is required, but attribution in source code distributions is required per the license terms.

---

## Quick Reference

### Build Commands
```bash
make          # Build firmware
make clean    # Remove build artifacts
make flash    # Flash to device
```

### Physical Setup
```
Alesis Nitro → MIDI Cable → Adapter → USB Cable → Wii USB Port
```

### Hardware Connections (ATmega32U4)
- **MIDI IN** → PD2 (UART RX1)
- **Status LED** → PC7 (optional)
- **USB Data** → D+ / D- (native USB)
- **Power** → 5V from USB

### Wii Compatibility
- **USB Vendor ID**: 0x21BAD (Harmonix Music)
- **USB Product ID**: 0x3110 (Rock Band Drums)
- **Device Class**: HID (Human Interface Device)
- **Compatible Games**: Rock Band 1, 2, 3, Beatles, etc.

### MIDI Configuration for Alesis Nitro
| Pad Type | MIDI Note | Rock Band Color |
|----------|-----------|-----------------|
| Kick     | C2 (36)   | Orange          |
| Snare    | D2 (38)   | Red             |
| Tom 1    | A2 (45)   | Blue (Pad)      |
| Tom 2    | G2 (43)   | Green (Pad)     |
| Tom 3    | C3 (48)   | Yellow (Pad)    |
| Crash 1  | C#3 (49)  | Blue (Cymbal)   |
| Crash 2  | D#3 (51)  | Green (Cymbal)  |
| Ride     | A#2 (46)  | Yellow (Cymbal) |

### Technical Specifications
- **MCU**: ATmega32U4 @ 16 MHz
- **MIDI**: 31,250 baud, 8N1, UART with interrupt
- **USB**: Full-speed (12 Mbps), HID device
- **Endpoints**: IN (0x81) and OUT (0x02), 64-byte, 10ms polling
- **Latency**: < 20ms (drum hit to Wii input)
- **Buffer**: 8-entry circular buffer for smooth performance

### Testing Checklist
- [ ] Firmware compiles without errors
- [ ] Device flashed successfully
- [ ] LED blinks when hitting drums (hardware test)
- [ ] Computer recognizes device as "Harmonix Music" (enumeration test)
- [ ] Wii recognizes drum controller
- [ ] All pads/cymbals respond in Rock Band game
- [ ] Velocity sensitivity working
- [ ] No missed notes during fast play

---

## Summary

This project enables the **Alesis Nitro Mesh Kit** (and other MIDI drum kits) to work as authentic **Rock Band drum controllers** on the **Nintendo Wii**. The adapter sits between your drums and the Wii, translating MIDI messages into the exact USB HID protocol the Wii expects from official Harmonix controllers.

### Recommended Hardware Configuration (Tested & Working)
- ✅ **Arduino Leonardo** board
- ✅ **Arduino MIDI Shield** (SparkFun or compatible)
- ✅ **Alesis Nitro Mesh Kit** (or any MIDI drum kit)
- **No soldering required!**

**Result**: Professional-quality mesh drum pads instead of plastic game drums, with full velocity sensitivity and cymbal support!

**Happy Drumming!** 🥁🎮

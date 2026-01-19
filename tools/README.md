# Rock Band MIDI-to-USB Testing Tools

This directory contains Python tools for testing, debugging, and validating the Rock Band MIDI-to-USB drum controller firmware.

## Tools Overview

### 1. usb_packet_analyzer.py
Analyzes USB packet captures (pcap files) from Wireshark to decode and validate HID reports.

**Purpose:** Debug USB communication between the device and Wii

**Requirements:**
```bash
pip install scapy
```

**Usage:**
```bash
# Capture USB traffic in Wireshark, save as capture.pcap
python3 usb_packet_analyzer.py capture.pcap

# Verbose mode (show all packets)
python3 usb_packet_analyzer.py capture.pcap --verbose
```

**What it does:**
- Decodes USB packet types (TOKEN, DATA0, DATA1, etc.)
- Filters for HID reports (DATA1 packets on endpoint 1)
- Shows only non-default reports (actual drum hits)
- Helps identify malformed or incorrect HID reports

---

### 2. hid_report_monitor.py
Monitors live HID reports from a connected Rock Band drum controller device.

**Purpose:** Real-time testing of firmware HID output

**Requirements:**
```bash
# Option 1: HID API (recommended)
pip install hidapi

# Option 2: PyUSB
pip install pyusb
```

**Usage:**
```bash
# Using HID API (recommended)
python3 hid_report_monitor.py --method hidapi

# Using PyUSB
python3 hid_report_monitor.py --method pyusb

# May require sudo on Linux
sudo python3 hid_report_monitor.py
```

**What it does:**
- Connects to device (VID: 0x1BAD, PID: 0x3110)
- Reads HID reports in real-time
- Shows only changed reports (filters duplicates)
- Displays hex data and basic button decoding
- Useful for testing firmware changes

---

## Development Workflow

### Testing Firmware Changes

1. **Build and flash firmware:**
   ```bash
   cd ..
   make clean && make
   make flash
   ```

2. **Monitor HID reports:**
   ```bash
   cd tools
   python3 hid_report_monitor.py
   ```

3. **Hit drums on Alesis kit** - observe HID reports change

4. **Verify report format:**
   - Check button bytes (0-1)
   - Check velocity values (vendor8[5-8])
   - Verify cymbal flags

### Capturing USB Traffic

**On Linux:**
```bash
# Install Wireshark
sudo apt-get install wireshark

# Allow USB capture
sudo usermod -a -G wireshark $USER
# Log out and back in

# Start Wireshark, select usbmonX interface
# Filter: usb.device_address == X && usb.endpoint_address == 0x81

# Save capture as .pcap file
# Analyze with: python3 usb_packet_analyzer.py capture.pcap
```

**On macOS:**
```bash
# Wireshark doesn't support USB capture on macOS
# Use Instruments.app or USB Prober instead
# Or use a Linux VM for packet capture
```

### Verifying Wii Compatibility

1. **Test on PC first:**
   ```bash
   # Check device enumeration
   lsusb -v -d 1bad:3110  # Linux
   # or use System Information on macOS

   # Monitor HID reports
   python3 hid_report_monitor.py
   ```

2. **Test on Wii:**
   - Plug device into Wii USB port
   - Start Rock Band game
   - Check controller calibration menu
   - Play test songs

3. **Debug issues:**
   - Capture USB traffic if not recognized
   - Verify descriptors match reference
   - Check HID report timing and format

## Troubleshooting

### "Device not found"
- Check device is connected via USB
- Verify VID:PID with `lsusb` (should be 1bad:3110)
- Try with sudo/admin privileges

### "Permission denied"
**Linux:**
```bash
# Add udev rule
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1bad", ATTR{idProduct}=="3110", MODE="0666"' | sudo tee /etc/udev/rules.d/99-rockband.rules
sudo udevadm control --reload-rules
```

**macOS:**
```bash
# May need to run with sudo
sudo python3 hid_report_monitor.py
```

### "Module not found"
```bash
# Install required Python packages
pip install scapy pyusb hidapi

# Or install all at once
pip install -r requirements.txt  # (if we create one)
```

## Creating a requirements.txt

For easier dependency management:
```bash
# Create requirements.txt
cat > requirements.txt << EOF
scapy>=2.4.5
pyusb>=1.2.1
hidapi>=0.12.0
EOF

# Install all dependencies
pip install -r requirements.txt
```

## Advanced Usage

### Automated Testing

You can use these tools in automated test scripts:

```python
#!/usr/bin/env python3
# test_firmware.py

import subprocess
import time

# Flash firmware
subprocess.run(["make", "flash"], cwd="..")
time.sleep(2)  # Wait for device to enumerate

# Start monitoring
subprocess.run(["python3", "hid_report_monitor.py", "--method", "hidapi"])
```

### Continuous Integration

These tools can be integrated into CI/CD pipelines for automated firmware validation (requires hardware-in-the-loop test setup).

## Contributing

If you create new testing tools:
1. Add them to this directory
2. Document them in this README
3. Include usage examples
4. Add any new dependencies to requirements.txt
5. Submit a pull request

---

For more information, see the main [README.md](../README.md) and [CONTRIBUTING.md](../CONTRIBUTING.md).

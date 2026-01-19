# Rock Band MIDI-to-USB Documentation

This directory contains reference documentation and development notes for the Rock Band MIDI-to-USB drum controller project.

## Files

### usb_descriptor_reference.txt
USB descriptor capture from the original Harmonix Rock Band drum controller. This file shows the exact USB device descriptors that the Wii expects from an official Rock Band drum controller.

**Contents:**
- Device descriptor (VID: 0x1BAD, PID: 0x3110)
- Configuration descriptor
- Interface descriptor (HID class)
- Endpoint descriptors (IN: 0x81, OUT: 0x02)
- HID descriptor details

**Use this file to:**
- Verify your firmware's USB descriptors match the original
- Understand the expected USB enumeration sequence
- Debug USB compatibility issues with the Wii

### development_notes.ipynb
Jupyter notebook with development notes, experiments, and analysis performed during the creation of this project.

**Contents:**
- Protocol analysis notes
- MIDI-to-HID mapping experiments
- USB traffic analysis
- Performance testing results

**To view:**
```bash
jupyter notebook development_notes.ipynb
```

## Additional Resources

### Official Documentation
- **LUFA Library**: See `vendor/lufa/` for USB library documentation
- **Main README**: See `../README.md` for project overview
- **Contributing Guide**: See `../CONTRIBUTING.md` for development guidelines

### External References
- **USB HID Specification**: [USB.org HID Specification](https://www.usb.org/hid)
- **MIDI Specification**: [midi.org](https://www.midi.org/specifications)
- **AVR Documentation**: [Microchip ATmega32U4 Datasheet](https://www.microchip.com/en-us/product/ATmega32U4)

## Research Notes

### How the Descriptors Were Captured

The USB descriptors in `usb_descriptor_reference.txt` were captured using:
1. **Linux lsusb**: `lsusb -v -d 1bad:3110`
2. **Wireshark**: USB traffic capture during enumeration
3. **macOS System Information**: USB device tree

This allowed reverse-engineering the exact USB descriptors needed for Wii compatibility.

### Key Findings

1. **Vendor/Product ID**: Must be exactly 0x1BAD:0x3110 (Harmonix Music)
2. **Device Class**: HID (0x03) defined at interface level
3. **Endpoint Configuration**:
   - IN (0x81): Interrupt, 64 bytes, 10ms interval
   - OUT (0x02): Interrupt, 64 bytes, 10ms interval
4. **HID Report**: 28-byte proprietary format (see main README)

### Testing Methodology

During development, the following testing approach was used:
1. Capture USB traffic from real Rock Band drums
2. Analyze HID report format and timing
3. Map MIDI notes from Alesis kit to HID reports
4. Verify Wii recognizes emulated controller
5. Test in-game with Rock Band software

## Contributing Documentation

If you discover new information or make improvements, please:
1. Update relevant documentation files
2. Add notes to development_notes.ipynb
3. Submit a pull request with clear descriptions
4. Reference any external sources used

---

For questions or contributions, see the main [CONTRIBUTING.md](../CONTRIBUTING.md) file.

# Contributing to Rock Band MIDI-to-USB Drum Controller

Thank you for your interest in contributing to this project! This document provides guidelines for contributing.

## How to Contribute

### Reporting Bugs

If you find a bug, please open an issue on GitHub with:
- A clear description of the problem
- Steps to reproduce the issue
- Your hardware configuration (board, drum kit, etc.)
- Expected vs actual behavior
- Any error messages or logs

### Suggesting Enhancements

Enhancement suggestions are welcome! Please open an issue describing:
- The enhancement you'd like to see
- Why it would be useful
- Any implementation ideas you have

### Pull Requests

We welcome pull requests! Here's the process:

1. **Fork the repository** and create a branch for your feature/fix
2. **Make your changes** following the code style guidelines below
3. **Test thoroughly** on real hardware if possible
4. **Update documentation** (README, code comments) as needed
5. **Submit a pull request** with a clear description of your changes

## Code Style Guidelines

### C Code Style

- **Indentation**: Use tabs, not spaces
- **Braces**: K&R style (opening brace on same line for functions, same line for control structures)
- **Naming**:
  - Functions: `snake_case` (e.g., `cb_push`, `uart_init`)
  - Variables: `snake_case` (e.g., `midi_buffer`, `report`)
  - Constants/Defines: `UPPER_CASE` (e.g., `MIDI_BAUD`, `LED_PIN`)
  - Types: `PascalCase_t` (e.g., `HIDReport_t`, `CircularBuffer_t`)
- **Comments**: Use `//` for single-line, `/* */` for multi-line
- **Line length**: Try to keep lines under 100 characters

### Example

```c
void uart_send(uint8_t data) {
    // Wait for empty transmit buffer
    while (!(UCSR1A & (1 << UDRE1)));
    // Put data into buffer
    UDR1 = data;
}
```

## Testing Your Changes

### Hardware Testing

If possible, test your changes on actual hardware:
- Arduino Leonardo + MIDI Shield
- Alesis Nitro Mesh Kit (or compatible MIDI drum kit)
- Nintendo Wii with Rock Band game

### Software Testing

- Compile firmware: `make clean && make`
- Check for warnings (fix all warnings)
- Test USB enumeration on PC/Mac
- Verify HID reports using included Python test scripts

## Development Workflow

1. **Set up development environment**
   ```bash
   git clone https://github.com/yourusername/rockband.git
   cd rockband
   git submodule update --init --recursive
   make
   ```

2. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Make changes and test**
   ```bash
   make clean && make
   make flash  # if you have hardware
   ```

4. **Commit with clear messages**
   ```bash
   git commit -m "Add support for XYZ feature"
   ```

5. **Push and create pull request**
   ```bash
   git push origin feature/your-feature-name
   ```

## Areas Where We Need Help

### High Priority
- **Hardware schematics**: Circuit diagrams for MIDI input
- **Testing**: Verification on different drum kits and boards
- **Documentation**: Build guide with photos/diagrams
- **Platform support**: Testing on Windows, Linux, macOS

### Medium Priority
- **Additional features**: Configurable note mapping, MIDI learn mode
- **Code organization**: Refactoring for better modularity
- **Build system**: CMake support, automated releases
- **Testing**: Unit tests, integration tests

### Low Priority
- **Performance**: Optimize latency and buffer usage
- **Other platforms**: Support for Xbox 360, PlayStation (different protocols)
- **GUI tools**: Configuration utility for note mapping

## Compatibility Testing

If you have access to hardware we haven't tested, please help by testing and reporting:

### Drum Kits
- Different Alesis models (Surge, Command, etc.)
- Roland TD-series
- Yamaha DTX series
- Other MIDI drum kits

### Microcontroller Boards
- SparkFun Pro Micro
- Teensy 2.0
- Adafruit ItsyBitsy 32u4
- Other ATmega32U4 boards

### MIDI Shields/Circuits
- Different MIDI shield models
- Custom MIDI circuits
- MIDI breakout boards

## Questions?

If you have questions about contributing:
- Open an issue on GitHub
- Check existing issues and pull requests
- Review the README for technical details

## License

By contributing to this project, you agree that your contributions will be licensed under the MIT License (see LICENSE file).

## Code of Conduct

Please be respectful and constructive in all interactions. We want this to be a welcoming community for everyone interested in rhythm games and hardware hacking!

---

Thank you for contributing! 🥁🎮

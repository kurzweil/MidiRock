#!/usr/bin/env python3
"""
HID Report Monitor for Rock Band MIDI-to-USB Drum Controller

This tool monitors live HID reports from the Rock Band drum controller device
connected via USB. Useful for testing and debugging the firmware.

Usage:
    python3 hid_report_monitor.py [--method pyusb|hidapi]

Requirements:
    # For PyUSB method:
    pip install pyusb

    # For HID API method (recommended):
    pip install hidapi

Example:
    python3 hid_report_monitor.py --method hidapi

Output:
    - Real-time HID reports as hex data
    - Only shows reports that change (filters duplicates)
    - Press Ctrl+C to exit
"""

import sys
import argparse

# Harmonix Rock Band Drums VID/PID
VID = 0x1BAD
PID = 0x3110


def monitor_with_hidapi():
    """Monitor HID reports using hidapi library (recommended)."""
    try:
        import hid
    except ImportError:
        print("Error: hidapi not installed")
        print("Install with: pip install hidapi")
        sys.exit(1)

    print(f"Connecting to device (VID: 0x{VID:04X}, PID: 0x{PID:04X})...")

    try:
        h = hid.device()
        h.open(VID, PID)
        h.set_nonblocking(1)
    except Exception as e:
        print(f"Error: Could not open device - {e}")
        print("\nTroubleshooting:")
        print("1. Ensure device is connected via USB")
        print("2. Check device is recognized: lsusb (Linux) or System Information (macOS)")
        print("3. You may need sudo/admin privileges")
        sys.exit(1)

    print("Connected! Monitoring HID reports (Ctrl+C to exit)...\n")

    last = None
    report_count = 0

    try:
        while True:
            data = h.read(64)  # Read up to 64 bytes (HID report size)
            if data and data != last:
                report_count += 1
                hex_data = ' '.join(f'{b:02X}' for b in data)
                print(f"Report #{report_count:04d}: {hex_data}")

                # Decode basic button state if first bytes match expected format
                if len(data) >= 3:
                    buttons = data[0]
                    hat = data[2]
                    print(f"  Buttons: 0x{buttons:02X}, Hat: 0x{hat:02X}")

                last = data

    except KeyboardInterrupt:
        print(f"\n\nExiting... (received {report_count} unique reports)")
        h.close()


def monitor_with_pyusb():
    """Monitor HID reports using PyUSB library."""
    try:
        import usb.core
        import usb.util
    except ImportError:
        print("Error: pyusb not installed")
        print("Install with: pip install pyusb")
        sys.exit(1)

    print(f"Finding device (VID: 0x{VID:04X}, PID: 0x{PID:04X})...")

    # Find the device
    dev = usb.core.find(idVendor=VID, idProduct=PID)
    if dev is None:
        print("Error: Device not found")
        print("\nTroubleshooting:")
        print("1. Ensure device is connected via USB")
        print("2. Check device is recognized: lsusb (Linux)")
        print("3. You may need sudo privileges: sudo python3 hid_report_monitor.py")
        sys.exit(1)

    # Set configuration 1
    try:
        dev.set_configuration(1)
    except usb.core.USBError as e:
        print(f"Error setting configuration: {e}")
        print("Try running with sudo/admin privileges")
        sys.exit(1)

    # Select interface 0
    cfg = dev.get_active_configuration()
    intf = cfg[(0, 0)]

    # Find endpoint 1 IN (address 0x81)
    endpoint = usb.util.find_descriptor(
        intf,
        custom_match=lambda e:
            usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN and
            e.bEndpointAddress == 0x81
    )

    if endpoint is None:
        print("Error: Endpoint 0x81 not found")
        sys.exit(1)

    print(f"Connected! Reading from endpoint 0x81 (Ctrl+C to exit)...\n")

    report_count = 0

    try:
        while True:
            try:
                data = dev.read(endpoint.bEndpointAddress, endpoint.wMaxPacketSize, timeout=5000)
                if data:
                    report_count += 1
                    hex_data = ' '.join(f'{b:02X}' for b in data)
                    print(f"Report #{report_count:04d}: {hex_data}")

            except usb.core.USBError as e:
                if e.errno == 110:  # timeout
                    print("(timeout, waiting for data...)")
                else:
                    raise

    except KeyboardInterrupt:
        print(f"\n\nExiting... (received {report_count} reports)")


def main():
    parser = argparse.ArgumentParser(
        description="Monitor HID reports from Rock Band drum controller",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    parser.add_argument(
        "--method",
        choices=["pyusb", "hidapi"],
        default="hidapi",
        help="USB library to use (default: hidapi)"
    )

    args = parser.parse_args()

    print("=" * 80)
    print("Rock Band MIDI-to-USB Drum Controller - HID Report Monitor")
    print("=" * 80)
    print()

    if args.method == "hidapi":
        monitor_with_hidapi()
    else:
        monitor_with_pyusb()


if __name__ == "__main__":
    main()

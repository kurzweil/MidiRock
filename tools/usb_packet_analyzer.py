#!/usr/bin/env python3
"""
USB Packet Analyzer for Rock Band MIDI-to-USB Drum Controller

This tool analyzes USB traffic captures (pcap files) from Wireshark to debug
and validate the HID reports sent by the Rock Band drum controller.

Usage:
    python3 usb_packet_analyzer.py <pcap_file>

Requirements:
    pip install scapy

Example:
    # Capture USB traffic in Wireshark, save as capture.pcap
    python3 usb_packet_analyzer.py capture.pcap

Output:
    - Decoded USB packets (TOKEN, DATA, etc.)
    - HID report data for endpoint 1 (drum reports)
    - Filtering for DATA1 packets with non-default content
"""

import sys
import struct
from scapy.all import rdpcap

# USB PID (Packet Identifier) mapping
USB_PIDS = {
    0xE1: "OUT",
    0x69: "IN",
    0xA5: "SOF",
    0x2D: "SETUP",
    0xC3: "DATA0",
    0x4B: "DATA1",
    0x87: "DATA2",
    0x0F: "MDATA",
    0xD2: "ACK",
    0x5A: "NAK",
    0x1E: "STALL",
    0x96: "NYET",
    0x3C: "PRE",
    0xB4: "ERR",
    0x78: "SPLIT",
    0xF0: "PING",
    0xC6: "EXT",
}

# Default HID report (idle state - no buttons pressed)
DEFAULT_HID_REPORT = "4b0000087f7f7f7f00000000000000000000000002000200020002008b6b"


def decode_token_packet(packet_bytes):
    """Decode a 3-byte USB token packet (IN/OUT/SETUP/SOF)."""
    if len(packet_bytes) != 3:
        raise ValueError("Token packet must be 3 bytes")

    pid, low, high = struct.unpack("<BBB", packet_bytes)

    # PID validity check
    if (pid & 0xF) ^ ((pid >> 4) & 0xF) != 0xF:
        raise ValueError(f"Invalid PID: 0x{pid:02X}")

    pid_name = USB_PIDS.get(pid, f"UNKNOWN(0x{pid:02X})")

    token_bits = (high << 8) | low
    addr = token_bits & 0x7F
    endp = (token_bits >> 7) & 0x0F
    crc5 = (token_bits >> 11) & 0x1F

    return {
        "type": "token",
        "pid": pid,
        "pid_name": pid_name,
        "addr": addr,
        "endpoint": endp,
        "crc5": crc5,
        "raw": packet_bytes.hex()
    }


def decode_data_packet(packet_bytes):
    """Decode a DATA packet (variable length, >=1 byte)."""
    if len(packet_bytes) < 1:
        raise ValueError("Data packet must be at least 1 byte (PID)")

    pid = packet_bytes[0]
    pid_name = USB_PIDS.get(pid, f"UNKNOWN(0x{pid:02X})")
    data = packet_bytes[1:-2] if len(packet_bytes) > 2 else b""

    return {
        "type": "data",
        "pid": pid,
        "pid_name": pid_name,
        "length": len(data),
        "data": data.hex(),
        "raw": packet_bytes.hex()
    }


def decode_usb_packet(packet_bytes):
    """Automatically detect and decode USB packet type."""
    if len(packet_bytes) == 3:
        return decode_token_packet(packet_bytes)
    elif len(packet_bytes) >= 1:
        return decode_data_packet(packet_bytes)
    else:
        raise ValueError(f"Unknown USB packet length: {len(packet_bytes)}")


def analyze_pcap(filename, verbose=False):
    """Analyze a pcap file for Rock Band HID reports."""
    try:
        packets = rdpcap(filename)
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found")
        return
    except Exception as e:
        print(f"Error reading pcap file: {e}")
        return

    print(f"Total packets: {len(packets)}")
    print("=" * 80)
    print("Analyzing HID reports (filtering for non-default DATA1 packets)...\n")

    hid_report_count = 0

    for i, pkt in enumerate(packets):
        raw = bytes(pkt)
        try:
            decoded = decode_usb_packet(raw)

            # Filter for DATA1 packets (HID reports)
            if decoded.get("pid_name") == "DATA1":
                # Skip default/idle reports
                if decoded["raw"] != DEFAULT_HID_REPORT:
                    hid_report_count += 1
                    print(f"Packet #{i}: HID Report (non-default)")
                    print(f"  Raw: {decoded['raw']}")
                    print(f"  Length: {decoded['length']} bytes")
                    print()

            # Optionally show IN packets for endpoint 1
            elif verbose and decoded.get("pid_name") == "IN":
                if decoded.get("addr") == 2 and decoded.get("endpoint") == 1:
                    print(f"Packet #{i}: IN request to endpoint 1")
                    print(f"  {decoded}")
                    print()

        except Exception as e:
            if verbose:
                print(f"Packet #{i}: Failed to decode - {e}")

    print("=" * 80)
    print(f"Found {hid_report_count} non-default HID reports")


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        print("\nError: Please provide a pcap file")
        print("Usage: python3 usb_packet_analyzer.py <pcap_file>")
        sys.exit(1)

    pcap_file = sys.argv[1]
    verbose = "--verbose" in sys.argv or "-v" in sys.argv

    analyze_pcap(pcap_file, verbose)


if __name__ == "__main__":
    main()

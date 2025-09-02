from scapy.all import rdpcap
import struct

# Read packets from a pcap file
#packets = rdpcap("/Users/kkurzweil/Desktop/Rockband/training")
#packets = rdpcap("/Users/kkurzweil/Desktop/Rockband/velocity")
packets = rdpcap("/Users/kkurzweil/Desktop/Rockband/kick")

print(f"Total packets: {len(packets)}")



# USB PID map for readability
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

def decode_token_packet(packet_bytes):
    """Decode a 3-byte USB token packet (IN/OUT/SETUP/SOF)."""
    if len(packet_bytes) != 3:
        raise ValueError("Token packet must be 3 bytes")

    pid, low, high = struct.unpack("<BBB", packet_bytes)

    # PID check
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

def decode_control_request(packet_bytes):
    """Decode an 8-byte USB control transfer setup packet."""
    if len(packet_bytes) != 8:
        raise ValueError("Control setup packet must be 8 bytes")

    bmRequestType, bRequest, wValue, wIndex, wLength = struct.unpack("<BBHHH", packet_bytes)
    return {
        "type": "control_request",
        "bmRequestType": bmRequestType,
        "bRequest": bRequest,
        "wValue": wValue,
        "wIndex": wIndex,
        "wLength": wLength,
        "raw": packet_bytes.hex()
    }

def decode_data_packet(packet_bytes):
    """Decode a DATA packet (variable length, >=1)."""
    if len(packet_bytes) < 1:
        raise ValueError("Data packet must be at least 1 byte (PID)")

    pid = packet_bytes[0]
    pid_name = USB_PIDS.get(pid, f"UNKNOWN(0x{pid:02X})")
    data = packet_bytes[1:]  # rest is payload

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
    elif len(packet_bytes) == 8:
        return decode_control_request(packet_bytes)
    elif len(packet_bytes) >= 1:
        return decode_data_packet(packet_bytes)
    else:
        raise ValueError(f"Unknown USB packet length: {len(packet_bytes)}")

# Print details of the first 5 packets
for i, pkt in enumerate(packets[:]):
    raw = bytes(pkt)
    decoded = decode_usb_packet(raw)
    if "DATA1" == decoded.get("pid_name", None):
        if decoded["raw"] != "4b0000087f7f7f7f00000000000000000000000002000200020002008b6b":
            print(i, decoded) 
        pass
    elif "IN" == decoded.get("pid_name", None):
        #print(i, decoded)
        if decoded["addr"] == 2 and decoded["endpoint"] == 1: 
            #if str(decoded["raw"]) == "698158":
            #print(i, decoded)
            #else:
            #    print(i, decoded) 
            pass

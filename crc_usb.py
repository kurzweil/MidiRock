def crc5_usb(data: bytes) -> int:
    """
    Compute USB CRC5 for an 11-bit token payload (address + endpoint).
    Returns a 5-bit integer.
    Polynomial: x^5 + x^2 + 1 (0b00101)
    """
    poly = 0b00101
    crc = 0b11111  # shift register initialized with all 1s

    # Only 11 LSBs are used for token packets
    value = int.from_bytes(data, 'little')
    for i in range(11):
        bit = (value >> (10 - i)) & 1
        msb = (crc >> 4) & 1
        crc = ((crc << 1) & 0x1F) | 0  # shift left
        if bit ^ msb:
            crc ^= poly
    # invert bits
    crc ^= 0x1F
    return crc

def crc16_usb(data: bytes) -> int:
    """
    Compute USB CRC16 for data packets (up to 1023 bytes).
    Polynomial: x^16 + x^15 + x^2 + 1 (0x8005)
    """
    poly = 0x8005
    crc = 0xFFFF  # shift register initialized with all 1s

    for byte in data:
        for i in range(8):
            bit = ((byte >> (7 - i)) & 1) ^ ((crc >> 15) & 1)
            crc = ((crc << 1) & 0xFFFF)
            if bit:
                crc ^= poly
    # invert bits
    crc ^= 0xFFFF
    return crc

def check_usb_packet(hex_packet: str) -> bool:
    """
    Given a full USB packet in hex, check if the CRC at the end is valid.
    For simplicity, assumes the last 2 bytes are CRC16 if packet is data,
    last byte is CRC5 if token.
    """
    packet = bytes.fromhex(hex_packet.replace(" ", ""))
    pid = packet[1]  # assuming 2nd byte is PID
    # Example: if PID is 0xE1 (DATA1) or 0xC3 (DATA0), check CRC16
    if pid & 0xF0 in (0xC0, 0xD0, 0xE0, 0xF0):  # DATA0/DATA1/etc
        payload = packet[2:-2]  # data payload
        crc_recv = int.from_bytes(packet[-2:], 'little')
        return crc16_usb(payload) == crc_recv
    else:  # token packet: check CRC5
        payload = packet[2:-1]
        crc_recv = packet[-1] & 0x1F
        return crc5_usb(payload) == crc_recv

# Example usage
hex_data = """
4b1000087f7f7f7f000000000000000000000000020002000200020075c4
"""
hex_data = hex_data.strip().replace("\n", "")
print(check_usb_packet(hex_data))  # True if checksum is correct

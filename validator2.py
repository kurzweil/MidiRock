import hid

VID = 0x1BAD
PID = 0x3110

h = hid.device()
h.open(VID, PID)

h.set_nonblocking(1)

print("Reading HID reports...")
try:
    while True:
        data = h.read(27)  # HID report length
        if data:
            hex_data = ' '.join(f'{b:02X}' for b in data)
            print(f"Received: {hex_data}")
except KeyboardInterrupt:
    h.close()

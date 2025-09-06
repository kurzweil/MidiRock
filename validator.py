import usb.core
import usb.util

# Device VID and PID
VID = 0x1BAD
PID = 0x3110

# Find the device
dev = usb.core.find(idVendor=VID, idProduct=PID)
if dev is None:
    raise ValueError("Device not found")

# Set configuration 1
dev.set_configuration(1)

# Select interface 0
cfg = dev.get_active_configuration()
intf = cfg[(0, 0)]

# Endpoint 1 IN (address 0x81)
endpoint = usb.util.find_descriptor(
    intf,
    custom_match=lambda e:
        usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN and
        e.bEndpointAddress == 0x81
)

if endpoint is None:
    raise ValueError("Endpoint not found")

print("Reading from endpoint 0x81 (interrupt IN)...")

try:
    while True:
        try:
            data = dev.read(endpoint.bEndpointAddress, endpoint.wMaxPacketSize, timeout=5000)
            print("Received:", data)
        except usb.core.USBError as e:
            if e.errno == 110:  # timeout
                print("Timeout, no data")
            else:
                raise
except KeyboardInterrupt:
    print("Exiting")

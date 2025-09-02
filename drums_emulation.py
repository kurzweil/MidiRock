from facedancer import *
from facedancer import main

@use_inner_classes_automatically
class RockbandDrums(USBDevice):
    product_string       : str = "Rockband Drums (Emulated)"
    manufacturer_string  : str = "KRAD"
    serial_number_string : str = "1234"
    vendor_id            : int = 0x1BAD
    product_id           : int = 0x3110

    class DefaultConfiguration(USBConfiguration):
        class DefaultInterface(USBInterface):
            pass

    @vendor_request_handler(number=14, direction=USBDirection.IN)
    @to_device
    def handle_control_request_14(self, request):
        # Most recent request was for 1B of data.
        # Replace me with your handler.
        request.stall()

main(RockbandDrums)

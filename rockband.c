#include <avr/io.h>
#include <util/delay.h>
#include "rockband.h"

#define LED_PIN PC7

/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{
    DDRC |= (1 << LED_PIN);
	SetupHardware();

	GlobalInterruptEnable();

	for (;;)
	{
		USB_USBTask();

		uint8_t ReceivedData[VENDOR_IO_EPSIZE];
		memset(ReceivedData, 0x00, sizeof(ReceivedData));

		Endpoint_SelectEndpoint(VENDOR_OUT_EPADDR);
		if (Endpoint_IsOUTReceived())
		{
			Endpoint_Read_Stream_LE(ReceivedData, VENDOR_IO_EPSIZE, NULL);
			Endpoint_ClearOUT();

			Endpoint_SelectEndpoint(VENDOR_IN_EPADDR);
			Endpoint_Write_Stream_LE(ReceivedData, VENDOR_IO_EPSIZE, NULL);
			Endpoint_ClearIN();
		}

        PORTC ^= (1 << LED_PIN);
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	/* Disable clock division */
	clock_prescale_set(clock_div_1);
	/* Hardware Initialization */
	USB_Init();
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs. */
void EVENT_USB_Device_Connect(void)
{
	/* Indicate USB enumerating */
	//LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs.
 */
void EVENT_USB_Device_Disconnect(void)
{
	/* Indicate USB not ready */
	//LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host set the current configuration
 *  of the USB device after enumeration - the device endpoints are configured.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	/* Setup Vendor Data Endpoints */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(VENDOR_IN_EPADDR,  EP_TYPE_BULK, VENDOR_IO_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(VENDOR_OUT_EPADDR, EP_TYPE_BULK, VENDOR_IO_EPSIZE, 1);

	/* Indicate endpoint configuration success or failure */
	//LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
	// Process vendor specific control requests here
}

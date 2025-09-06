#include <avr/io.h>
#include <util/delay.h>
#include "rockband.h"

#define LED_PIN PC7

typedef struct {
    uint16_t buttons;
    uint8_t  hat;
    uint8_t  X, Y, Z, Rz;
    uint8_t  vendor8[12];
    uint16_t vendor16[4];
} HIDReport_t;

static uint8_t HIDReportBuffer[sizeof(HIDReport_t)];

USB_ClassInfo_HID_Device_t HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = INTERFACE_ID_HID,
				.ReportINEndpoint             =
					{
						.Address              = HID_IN_EPADDR,
						.Size                 = HID_IO_EPSIZE,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = HIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(HIDReportBuffer),
			},
	};

void uart_init(void) {
    // Calculate UBRR value for the desired baud rate
    uint16_t ubrr = (F_CPU / (16UL * MIDI_BAUD)) - 1;

    // Set baud rate
    UBRR1H = (uint8_t)(ubrr >> 8);
    UBRR1L = (uint8_t)ubrr;

    // Enable transmitter and receiver
    UCSR1B = (1 << TXEN1) | (1 << RXEN1);

    // Set frame format: 8 data bits, no parity, 1 stop bit
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}
     
/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{
	uint8_t *PORTn;
    DDRC |= (1 << LED_PIN);
	DDRD &= ~(1 << PD0);
	SetupHardware();

	GlobalInterruptEnable();

	for (;;)
	{
		USB_USBTask();

		HIDReport_t report;
		memset(&report, 0, sizeof(report));

		// Fill report based on PD0
		if (PIND & (1 << PD0)) {
			report.buttons = 0x0402;
			report.hat     = 0x08;
			report.X       = 0x7F;
			report.Y       = 0x7F;
			report.Z       = 0x7F;
			report.Rz      = 0x7F;

			uint8_t vendor8_template[12] = {0,0,0,0,0,0,0x54,0,0,0,0,0};
			memcpy(report.vendor8, vendor8_template, sizeof(vendor8_template));

			uint16_t vendor16_template[4] = {2,2,2,2};
			memcpy(report.vendor16, vendor16_template, sizeof(vendor16_template));
		} else {
			report.buttons  = 0x0000;
			report.hat      = 0x08;
			report.X        = 0x7F;
			report.Y        = 0x7F;
			report.Z        = 0x7F;
			report.Rz       = 0x7F;
			memset(report.vendor8, 0, sizeof(report.vendor8));
			for (int i = 0; i < 4; i++) report.vendor16[i] = 2;
		}

		// Service OUT endpoint first (if host sent data)
		Endpoint_SelectEndpoint(HID_OUT_EPADDR);
		if (Endpoint_IsOUTReceived()) {
			uint8_t received[HID_IO_EPSIZE];
			Endpoint_Read_Stream_LE(received, HID_IO_EPSIZE, NULL);
			Endpoint_ClearOUT();
		}

		// Service IN endpoint (host requested data)
		Endpoint_SelectEndpoint(HID_IN_EPADDR);
		if (Endpoint_IsINReady()) {
			Endpoint_Write_Stream_LE((uint8_t *)&report, sizeof(report), NULL);
			Endpoint_ClearIN(); // this signals the host that data is ready
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
	//ConfigSuccess &= Endpoint_ConfigureEndpoint(VENDOR1_IN_EPADDR,  EP_TYPE_INTERRUPT, VENDOR_IO_EPSIZE, 1);
	//ConfigSuccess &= Endpoint_ConfigureEndpoint(VENDOR1_OUT_EPADDR, EP_TYPE_INTERRUPT, VENDOR_IO_EPSIZE, 1);

	//ConfigSuccess &= Endpoint_ConfigureEndpoint(VENDOR2_IN_EPADDR,  EP_TYPE_INTERRUPT, VENDOR_IO_EPSIZE, 1);
	//ConfigSuccess &= Endpoint_ConfigureEndpoint(VENDOR2_OUT_EPADDR, EP_TYPE_INTERRUPT, VENDOR_IO_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(HID_IN_EPADDR,  EP_TYPE_INTERRUPT, HID_IO_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(HID_OUT_EPADDR, EP_TYPE_INTERRUPT, HID_IO_EPSIZE, 1);
	/* Indicate endpoint configuration success or failure */
	/* Indicate endpoint configuration success or failure */
	//LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
	/* Handle HID Class specific requests */
	switch (USB_ControlRequest.bRequest)
	{
		case HID_REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				//USB_KeyboardReport_Data_t KeyboardReportData;

				/* Create the next keyboard report for transmission to the host */
				//CreateKeyboardReport(&KeyboardReportData);

				Endpoint_ClearSETUP();

				/* Write the report data to the control endpoint */
				//Endpoint_Write_Control_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData));
				Endpoint_ClearOUT();
			}

			break;
		case HID_REQ_SetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				/* Wait until the LED report has been sent by the host */
				while (!(Endpoint_IsOUTReceived()))
				{
					if (USB_DeviceState == DEVICE_STATE_Unattached)
					  return;
				}

				/* Read in the LED report from the host */
				uint8_t LEDStatus = Endpoint_Read_8();

				Endpoint_ClearOUT();
				Endpoint_ClearStatusStage();

				/* Process the incoming LED report */
				//ProcessLEDReport(LEDStatus);
			}

			break;
		case HID_REQ_GetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				/* Write the current protocol flag to the host */
				//Endpoint_Write_8(UsingReportProtocol);

				Endpoint_ClearIN();
				Endpoint_ClearStatusStage();
			}

			break;
		case HID_REQ_SetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();

				/* Set or clear the flag depending on what the host indicates that the current Protocol should be */
				//UsingReportProtocol = (USB_ControlRequest.wValue != 0);
			}

			break;
		case HID_REQ_SetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();

				/* Get idle period in MSB, IdleCount must be multiplied by 4 to get number of milliseconds */
				//IdleCount = ((USB_ControlRequest.wValue & 0xFF00) >> 6);
			}

			break;
		case HID_REQ_GetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				/* Write the current idle duration to the host, must be divided by 4 before sent to host */
				//Endpoint_Write_8(IdleCount >> 2);

				Endpoint_ClearIN();
				Endpoint_ClearStatusStage();
			}

			break;
	}
}

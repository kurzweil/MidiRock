#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "rockband.h"

#define MIDI_BAUD 31250UL

#define LED_PIN PC7

#define BUFFER_SIZE 8  // Size of the circular buffer

typedef struct {
    uint8_t button[2];
    uint8_t hat;
    uint8_t X, Y, Z, Rz;
    uint8_t vendor8[12];
    uint16_t vendor16[4];
} HIDReport_t;

typedef struct {
    HIDReport_t buffer[BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
    bool full;
} CircularBuffer_t;

void cb_init(CircularBuffer_t *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->full = false;
}

// Advance pointer
static void advance_pointer(CircularBuffer_t *cb) {
    if (cb->full) {
        cb->tail = (cb->tail + 1) % BUFFER_SIZE;  // overwrite oldest
    }
    cb->head = (cb->head + 1) % BUFFER_SIZE;
    cb->full = (cb->head == cb->tail);
}

// Retreat pointer after reading
static void retreat_pointer(CircularBuffer_t *cb) {
    cb->full = false;
    cb->tail = (cb->tail + 1) % BUFFER_SIZE;
}

bool cb_is_empty(CircularBuffer_t *cb) {
    return (!cb->full && (cb->head == cb->tail));
}

// Add an element to the buffer (copy report)
void cb_push(CircularBuffer_t *cb, const HIDReport_t *item) {
    memcpy(&cb->buffer[cb->head], item, sizeof(HIDReport_t));
    advance_pointer(cb);
}

// Remove an element from the buffer
bool cb_pop(CircularBuffer_t *cb, HIDReport_t *item) {
    if (cb_is_empty(cb)) {
        return false; // nothing to read
    }
    memcpy(item, &cb->buffer[cb->tail], sizeof(HIDReport_t));
    retreat_pointer(cb);
    return true;
}

/*
00 00 08 7F 7F 7F 7F 00 00 00 00 00 00 00 00 00 00 00 00 02 00 02 00 02 00 02 00 Nothing
20 00 08 7F 7F 7F 7F 00 00 00 00 00 00 00 00 00 00 00 00 02 00 02 00 02 00 02 00 Kick 2 Black
10 00 08 7F 7F 7F 7F 00 00 00 00 00 00 00 00 00 00 00 00 02 00 02 00 02 00 02 00 Kick 1 Orange
04 04 08 7F 7F 7F 7F 00 00 00 00 00 FF 00 00 00 00 00 00 02 00 02 00 02 00 02 00 Red / 1
08 04 08 7F 7F 7F 7F 00 00 00 00 FF 00 00 00 00 00 00 00 02 00 02 00 02 00 02 00 Yellow / 2
01 04 08 7F 7F 7F 7F 00 00 00 00 00 00 00 FF 00 00 00 00 02 00 02 00 02 00 02 00 Blue / 3
02 04 08 7F 7F 7F 7F 00 00 00 00 00 00 FF 00 00 00 00 00 02 00 02 00 02 00 02 00 Green / 4
      XX Hat = 08 centered, clockwise 00 up, 01 up/right, 02 right, etc
   XX buttons = 08 cymbal, 04 drum hit, 01 minus, 02 plus, 10 home 
XX buttons = 01 1, 08 2, 02 A, 04 B
10 04 08 7F 7F 7F 7F 00 00 00 00 00 00 00 00 66 00 00 00 02 00 02 00 02 00 02 00
08 04 08 7F 7F 7F 7F 00 00 00 00 00 00 00 6B
02 08 08 7F 7F 7F 7F 00 00 00 00 00 00 FF 00 00 00 00 00 02 00 02 00 02 00 02 00
*/

uint8_t map_note(uint8_t x) {
    switch (x) {
        case 0x2C: return 129; 	//C2
        case 0x24: return 128; 	//C2
        case 0x26: return 2;		//D2
        case 0x30: return 3;		//B2
        case 0x2D: return 0;		//D3
        case 0x2B: return 1;		//G2
        default: return 255;
    }
}

HIDReport_t default_report = {
    .button = {0x00, 0x00},
    .hat    = 0x08,
    .X      = 0x7F,
    .Y      = 0x7F,
    .Z      = 0x7F,
    .Rz     = 0x7F,
    .vendor8 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .vendor16 = {0x0002, 0x0002, 0x0002, 0x0002}
};

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

void uart_send(uint8_t data) {
    // Wait for empty transmit buffer
    while (!(UCSR1A & (1 << UDRE1)));
    // Put data into buffer
    UDR1 = data;
}

void uart_init(void) {
    // Calculate UBRR value for the desired baud rate
    uint16_t ubrr = (F_CPU / (16UL * MIDI_BAUD)) - 1;

    // Set baud rate
    UBRR1H = (uint8_t)(ubrr >> 8);
    UBRR1L = (uint8_t)ubrr;

    // Enable transmitter and receiver
    UCSR1B = (1 << TXEN1) | (1 << RXEN1) | (1<<RXCIE1);

    // Set frame format: 8 data bits, no parity, 1 stop bit
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

#define MIDI_SIZE 3  // Max message size we expect
#define NOTE_OFF       0x80
#define NOTE_ON        0x90
#define CONTROL_CHANGE 0xB0

volatile uint8_t midi_buffer[MIDI_SIZE];
volatile uint8_t midi_index = 0;
volatile uint8_t midi_byte = 0;       // Last status byte
volatile uint8_t midi_complete = 0;   // Flag: 1 when message complete

// Helper: get expected MIDI message length from status byte
static uint8_t midi_message_length(uint8_t status) {
    if ((status & 0xF0) == 0xC0 || (status & 0xF0) == 0xD0)
        return 2;  // Program Change & Channel Pressure
    else
        return 3;  // Note on/off, Control Change, etc.
}

ISR(USART1_RX_vect) {
    uint8_t byte = UDR1;

    if (byte & 0x80) {
        // Status byte detected: start new message
        midi_index = 0;
        midi_buffer[midi_index++] = byte;
        midi_byte = byte;
        midi_complete = 0; // reset flag
    } else {
        // Data byte
        if (midi_index < MIDI_SIZE) {
            midi_buffer[midi_index++] = byte;
        }
    }

    // Check if message is complete
    if (midi_index >= midi_message_length(midi_byte)) {
        midi_complete = 1;  // flag message ready for main loop
    }
}

/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{
    CircularBuffer_t cb;
    cb_init(&cb);
    DDRC |= (1 << LED_PIN);
	uart_init();
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	/* Disable clock division */
	clock_prescale_set(clock_div_1);
	/* Hardware Initialization */
	USB_Init();

	GlobalInterruptEnable();

	HIDReport_t report = {
		.button = {0x00, 0x00},
		.hat    = 0x08,
		.X      = 0x7F,
		.Y      = 0x7F,
		.Z      = 0x7F,
		.Rz     = 0x7F,
		.vendor8 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		.vendor16 = {0x0002, 0x0002, 0x0002, 0x0002}
	};

	for (;;)
	{
		USB_USBTask();

		if (midi_complete != 0) {
			uint8_t type = midi_buffer[0] & 0xF0;   // upper nibble = message type
    		uint8_t channel = midi_buffer[0] & 0x0F;
            uint8_t note = midi_buffer[1];
            uint8_t velocity = midi_buffer[2];
			report.vendor8[9] = midi_buffer[0];
			report.vendor8[10] = midi_buffer[1];
			report.vendor8[11] = midi_buffer[2];
			
			if (type == NOTE_OFF || (type == NOTE_ON && velocity == 0)) {
				uint8_t offset = map_note(note);
				if (offset == 255) {
					cb_push(&cb, &report);
				} else if (offset == 129) {
					report.button[1] &= ~0x02;
					cb_push(&cb, &report);
				} else if (offset == 128) {
					report.button[0] &= ~0x10;
					cb_push(&cb, &report);
				} else if (offset >= 0) {
					PORTC &= ~(1 << LED_PIN);
					report.button[0] &= ~(1 << offset);
					report.vendor8[5+offset] = 0;
					cb_push(&cb, &report);
				}
			} else if (type == NOTE_ON) {
				uint8_t offset = map_note(note);
				if (offset == 255) {
					cb_push(&cb, &report);
				} else if (offset == 129) {
					report.button[1] |= 0x02;
					cb_push(&cb, &report);
				} else if (offset == 128) {
					report.button[0] |= 0x10;
					cb_push(&cb, &report);
				} else if (offset >= 0) {
					PORTC |= (1 << LED_PIN);
					report.button[0] |= (1 << offset);
					report.button[1] = 0x04;
					report.vendor8[5+offset] = velocity;
					cb_push(&cb, &report);
				}
			}
            midi_complete = 0;  // Clear flag
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
			HIDReport_t r;
			if (cb_pop(&cb, &r)) {
				Endpoint_Write_Stream_LE((uint8_t *)&r, sizeof(r), NULL);
			} else {
				Endpoint_Write_Stream_LE((uint8_t *)&default_report, sizeof(default_report), NULL);
			}
			Endpoint_ClearIN(); // this signals the host that data is ready
		}

		
	}
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

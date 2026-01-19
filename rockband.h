/*
 * Rock Band MIDI-to-USB Drum Controller for Nintendo Wii
 *
 * Copyright (c) 2024 Rock Band MIDI-to-USB Drum Controller Contributors
 *
 * This file is part of the Rock Band MIDI-to-USB project.
 * Licensed under the MIT License - see LICENSE file for details.
 *
 * This project uses the LUFA library, Copyright (C) Dean Camera, 2021.
 * LUFA is used under its permissive license - see vendor/lufa for details.
 */

#ifndef _BULK_VENDOR_H_
#define _BULK_VENDOR_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>

		#include "Descriptors.h"

		#include <LUFA/Drivers/USB/USB.h>
		#include <LUFA/Platform/Platform.h>

	/* Function Prototypes: */
		void SetupHardware(void);

		void EVENT_USB_Device_Connect(void);
		void EVENT_USB_Device_Disconnect(void);
		void EVENT_USB_Device_ConfigurationChanged(void);
		void EVENT_USB_Device_ControlRequest(void);

#endif
/*
 * Rock Band MIDI-to-USB Drum Controller for Nintendo Wii
 *
 * USB Descriptors - Emulates Harmonix Rock Band Drum Controller
 *
 * Copyright (c) 2024 Rock Band MIDI-to-USB Drum Controller Contributors
 *
 * This file is part of the Rock Band MIDI-to-USB project.
 * Licensed under the MIT License - see LICENSE file for details.
 *
 * This project uses the LUFA library, Copyright (C) Dean Camera, 2021.
 * LUFA is used under its permissive license - see vendor/lufa for details.
 */

#include "Descriptors.h"

const USB_Descriptor_HIDReport_Datatype_t PROGMEM HIDReport[] =
{
    HID_RI_USAGE_PAGE(8, 0x01),            /* Generic Desktop */
    HID_RI_USAGE(8, 0x05),                 /* Game Pad */
    HID_RI_COLLECTION(8, 0x01),            /* Application */
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(8, 0x01),
        HID_RI_PHYSICAL_MINIMUM(8, 0x00),
        HID_RI_PHYSICAL_MAXIMUM(8, 0x01),
        HID_RI_REPORT_SIZE(8, 0x01),
        HID_RI_REPORT_COUNT(8, 0x0D),
        HID_RI_USAGE_PAGE(8, 0x09),        /* Button */
        HID_RI_USAGE_MINIMUM(8, 0x01),
        HID_RI_USAGE_MAXIMUM(8, 0x0D),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        HID_RI_REPORT_COUNT(8, 0x03),
        HID_RI_INPUT(8, HID_IOF_CONSTANT),
        HID_RI_USAGE_PAGE(8, 0x01),        /* Generic Desktop */
        HID_RI_LOGICAL_MAXIMUM(8, 0x07),
        HID_RI_PHYSICAL_MAXIMUM(16, 0x013B),
        HID_RI_REPORT_SIZE(8, 0x04),
        HID_RI_REPORT_COUNT(8, 0x01),
        HID_RI_UNIT(8, 0x14),              /* Rotation (Eng. Pos) */
        HID_RI_USAGE(8, 0x39),             /* Hat switch */
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NULLSTATE),
        HID_RI_UNIT(8, 0x00),
        HID_RI_REPORT_COUNT(8, 0x01),
        HID_RI_INPUT(8, HID_IOF_CONSTANT),
        HID_RI_LOGICAL_MAXIMUM(16, 0x00FF),
        HID_RI_PHYSICAL_MAXIMUM(16, 0x00FF),
        HID_RI_USAGE(8, 0x30),             /* X */
        HID_RI_USAGE(8, 0x31),             /* Y */
        HID_RI_USAGE(8, 0x32),             /* Z */
        HID_RI_USAGE(8, 0x35),             /* Rz */
        HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_REPORT_COUNT(8, 0x04),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        HID_RI_USAGE_PAGE(16, 0xFF00),     /* Vendor-defined */
        HID_RI_USAGE(8, 0x20),
        HID_RI_USAGE(8, 0x21),
        HID_RI_USAGE(8, 0x22),
        HID_RI_USAGE(8, 0x23),
        HID_RI_USAGE(8, 0x24),
        HID_RI_USAGE(8, 0x25),
        HID_RI_USAGE(8, 0x26),
        HID_RI_USAGE(8, 0x27),
        HID_RI_USAGE(8, 0x28),
        HID_RI_USAGE(8, 0x29),
        HID_RI_USAGE(8, 0x2A),
        HID_RI_USAGE(8, 0x2B),
        HID_RI_REPORT_COUNT(8, 0x0C),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        HID_RI_USAGE(16, 0x2621),          /* Vendor-defined */
        HID_RI_REPORT_COUNT(8, 0x08),
        HID_RI_FEATURE(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        HID_RI_USAGE(16, 0x2621),          /* Vendor-defined */
        HID_RI_OUTPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        HID_RI_LOGICAL_MAXIMUM(16, 0x03FF),
        HID_RI_PHYSICAL_MAXIMUM(16, 0x03FF),
        HID_RI_USAGE(8, 0x2C),
        HID_RI_USAGE(8, 0x2D),
        HID_RI_USAGE(8, 0x2E),
        HID_RI_USAGE(8, 0x2F),
        HID_RI_REPORT_SIZE(8, 0x10),
        HID_RI_REPORT_COUNT(8, 0x04),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
    HID_RI_END_COLLECTION(0),
};

/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

	.USBSpecification       = VERSION_BCD(1,1,0),
	.Class                  = USB_CSCP_NoDeviceClass,
	.SubClass               = USB_CSCP_NoDeviceSubclass,
	.Protocol               = USB_CSCP_NoDeviceProtocol,

	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID               = 0x1BAD,
	.ProductID              = 0x3110,
	.ReleaseNumber          = VERSION_BCD(2,0,0),

	.ManufacturerStrIndex   = STRING_ID_Manufacturer,
	.ProductStrIndex        = STRING_ID_Product,
	.SerialNumStrIndex      = NO_DESCRIPTOR,

	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
	.Config =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

			.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
			.TotalInterfaces        = 1,

			.ConfigurationNumber    = 1,
			.ConfigurationStrIndex  = NO_DESCRIPTOR,

			.ConfigAttributes       = USB_CONFIG_ATTR_RESERVED,

			.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
		},

	.HID_Interface =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber        = INTERFACE_ID_HID,
			.AlternateSetting       = 0x00,

			.TotalEndpoints         = 2,

			.Class                  = HID_CSCP_HIDClass,
			.SubClass               = 0x00,
			.Protocol               = 0x00,

			.InterfaceStrIndex      = NO_DESCRIPTOR
		},

	.HID_HID =
		{
			.Header                 = {.Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID},

			.HIDSpec                = VERSION_BCD(1,1,1),
			.CountryCode            = 0x00,
			.TotalReportDescriptors = 1,
			.HIDReportType          = HID_DTYPE_Report,
			.HIDReportLength        = sizeof(HIDReport) 
		},

	.HID_ReportINEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = HID_IN_EPADDR,
			.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = HID_IO_EPSIZE,
			.PollingIntervalMS      = 0x0A
		},

	.HID_ReportOUTEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = HID_OUT_EPADDR,
			.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = HID_IO_EPSIZE,
			.PollingIntervalMS      = 0x0A
		},
};

/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(L"Licenced by Nintendo of America ");

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(L"Harmonix Drum Controller for Nintendo Wii");

/** This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	const void* Address = NULL;
	uint16_t    Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = &DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = &ConfigurationDescriptor;
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (DescriptorNumber)
			{
				case STRING_ID_Language:
					Address = &LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case STRING_ID_Manufacturer:
					Address = &ManufacturerString;
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case STRING_ID_Product:
					Address = &ProductString;
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
			}
			break;
		case HID_DTYPE_HID:
			Address = &ConfigurationDescriptor.HID_HID;
			Size    = sizeof(USB_HID_Descriptor_HID_t);
			break;
		case HID_DTYPE_Report:
			Address = &HIDReport;
			Size    = sizeof(HIDReport);
			break;
	}

	*DescriptorAddress = Address;
	return Size;
}


#ifndef usbbase_h
#define usbbase_h

#include "types.h"
#include "library.h"

#include "usb.h"

#define SysBase UsbBase->USB_SysBase

#define VERSION  0
#define REVISION 1

#define USB_BUFSIZ	512

typedef struct UsbBase
{
	struct Library	LibNode;
	APTR			USB_SysBase;
	//USB.c
	struct usb_device	USB_Dev[USB_MAX_DEVICE];
	UINT32				Dev_Index;
	UINT32				Running;
	UINT32				Asynch_Allowed;
	char				USB_Started; /* flag for the started/stopped USB status */

	//USB_HUB.c
	struct usb_hub_device USB_HubDev[USB_MAX_HUB];
	int USB_HubIndex;

	// OTG Special Vars
	//UINT8 *allocated_buffer;
	//UINT8 *aligned_buffer;
	//UINT8 *status_buffer;

	//dwc_otg_core_if_t g_core_if;

	#define MAX_DEVICE 16
	#define MAX_ENDPOINT 8
	int bulk_data_toggle[MAX_DEVICE][MAX_ENDPOINT];
	int control_data_toggle[MAX_DEVICE][MAX_ENDPOINT];

} UsbBase;

#endif

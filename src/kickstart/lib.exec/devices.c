#include "types.h"
#include "list.h"
#include "ports.h"
#include "sysbase.h"
#include "device.h"
#include "io.h"

#include "exec_funcs.h"

void lib_AddDevice(SysBase *SysBase, struct Device *device)
{
	device->dd_Library.lib_Node.ln_Type=NT_DEVICE;
	device->dd_Library.lib_Flags|=LIBF_CHANGED;

	// TODO: Pls Fix me !!
	//SumLibrary(&device->dd_Library);
	//ObtainSemaphore(&SysBase->LockDev);
  	Forbid();
  	Enqueue(&SysBase->DevList,&device->dd_Library.lib_Node);
  	Permit();
	//ReleaseSemaphore(&SysBase->LockDev);
}

UINT32 lib_RemDevice(SysBase *SysBase, struct Device *device)
{
	// Bogos check !
	if (device==NULL) return 0;

	UINT32 ret;
	//ObtainSemaphore(&SysBase->LockDev);

	Forbid();
  	Remove(&device->dd_Library.lib_Node);
	// Call the Expunge Function of the Device
	ret = (((UINT32(*)(struct Device *)) _GETVECADDR(device,3))(device));

	//ReleaseSemaphore(&SysBase->LockDev);
	Permit();
	return ret;
}

void lib_CloseDevice(SysBase *SysBase, struct IORequest *iORequest)
{
	if(NULL != iORequest->io_Device)
	{
		Forbid();
		// Call CloseDeviceFunction on Device
		(((void(*)(struct Device *, struct IORequest *))_GETVECADDR(iORequest->io_Device,2))(iORequest->io_Device, iORequest));
		// Trash Device in iORequest Structure
		iORequest->io_Device=(struct Device *)-1;
		Permit();
	}
}

APTR(*dodevice)(struct IORequest *,UINT32, UINT32, struct Device *);

INT32 lib_OpenDevice(struct SysBase *SysBase, STRPTR devName, UINT32 unitNum, struct IORequest *iORequest, UINT32 flags)
{
	struct Device *device;
	INT32 ret=IOERR_OPENFAIL;

	Forbid();
	device=(struct Device *)FindName(&SysBase->DevList,devName);
	Permit();
	
	if (NULL != device)
	{
		//DPrintF("OpenDevice: %x\n",device); 
		//DPrintF("OpenDevice: %s\n",device->dd_Library.lib_Node.ln_Name); 
	    /* Init iorequest */
    	iORequest->io_Error  = 0;
    	iORequest->io_Device = device;
    	iORequest->io_Unit   = NULL;

    	Forbid();
    	if ((((APTR(*)(struct Device *, struct IORequest *,UINT32, UINT32)) _GETVECADDR(device,1))(device, iORequest, unitNum, flags)) == NULL)
		{
			Permit();
			return ret;
		}
		Permit();
    	ret = iORequest->io_Error;
    	if (ret < 0) iORequest->io_Device = NULL;
	}
	return ret;
}




#include "types.h"
#include "sdhci_intern.h"
#include "exec_funcs.h"


APTR sdhci_OpenDev(struct SDHCIBase *SDHCIBase, struct IORequest *ioreq, UINT32 unitnum, UINT32 flags)
{
    // we dont allow unitnumbers bigger than 0
    if (unitnum>0) 
    {
		ioreq->io_Error = IOERR_OPENFAIL;
		return SDHCIBase;
	}
	ioreq->io_Error = 0;
    ioreq->io_Unit = (struct Unit*)&SDHCIBase->Units;
    ioreq->io_Message.mn_Node.ln_Type = NT_REPLYMSG;
	ioreq->io_Device = (struct Device *)SDHCIBase;
	
	SDHCIBase->Device.dd_Library.lib_OpenCnt++;
	SDHCIBase->Device.dd_Library.lib_Flags &= ~LIBF_DELEXP;
	return SDHCIBase;
}


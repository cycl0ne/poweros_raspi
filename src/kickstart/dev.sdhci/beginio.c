#include "types.h"
#include "sdhci_intern.h"
#include "exec_funcs.h"

void sdhci_BeginIO(struct SDHCIBase *SDHCIBase, struct IOExtTD *ioreq)
{
    if (ioreq->iotd_Req.io_Flags & IOF_QUICK)
    {
		DPrintF("IOF_QUICK\n");
		switch(ioreq->iotd_Req.io_Command)
		{
			case TD_CHANGESTATE:
				//tdu = (struct TDU *)iotd->iotd_Req.io_Unit;
				//if (!((tdu->pub.tdu_PubFlags & TDPF_NOCLICK)) || (tdu->tdu_DiskIn == TDU_DISK))
				break;
			case CMD_READ:
			case CMD_UPDATE:
			case CMD_WRITE:
			case TD_FORMAT:
			case TD_MOTOR:
				PutMsg(SDHCIBase->UnitPort, &ioreq->iotd_Req.io_Message);
				ioreq->iotd_Req.io_Flags &= ~IOF_QUICK;
				return;
		}
		DPrintF("Yes QUICK!\n");
		sdhci_PerformIO(SDHCIBase,ioreq);
		return;
    }
    else
    {
		DPrintF("NO IOF_QUICK\n");

		PutMsg(SDHCIBase->UnitPort, &ioreq->iotd_Req.io_Message);
		ioreq->iotd_Req.io_Flags &= ~IOF_QUICK;
		return;
    }
}

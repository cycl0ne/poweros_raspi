#include "types.h"
#include "sdhci_intern.h"
#include "exec_funcs.h"

#define SetError(a) ioreq->iotd_Req.io_Error = a;

BOOL sdhci_PerformIO(struct SDHCIBase *SDHCIBase, struct IOExtTD *ioreq)
{
	BOOL reply = TRUE;
	struct SDHCIUnit *unit;
	
	unit = (struct SDHCIUnit *)ioreq->iotd_Req.io_Unit;

	switch(ioreq->iotd_Req.io_Command)
	{
		case CMD_CLEAR:
			if (ioreq->iotd_Count > unit->Counter) 
			{
				SetError(TDERR_DiskChanged);
				break;
			}
			unit->Flags = 0;
			SetError(0);
		case CMD_READ:
			if (ioreq->iotd_Count > unit->Counter) 
			{
				SetError(TDERR_DiskChanged);
				break;
			}
			DPrintF("[SDHCI] Read ioreq: %x, io_Data: %x, io_Length: %x, io_Offset: %x\n", 
				ioreq, ioreq->iotd_Req.io_Data, ioreq->iotd_Req.io_Length, ioreq->iotd_Req.io_Offset);
			unit->Unit.unit_flags |= UNITF_BUSY;
			//ioreq->iotd_Req.io_Actual = unit->MMC->block_dev.block_read(0, ioreq->iotd_Req.io_Offset, ioreq->iotd_Req.io_Length, ioreq->iotd_Req.io_Data); //Fixme, at the moment only one unit (0).
			unit->Unit.unit_flags &= ~UNITF_BUSY;
			DPrintF("[SDHCI] Result: %x\n", ioreq->iotd_Req.io_Actual);
			break;
		case CMD_UPDATE:
#if 0
			if (ioreq->iotd_Count > unit->Counter) 
			{
				SetError(TDERR_DiskChanged);
				break;
			}
			//FIXME: Implement
#endif
			break;
		case CMD_WRITE:
			if (ioreq->iotd_Count > unit->Counter) 
			{
				SetError(TDERR_DiskChanged);
				break;
			}
			DPrintF("[SDHCI] Write ioreq: %x, io_Data: %x, io_Length: %x, io_Offset: %x\n", 
				ioreq, ioreq->iotd_Req.io_Data, ioreq->iotd_Req.io_Length, ioreq->iotd_Req.io_Offset);
			unit->Unit.unit_flags |= UNITF_BUSY;
			ioreq->iotd_Req.io_Actual = unit->MMC->block_dev.block_write(0, ioreq->iotd_Req.io_Offset, ioreq->iotd_Req.io_Length, ioreq->iotd_Req.io_Data); //Fixme, at the moment only one unit (0).
			unit->Unit.unit_flags &= ~UNITF_BUSY;
			DPrintF("[SDHCI] Result: %x\n", ioreq->iotd_Req.io_Actual);
			break;
		case TD_CHANGENUM:
			ioreq->iotd_Req.io_Actual = unit->Counter;
            SetError(0);
			break;
		case TD_ADDCHANGEINT:
			AddTail(&unit->ChangeListener,(struct Node *)ioreq);
			reply = FALSE;
			break;
		case TD_REMCHANGEINT:
			Remove((struct Node *)ioreq);
			SetError(0);
			break;
		case TD_CHANGESTATE:
			DPrintF("[SDHCI] FIXME: Todo \n");
			SetError(0);
			break;
		case TD_FORMAT:
			if (ioreq->iotd_Count > unit->Counter) 
			{
				SetError(TDERR_DiskChanged);
				break;
			}
			DPrintF("[SDHCI] Format ioreq: %x, io_Data: %x, io_Length: %x, io_Offset: %x\n", 
					ioreq, ioreq->iotd_Req.io_Data, ioreq->iotd_Req.io_Length, ioreq->iotd_Req.io_Offset);

			unit->Unit.unit_flags |= UNITF_BUSY;
			//ioreq->iotd_Req.io_Actual = unit->MMC->block_dev.block_write(0, ioreq->iotd_Req.io_Offset, ioreq->iotd_Req.io_Length, ioreq->iotd_Req.io_Data); //Fixme, at the moment only one unit (0).
			unit->Unit.unit_flags &= ~UNITF_BUSY;
			DPrintF("[SDHCI] Result: %x\n", ioreq->iotd_Req.io_Actual);
			SetError(0);
			break;
		case TD_PROTSTATUS:
			ioreq->iotd_Req.io_Actual = unit->ProtStatus;
            SetError(0);
			break;
		case TD_GETGEOMETRY:  // FIXME: Enhance this
            DPrintF("[SDHCI] TD_GETGEOMETRY: ioreq=%x\n", ioreq);
			struct DriveGeometry *geo = (struct DriveGeometry *)ioreq->iotd_Req.io_Data;
			geo->dg_SectorSize = unit->MMC->block_dev.blksz; //512;
			geo->dg_TotalSectors = unit->MMC->block_dev.lba; //DP_STOTAL;
			geo->dg_Cylinders = 0;//DP_TRACKS;
			geo->dg_CylSectors = 0; //DP_SECTORS*2;
			geo->dg_Heads = 0;
			geo->dg_TrackSectors = 0; //DP_SECTORS;
			geo->dg_BufMemType = MEMF_FAST;
			geo->dg_DeviceType = DG_DIRECT_ACCESS;
			geo->dg_Flags = DGF_REMOVABLE;
            SetError(0);
			break;
		case TD_GETDRIVETYPE:
			ioreq->iotd_Req.io_Actual = unit->MMC->block_dev.type;
			SetError(0);
			break;
		case TD_GETNUMTRACKS:
			ioreq->iotd_Req.io_Actual = unit->MMC->block_dev.lba << 9;
			SetError(0);
			break;
		case TD_SEEK:
			if (ioreq->iotd_Count > unit->Counter) 
			{
				SetError(TDERR_DiskChanged);
				break;
			}
			break;
		default:
			SetError(IOERR_NOCMD);
			break;
	}	
	return reply;
}

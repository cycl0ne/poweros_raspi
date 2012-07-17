#include "types.h"
#include "sdhci_intern.h"
#include "exec_funcs.h"

INT32 board_mmc_init();
#include "mmc.h"

static void print_mmcinfo(struct mmc *mmc)
{
	printf("Device: %s\n", mmc->name);
	printf("Manufacturer ID: %x\n", mmc->cid[0] >> 24);
	printf("OEM: %x\n", (mmc->cid[0] >> 8) & 0xffff);
	printf("Name: %c%c%c%c%c \n", mmc->cid[0] & 0xff,
			(mmc->cid[1] >> 24), (mmc->cid[1] >> 16) & 0xff,
			(mmc->cid[1] >> 8) & 0xff, mmc->cid[1] & 0xff);

	printf("Tran Speed: %d\n", mmc->tran_speed);
	printf("Rd Block Len: %d\n", mmc->read_bl_len);

	printf("%s version %d.%d\n", IS_SD(mmc) ? "SD" : "MMC",
			(mmc->version >> 4) & 0xf, mmc->version & 0xf);

	printf("High Capacity: %s\n", mmc->high_capacity ? "Yes" : "No");
	printf("Capacity: ");
	UINT64 tmp = mmc->capacity;
	tmp = (tmp >>20)&0xffffffff;
	UINT32 tmp2 = tmp & 0xffffffff;
	printf("%d GiB\n", tmp2>>10);

	printf("Bus Width: %d-bit\n", mmc->bus_width);
}

static int curr_device = -1;

int do_mmcinfo () //cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct mmc *mmc;

	if (curr_device < 0) {
		if (get_mmc_num() > 0)
			curr_device = 0;
		else {
			printf("No MMC device available\n");
			return 1;
		}
	}
	mmc = find_mmc_device(curr_device);
	if (mmc) {
		mmc_init(mmc);

		printf("print Infos:\n");

		print_mmcinfo(mmc);
		return 0;
	} else {
		printf("no mmc device at slot %x\n", curr_device);
		return 1;
	}
}

void sdhci_int_createTimeDC(struct SDHCIBase *SDHCIBase)
{
	SDHCIBase->Timer0_Unit->tr_node.io_Command	= TR_ADDREQUEST;
	SDHCIBase->Timer0_Unit->tr_time.tv_secs		= 2;
	SDHCIBase->Timer0_Unit->tr_time.tv_micro	= 0;
	SendIO((struct IORequest *)SDHCIBase->Timer0_Unit);
}

void dev_SDHCITask(struct SDHCIBase *SDHCIBase) 
{
	DPrintF("[SDHCITask] started (%x)\n", SDHCIBase);

	UINT32	signal, signalTime, signalTask;
	UINT32	waitsignal;
    struct IOExtTD	*iosdhci = NULL;

	SDHCIBase->UnitPort = CreateMsgPort();
	SDHCIBase->TimerPort = CreateMsgPort();
	SDHCIBase->Timer0_Unit = CreateIORequest(SDHCIBase->TimerPort, sizeof(struct TimeRequest));
	//SDHCIBase->Timer1_Unit = CreateIORequest(SDHCIBase->TimerPort, sizeof(struct TimeRequest));
	
	if (0 != OpenDevice("timer.device", UNIT_VBLANK, (struct IORequest *)SDHCIBase->Timer0_Unit, 0)) 
	{
		DPrintF("[SDHCITask] goes down, no timer.device to open\n");
		DeleteIORequest((struct IORequest *)SDHCIBase->Timer0_Unit);
		DeleteMsgPort(SDHCIBase->UnitPort);
		DeleteMsgPort(SDHCIBase->TimerPort);
		return;
	}

	NewListType(&SDHCIBase->Units.ChangeListener, NT_INTERRUPT);
	
	//SDHCIBase->Units.MMC = find_mmc_device(0);
	//if (SDHCIBase->Units.MMC) mmc_init(SDHCIBase->Units.MMC);
	
	//SDHCIBase->Timer0_Dev = SDHCIBase->Timer0_Unit->tr_node.io_Device;
	//SDHCIBase->Timer1_Dev = SDHCIBase->Timer1_Unit->tr_node.io_Device;

	signalTask = (1 << SDHCIBase->UnitPort->mp_SigBit);
	signalTime = (1 << SDHCIBase->TimerPort->mp_SigBit);
	waitsignal = signalTask | signalTime;	

	while (!(SDHCIBase->Flags & SDHCIF_Shutdown))
	{
//		DPrintF("[SDHCITask] Create TimeDC\n");	
		sdhci_int_createTimeDC(SDHCIBase);
		signal = 0;
//		DPrintF("[SDHCITask] Wait Signal %x\n", waitsignal);
		signal = Wait(waitsignal);
//		DPrintF("[SDHCITask] Signal %x\n", signal);
		if (signal & signalTask)
		{
			DPrintF("[SDHCITask] There is work for us todo\n");		
			while((iosdhci = (struct IOExtTD *)GetMsg(SDHCIBase->UnitPort)) != NULL)
			{
				//DPrintF("[SDHCITask] IORequest\n");
				if (sdhci_PerformIO( SDHCIBase, iosdhci))
				{
					//DPrintF("[SDHCITask] IORequest replied\n");
					ReplyMsg((struct Message *)iosdhci);
				}
				//DPrintF("[SDHCITask] finished\n");
			}
			DPrintF("[SDHCITask] finished List of IOs\n");
		} else if (signal & signalTime)
		{
			// Accept Message
			//WaitIO((struct IORequest *)SDHCIBase->Timer0_Unit);
			// DiscChange to detect.
			// SDHCIBase->
			/*
			tdu->pub.tdu_Counter++;
			Forbid();
			ForeachNode(&tdu->tdu_Listeners,iotd)
			{
				Cause((struct Interrupt *)((struct IOExtTD *)iotd->iotd_Req.io_Data));
			}
			Permit();
			*/
			DPrintF("[SDHCITask] Checking for Disc Change\n");
		} else {
			DPrintF("[SDHCITask] Woke from a strange signal %x", signal);
		}
	}
	
	//do_mmcinfo();
	
	for(;;);
}

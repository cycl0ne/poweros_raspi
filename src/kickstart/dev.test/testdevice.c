#include "types.h"
#include "testdev_intern.h"
#include "exec_funcs.h"

APTR tdev_OpenDev(struct TestDevBase *TestDevBase, struct IORequest *ioreq, UINT32 unitnum, UINT32 flags);
APTR tdev_CloseDev(struct TestDevBase *TestDevBase, struct IORequest *ioreq);
APTR tdev_ExpungeDev(struct TestDevBase *TestDevBase);
APTR tdev_ExtFuncDev(struct TestDevBase *TestDevBase);
void tdev_BeginIO(TestDevBase *TestDevBase, struct IORequest *ioreq);
void tdev_AbortIO(TestDevBase *tdev_, struct IORequest *ioreq);
BOOL tdev_PerformIO(struct TestDevBase *TestDevBase, struct IORequest *ior);

APTR tdev_OpenDev(struct TestDevBase *TestDevBase, struct IORequest *ioreq, UINT32 unitnum, UINT32 flags)
{
    // we only have TDEV_UNIT_MAX Units
	if (unitnum < TDEV_UNIT_MAX)
	{
		ioreq->io_Error = 0;
		ioreq->io_Message.mn_Node.ln_Type = NT_REPLYMSG;	
		TestDevBase->Device.dd_Library.lib_OpenCnt++;
		TestDevBase->Device.dd_Library.lib_Flags &= ~LIBF_DELEXP;
	} else
	{
		ioreq->io_Error = IOERR_OPENFAIL;
	}
	return TestDevBase;		
}

APTR tdev_CloseDev(struct TestDevBase *TestDevBase, struct IORequest *ioreq)
{
	TestDevBase->Device.dd_Library.lib_OpenCnt--;
	return NULL;
}

APTR tdev_ExpungeDev(struct TestDevBase *TestDevBase)
{
	return NULL;
}

APTR tdev_ExtFuncDev(struct TestDevBase *TestDevBase)
{
	return NULL;
}

void tdev_BeginIO(TestDevBase *TestDevBase, struct IORequest *ioreq)
{
	ioreq->io_Error = 0;

	// First we try to do everything "QUICK" = Synchronous
	if (ioreq->io_Flags & IOF_QUICK)
	{
		switch(ioreq->io_Command)
		{
			// These Commands cant be done QUICK (async)
			case CMD_READ:
			case CMD_WRITE:
				PutMsg(TestDevBase->TaskPort, &ioreq->io_Message);
				ioreq->io_Flags &= ~IOF_QUICK;
				return;
			default:
				break;
		}
		// All the rest can be done Sync
		tdev_PerformIO(TestDevBase, ioreq);
		return;
	} else
	{
		PutMsg(TestDevBase->TaskPort, &ioreq->io_Message);
		ioreq->io_Flags &= ~IOF_QUICK;
		return;		
	}
}

void tdev_AbortIO(TestDevBase *tdev_, struct IORequest *ioreq)
{
	ioreq->io_Error = 0;
	return;
}


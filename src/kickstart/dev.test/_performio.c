#include "types.h"
#include "testdev_intern.h"
#include "exec_funcs.h"

#define SetError(a) ior->io_Error = a;

BOOL tdev_PerformIO(struct TestDevBase *TestDevBase, struct IORequest *ior)
{
	// if TRUE, reply to our sender
	BOOL reply = TRUE;
	
	switch(ior->io_Command)
	{
		case CMD_READ:
			DPrintF("READ (NON Quick Command)\n");
            SetError(0);
			break;
		case CMD_UPDATE:
			DPrintF("Update (Quick Command)\n");
            SetError(0);
			break;
		default:
			SetError(IOERR_NOCMD);
			break;
	}
	return reply;
}

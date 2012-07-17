#include "types.h"
#include "sdhci_intern.h"
#include "exec_funcs.h"


APTR sdhci_CloseDev(struct SDHCIBase *SDHCIBase, struct IORequest *ioreq)
{
	SDHCIBase->Device.dd_Library.lib_OpenCnt--;
	return NULL;
}


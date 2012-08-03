#include "types.h"
#include "resident.h"
#include "sysbase.h"

#include "exec_funcs.h"

/*
* This Task initialises everything after ColdStart, where Multitasking is activated.
* this is why we use a Task here and leave Kernel Init.
*/

void lib_ExecTask(struct SysBase *SysBase) 
{
	InitResidentCode(RTF_COLDSTART);
	DPrintF("[EXECTASK] Finished, we are leaving... bye bye... till next reboot\n");
}


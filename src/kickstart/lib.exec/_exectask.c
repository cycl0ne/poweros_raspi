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
	struct ResidentNode *res;

	DPrintF("[EXECTASK] RTF_COLDSTART\n");
    ForeachNode(&SysBase->ResidentList, res)
    {
        if (res->rn_Resident->rt_Flags & RTF_COLDSTART) 
        {
			DPrintF("(COLDSTART)InitResident %s (%x)\n", res->rn_Resident->rt_Name,res->rn_Resident);    
            if (InitResident(res->rn_Resident, NULL)== NULL) 
            {
				DPrintF("[INIT] ROMTAG Scanner Failed (ColdStart)\n");
				DPrintF("on Resident [%x] name: %s\n", res->rn_Resident, res->rn_Resident->rt_Name);
				DPrintF("System is not halted, but no new modules loaded\n");
				for(;;);
            }
        }
    }	
	
	DPrintF("[EXECTASK] RTF_AFTERDOS\n");
    ForeachNode(&SysBase->ResidentList, res)
    {
        if (res->rn_Resident->rt_Flags & RTF_AFTERDOS) 
        {
            DPrintF("(AFTERDOS)InitResident %s (%x)\n", res->rn_Resident->rt_Name,res->rn_Resident);    
            if (InitResident(res->rn_Resident, NULL)== NULL) 
            {
				DPrintF("[INIT] ROMTAG Scanner Failed (RTF_AFTERDOS)\n");
				DPrintF("on Resident [%x] name: %s\n", res->rn_Resident, res->rn_Resident->rt_Name);
				DPrintF("System is not halted, but no new modules loaded\n");
			}
        }
    }
	DPrintF("[EXECTASK] Finished, we are leaving... bye bye... till next reboot\n");
}


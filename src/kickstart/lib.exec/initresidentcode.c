#include "types.h"
#include "resident.h"
#include "sysbase.h"

#include "exec_funcs.h"

void lib_InitResidentCode(struct SysBase * SysBase, UINT32 startClass)
{
	struct ResidentNode *res;
	switch (startClass)
	{
		case RTF_AFTERDOS:
			DPrintF("[DOSTASK] RTF_AFTERDOS\n");
			break;
		case RTF_COLDSTART:	
			DPrintF("[EXECTASK] RTF_COLDSTART\n");
			break;
		case RTF_SINGLETASK:
			DPrintF("[INIT] RTF_SINGLETASK\n");
			break;
	}

    ForeachNode(&SysBase->ResidentList, res)
    {
        if (res->rn_Resident->rt_Flags & startClass) 
        {
			DPrintF("InitResident %s (%x)\n", res->rn_Resident->rt_Name,res->rn_Resident);    
            if (InitResident(res->rn_Resident, NULL)== NULL) 
            {
				DPrintF("[INIT] InitResidentCode Scanner Failed \n");
				DPrintF("on Resident [%x] name: %s\n", res->rn_Resident, res->rn_Resident->rt_Name);
				DPrintF("System is not halted, but no new modules loaded\n");
				for(;;);
            }
        }
    }	

}
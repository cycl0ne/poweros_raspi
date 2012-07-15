#include "types.h"
#include "list.h"
#include "ports.h"
#include "sysbase.h"

#include "alerts.h"
#include "exec_funcs.h"

void lib_Alert(SysBase *SysBase, UINT32 alertNum)
{
    struct Task *task = SysBase->thisTask;
	
    if (alertNum & AT_DeadEnd)
    {
		//DEADEND RESET NORMALY ;)
		for(;;);
	}
}
#include "types.h"
#include "list.h"
#include "ports.h"
#include "sysbase.h"

APTR lib_ExpungeLib(struct SysBase *SysBase)
{
	return NULL;
}

APTR lib_ExtFuncLib(void)
{
	return NULL;
}

APTR lib_OpenLib(struct SysBase *SysBase)
{
	SysBase->LibNode.lib_OpenCnt++;
	//SysBase->LibNode.lib_Flags &= ~LIBF_DELEXP;
	return(SysBase);
}

APTR lib_CloseLib(struct SysBase *SysBase)
{
	SysBase->LibNode.lib_OpenCnt--;
	return NULL;
}

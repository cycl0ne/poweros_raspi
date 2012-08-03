#include "utilitybase.h"

APTR util_OpenLib(struct UtilityBase *UtilityBase)
{
	UtilityBase->ut_lib.lib_OpenCnt++;
	//SysBase->LibNode.util_Flags &= ~LIBF_DELEXP;
	return(UtilityBase);
}

APTR util_CloseLib(struct UtilityBase *UtilityBase)
{
	UtilityBase->ut_lib.lib_OpenCnt--;
	return NULL;
}

APTR util_ExpungeLib(struct UtilityBase *UtilityBase)
{
	return NULL;
}

APTR util_ExtFuncLib(void)
{
	return NULL;
}

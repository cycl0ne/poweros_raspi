#include "expansionbase.h"

APTR exp_OpenLib(struct ExpansionBase *ExpansionBase)
{
	ExpansionBase->Library.lib_OpenCnt++;
	//SysBase->LibNode.exp_Flags &= ~LIBF_DELEXP;
	return(ExpansionBase);
}

APTR exp_CloseLib(struct ExpansionBase *ExpansionBase)
{
	ExpansionBase->Library.lib_OpenCnt--;
	return NULL;
}

APTR exp_ExpungeLib(struct ExpansionBase *ExpansionBase)
{
	return NULL;
}

APTR exp_ExtFuncLib(void)
{
	return NULL;
}

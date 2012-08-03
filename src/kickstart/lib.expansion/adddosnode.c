/*
*   $Id: adddosnode.c
*
*   NAME
*
*   SYNOPSIS
*
*   FUNCTION
*
*   INPUTS
*
*	RESULTS
*
*/

#include "expansionbase.h"
#include "exec_funcs.h"

#define SysBase ExpansionBase->SysBase

BOOL exp_AddDosNode(struct ExpansionBase *ExpansionBase, INT32 bootPri, UINT32 flags, struct DeviceNode *deviceNode)
{
	struct DeviceNode *dn;
	
	DPrintF("[Expansion.library] AddBootNode: Adding %s from Task %s\n", deviceNode->dn_Node.ln_Name, FindTask(NULL)->Node.ln_Name);

	if (NULL == ExpansionBase->DosBase)
	{
		ExpansionBase->DosBase = OpenLibrary("dos.library", 0);
		if (NULL == ExpansionBase->DosBase)
		{
			dn = (struct DeviceNode *)FindName(&ExpansionBase->MountList, deviceNode->dn_Node.ln_Name);
			if (NULL != dn)
			{
				DPrintF("[Expansion.library] AddBootNode: duplicate Device\n");
				return FALSE;
			}
			deviceNode->dn_Node.ln_Pri = bootPri; // BootPrio
			ObtainSemaphore(&ExpansionBase->MountListLock);
			Enqueue(&ExpansionBase->MountList, (struct Node *)deviceNode);
			ReleaseSemaphore(&ExpansionBase->MountListLock);
			return TRUE;
		}
	}
	// DOS is open
	#if 0
	// We have no dos.library at the moment :-P
	if (AddDosEntry((struct DosList *)deviceNode))
	{
		if (flags)	DeviceProc(deviceNode->dn_Node.ln_Name);
		return TRUE
	}
	#endif
	return FALSE;
}

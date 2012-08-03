/*
*   $Id: makedosnode.c
*
*   NAME
*	MakeDosNode -- construct dos data structures that a disk needs
*
*   SYNOPSIS
*	struct DeviceNode = MakeDosNode(struct ExpDosNode *)
*
*   FUNCTION
*	This routine manufactures the data structures needed to enter
*	a dos disk device into the system.
*
*   INPUTS
*	see expansion.h for the ExpDosNode
*
*	RESULTS
*	NULL if no memory was available or a pointer to a deviceNode.
*
*/

#include "expansionbase.h"
#include "exec_funcs.h"

#define SysBase ExpansionBase->SysBase

static UINT32 strlen(const char * s)
{
	const char *sc;
	for (sc = s; *sc != '\0'; ++sc)/* nothing */;
	return sc - s;
}

static char* strcpy(char *dst0, const char *src0)
{
	char *s = dst0;
	while (*dst0++ = *src0++);
	return s;
}

static void *copymemfast(void* dest, const void* src, int n) 
{
    const UINT32 *f = src;
    UINT32 *t = dest;
	n = n>>2;

    while (n-- > 0) *t++ = *f++;
    return dest;
}


struct DeviceNode *exp_MakeDosNode(struct ExpansionBase *ExpansionBase, struct ExpDosNode *parameter)
{
	struct DeviceNode 			*dn;
	struct HandlerStartupMsg	*hsm;
	struct DosEnvec				*de;
	INT32	len1, len2;
	APTR	execName, dosName;
	
	if (NULL == parameter) return NULL;
	
	len1 = strlen(parameter->dosName);
	len2 = strlen(parameter->execName);
	
	dn = AllocVec(sizeof(*dn)+sizeof(*hsm)+sizeof(*de), MEMF_FAST|MEMF_CLEAR);
	if (NULL == dn) return NULL;
	
	dosName = AllocVec(len1, MEMF_FAST|MEMF_CLEAR);
	if (NULL == dosName) {FreeVec(dn); return NULL;}
	execName = AllocVec(len2, MEMF_FAST|MEMF_CLEAR);
	if (NULL == execName) {FreeVec(dn); FreeVec(dosName);return NULL;}

	hsm	= (struct HandlerStartupMsg*)(&dn[1]);
	de	= (struct DosEnvec*)(&hsm[1]);

	strcpy(dosName, parameter->dosName);
	strcpy(execName, parameter->execName);

	// this is a struct full of 32bit ints, copy fast
	copymemfast(de, &parameter->dosEnv, sizeof(struct DosEnvec));
	
	hsm->hsm_Unit = parameter->unitNum;
	hsm->hsm_Device = execName;
	hsm->hsm_Environ = de;
	hsm->hsm_Flags = parameter->unitFlags;
	
	dn->dn_Node.ln_Name = dosName;
	dn->dn_Node.ln_Type = DLT_DEVICE;
	dn->dn_Lock			= NULL;
	dn->dn_HandlerStack	= 0x1000;
	dn->dn_HandlerPrio	= 10;
	dn->dn_HandlerStartupMsg = hsm;
	
	return dn;
}


#include "expansionbase.h"
#include "resident.h"

#include "exec_funcs.h"

#include "expansion_protos.h"

#define LIBRARY_VERSION_STRING "\0$VER: expansion.library 0.1 ("__DATE__")\r\n";
#define LIBRARY_VERSION 0
#define LIBRARY_REVISION 1

static const char name[] = "expansion.library";
static const char version[] = LIBRARY_VERSION_STRING
static const char EndResident;

static struct ExpansionBase *exp_Init(struct ExpansionBase *ExpansionBase, UINT32 *segList, APTR SysBase);
APTR exp_OpenLib(struct ExpansionBase *ExpansionBase);
APTR exp_CloseLib(struct ExpansionBase *ExpansionBase);
APTR exp_ExpungeLib(struct ExpansionBase *ExpansionBase);
APTR exp_ExtFuncLib(void);

static APTR FuncTab[] = 
{
	(void(*)) exp_OpenLib,
	(void(*)) exp_CloseLib,
	(void(*)) exp_ExpungeLib,
	(void(*)) exp_ExtFuncLib,
	(void(*)) exp_AddDosNode,
	(void(*)) exp_MakeDosNode,

	(APTR) ((UINT32)-1)
};

static const APTR InitTab[4]=
{
	(APTR)sizeof(struct ExpansionBase),
	(APTR)FuncTab,
	(APTR)NULL,
	(APTR)exp_Init
};

static const struct Resident ROMTag = 
{
	RTC_MATCHWORD,
	(struct Resident *)&ROMTag,
	(APTR)&EndResident,
	RTF_SINGLETASK,
	LIBRARY_VERSION,
	NT_LIBRARY,
	110,
	(char *)name,
	(char*)&version[7],
	0,
	&InitTab
};

static struct ExpansionBase *exp_Init(struct ExpansionBase *ExpansionBase, UINT32 *segList, APTR SysBase)
{
	ExpansionBase->Library.lib_OpenCnt = 0;
	ExpansionBase->Library.lib_Node.ln_Pri = -100;
	ExpansionBase->Library.lib_Node.ln_Type = NT_LIBRARY;
	ExpansionBase->Library.lib_Node.ln_Name = (STRPTR)name;
	ExpansionBase->Library.lib_Version = LIBRARY_VERSION;
	ExpansionBase->Library.lib_Revision = LIBRARY_REVISION;
	ExpansionBase->Library.lib_IDString = (STRPTR)&version[7];	
	ExpansionBase->SysBase	= SysBase;
	ExpansionBase->DosBase	= NULL; // For later use

	NewListType(&ExpansionBase->BoardList, NT_PCILIST);
	InitSemaphore(&ExpansionBase->BoardListLock);

	NewListType(&ExpansionBase->MountList, NT_DOSLIST);	
	InitSemaphore(&ExpansionBase->MountListLock);
	
	return ExpansionBase;	
}

static const char EndResident = 0;


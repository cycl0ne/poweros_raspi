#include "utilitybase.h"
#include "resident.h"

#include "utility_protos.h"

#define LIBRARY_VERSION_STRING "\0$VER: utility.library 0.1 ("__DATE__")\r\n";
#define LIBRARY_VERSION 0
#define LIBRARY_REVISION 1

static const char name[] = "utility.library";
static const char version[] = LIBRARY_VERSION_STRING
static const char EndResident;

static struct UtilityBase *util_Init(struct UtilityBase *UtilityBase, UINT32 *segList, APTR SysBase);
APTR util_OpenLib(struct UtilityBase *UtilityBase);
APTR util_CloseLib(struct UtilityBase *UtilityBase);
APTR util_ExpungeLib(struct UtilityBase *UtilityBase);
APTR util_ExtFuncLib(void);

static APTR FuncTab[] = 
{
	(void(*)) util_OpenLib,
	(void(*)) util_CloseLib,
	(void(*)) util_ExpungeLib,
	(void(*)) util_ExtFuncLib,

	(void(*)) util_IsLower,
	(void(*)) util_IsUpper,
	(void(*)) util_ToUpper,
	(void(*)) util_ToLower,
	(void(*)) util_Strlen,
	(void(*)) util_Strnlen,
	(void(*)) util_Strncasecmp,
	(void(*)) util_Stricmp,
	(void(*)) util_Strnicmp,
	(void(*)) util_Strchr,
	(void(*)) util_Strcpy,
	(void(*)) util_Strrchr,
	(void(*)) util_Strcat,

	(void(*)) util_FindTagItem,
	(void(*)) util_NextTagItem,
	(void(*)) util_GetTagData,
	(void(*)) util_PackBoolTags,

	(void(*)) util_AllocateTagItems,
	(void(*)) util_FreeTagItems,
	(void(*)) util_CloneTagItems,
	(void(*)) util_RefreshTagItemClones,
	(void(*)) util_FilterTagItems,
	(void(*)) util_TagInArray,
	(void(*)) util_CallHookPtr,
	
	(APTR) ((UINT32)-1)
};

static const APTR InitTab[4]=
{
	(APTR)sizeof(struct UtilityBase),
	(APTR)FuncTab,
	(APTR)NULL,
	(APTR)util_Init
};

static const struct Resident ROMTag = 
{
	RTC_MATCHWORD,
	(struct Resident *)&ROMTag,
	(APTR)&EndResident,
	RTF_COLDSTART,
	LIBRARY_VERSION,
	NT_LIBRARY,
	30,
	(char *)name,
	(char*)&version[7],
	0,
	&InitTab
};

static struct UtilityBase *util_Init(struct UtilityBase *UtilityBase, UINT32 *segList, APTR SysBase)
{
	UtilityBase->ut_lib.lib_OpenCnt = 0;
	UtilityBase->ut_lib.lib_Node.ln_Pri = 0;
	UtilityBase->ut_lib.lib_Node.ln_Type = NT_LIBRARY;
	UtilityBase->ut_lib.lib_Node.ln_Name = (STRPTR)name;
	UtilityBase->ut_lib.lib_Version = LIBRARY_VERSION;
	UtilityBase->ut_lib.lib_Revision = LIBRARY_REVISION;
	UtilityBase->ut_lib.lib_IDString = (STRPTR)&version[7];	
	UtilityBase->ut_SysBase	= SysBase;
	
	return UtilityBase;	
}

static const char EndResident = 0;


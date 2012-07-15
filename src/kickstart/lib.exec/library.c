#include "types.h"
#include "list.h"
#include "ports.h"
#include "sysbase.h"

#include "alerts.h"
#include "exec_funcs.h"

struct Library *lib_OpenLibrary(SysBase *SysBase, UINT8 *libName, UINT32 version)
{
  struct Library *library;
  if (libName == NULL) return NULL;
  Forbid();
  library = (struct Library *) FindName(&SysBase->LibList, libName);
  Permit();

  if(library!=NULL)
  {
    if(library->lib_Version>=version)
    {
      library = (((struct Library *(*)(struct Library *)) _GETVECADDR(library,LIB_OPEN))(library));
    }
    else library=NULL;
  }
  return library;
}

void lib_CloseLibrary(SysBase *SysBase,struct Library *library)
{
  if (library == NULL) return;
  Forbid();
  (((void(*)(struct Library *)) _GETVECADDR(library,LIB_CLOSE))(library));
  Permit();
}

void lib_AddLibrary(SysBase *SysBase,struct Library *library)
{
  library->lib_Node.ln_Type=NT_LIBRARY;
  library->lib_Flags|=LIBF_CHANGED;
  SumLibrary(library);

  Forbid();
  //ObtainSemaphore(&SysBase->LockLib);
  Enqueue(&SysBase->LibList,&library->lib_Node);
  Permit();
  //ReleaseSemaphore(&SysBase->LockLib);
}

UINT32 lib_RemLibrary(SysBase *SysBase, struct Library *library)
{
	if (library == NULL) return NULL;
	UINT32 ret;
	//ObtainSemaphore(&SysBase->LockLib);
	Forbid();
	Remove(&library->lib_Node);
	Permit();
	//ReleaseSemaphore(&SysBase->LockLib);
	ret = (((UINT32(*)(struct Library *)) _GETVECADDR(library,LIB_EXPUNGE))(library));
	return ret;
}

void lib_SumLibrary(SysBase *SysBase, struct Library *library)
{
  UINT16 oldflags;
  UINT32 sum;

  Forbid();

  if((library->lib_Flags&LIBF_SUMUSED)&&!(library->lib_Flags&LIBF_SUMMING)){
  /* As long as the library is marked as changed */
  do {
    UINT32 *lp;

    /* Memorize library flags */
    oldflags=library->lib_Flags;
    /* Tell other tasks: Summing in progress */
    library->lib_Flags|=LIBF_SUMMING;
    library->lib_Flags&=~LIBF_CHANGED;

    /* As long as the summing goes multitasking may be permitted. */
    Permit();

    /* Build checksum. Note: library bases are LONG aligned */
    sum=0;
    /* Get start of jumptable */
    lp=(UINT32 *)((UINT8 *)library+library->lib_NegSize);
    /* And sum it up */
    while(lp<(UINT32 *)library) sum+=*lp++;
    /* Summing complete. Arbitrate again. */
    Forbid();
    /* Remove summing flag */
    library->lib_Flags&=~LIBF_SUMMING;

    /* Do it again if the library changed while summing. */
  }while(library->lib_Flags&LIBF_CHANGED);
  if(!(oldflags&LIBF_CHANGED)&&library->lib_Sum!=sum) Alert(AT_DeadEnd|AN_LibChkSum);

  /* Set new checksum */
  library->lib_Sum=sum;
  }
  Permit();
}


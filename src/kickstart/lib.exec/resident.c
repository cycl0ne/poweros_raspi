#include "types.h"
#include "list.h"
#include "ports.h"
#include "sysbase.h"
#include "device.h"
#include "io.h"
#include "resident.h"

#include "exec_funcs.h"

APTR lib_InitResident(SysBase *SysBase, struct Resident *resident, APTR segList)
{
	struct init {
        UINT32 dSize;
        APTR vectors;
        APTR structure;
        UINT32 (*init)();
	};
	
	struct init *init = (struct init *)resident->rt_Init;
	struct Library *library;
	
	if(resident->rt_MatchWord != RTC_MATCHWORD || resident->rt_MatchTag != resident) return NULL;
	/* Depending on the autoinit flag... */
	if(resident->rt_Flags & RTF_AUTOINIT)
	{
	/* ...initialize automatically... */

    	library = MakeLibrary(init->vectors, init->structure, NULL, init->dSize,(UINT32) segList);

	    if(library != NULL)
	    {
    	    library->lib_Node.ln_Type = resident->rt_Type;
	        library->lib_Node.ln_Name = resident->rt_Name;
	        library->lib_Version      = resident->rt_Version;
	        library->lib_IDString     = resident->rt_IdString;
	        library->lib_Flags	      = LIBF_SUMUSED|LIBF_CHANGED;

    	    if(init->init)
	        {
				library = (((struct Library*(*)(struct Library *,APTR, struct SysBase *)) init->init)(library, segList, SysBase));
			
    	    }
    	    if(library != NULL)
	        {	
		        switch(resident->rt_Type)
		        {
    	    	case NT_DEVICE:
	    	    	AddDevice((struct Device *)library);
			        break;
		        case NT_LIBRARY:
			        AddLibrary(library);
			        break;
		        case NT_RESOURCE:
//        			AddResource(library);
	        		break;
		        }
	        }
	    }
	    return library;
    } else
    {
		return library = (((struct Library*(*)(struct Library *,APTR, struct SysBase *)) init->init)(library, segList, SysBase));
    }
}

BOOL lib_RomTagScanner(SysBase *SysBase, UINT32 *start, UINT32 *end)
{
    struct List         *mods= &SysBase->ResidentList;
    struct Resident     *res = NULL;
    struct ResidentNode *node = NULL;
    UINT8 *ptr = (UINT8 *)start;
    
    //DPrintF("RomTagScanner - Start: %x End: %x\n", start, end);
    
    while( ptr <= (UINT8 *)end)
    {
        res = (struct Resident *)ptr;
        
        // Check for a Resident Structure
        if (res->rt_MatchWord == RTC_MATCHWORD && res->rt_MatchTag == res)
        {
            //DPrintF("RomTagScanner - Found Resident: %s\n",res->rt_Name);
            node = AllocVec(sizeof(struct ResidentNode), MEMF_FAST); //MEMF_CLEAR|MEMF_PUBLIC);
            if (!node) return FALSE;
            node->rn_Resident    = res;
            node->rn_Node.ln_Pri = res->rt_Pri;
            // Enqueue found Resident
            Enqueue(mods, &node->rn_Node);
        }   
        ptr++;
    }
    return TRUE;
}

#define LIB_VECTSIZE	4	/* Each library entry takes 4 bytes */
#define LIB_RESERVED	4	/* Exec reserves the first 4 vectors */
#define LIB_BASE	(-LIB_VECTSIZE)
#define LIB_USERDEF	(LIB_BASE-(LIB_RESERVED*LIB_VECTSIZE))
#define LIB_NONSTD	(LIB_USERDEF)

struct Library *lib_MakeLibrary(SysBase *SysBase, APTR funcInit, APTR structInit, UINT32(*libInit)(struct Library*,APTR, struct SysBase*), UINT32 dataSize, UINT32 segList)
{
  struct Library *library;
  UINT32 negsize=0;

  /* Calculate the jumptable's size */
  if(*(INT16 *)funcInit==-1)
  {
    /* Count offsets */
    INT16 *fp=(INT16 *)funcInit+1;
    while(*fp++!=-1) negsize+=LIB_VECTSIZE;
  } else {
    /* Count function pointers */
    void **fp=(void **)funcInit;
    while(*fp++!=(void *)-1) negsize+=LIB_VECTSIZE;
  }

  library=(struct Library *)AllocVec(dataSize+negsize,MEMF_FAST);//MEMF_CLEAR);
  /* And initilize the library */

  if(library!=NULL)
  {
    /* Get real library base */
    library=(struct Library *)((char *)library+negsize);
    /* Build jumptable */
    if(*(INT16 *)funcInit==-1)
    {
      MakeFunctions(library,(INT16 *)funcInit+1,(UINT32)funcInit); // Offsets
    } else
    {
      MakeFunctions(library,funcInit,(UINT32)NULL); // Function Pointers
    }
    library->lib_NegSize=(UINT16)negsize;  // Negsize
    library->lib_PosSize=(UINT16)dataSize; // and DataSize the correct Values
    if(structInit!=NULL) {};//InitStruct(structInit,library,0); // Create Structure
    if(libInit!=NULL)
    libInit(library,(APTR) segList, SysBase); // Call Libinit Function
  }
  return library;
}

void lib_MakeFunctions(SysBase *SysBase, APTR target, APTR functionArray, UINT32 funcDispBase)
{
  INT32 n=1;
  APTR vecaddr;

  if (funcDispBase!=(UINT32)NULL)
  {
    /* If FuncDispBase is non-NULL it's an array of relative offsets */
    INT16 *fp=(INT16 *)functionArray;
    /* -1 terminates the array */
    while(*fp!=-1)
    {
      /* Decrement vector pointer by one and install vector */
      vecaddr = (APTR)((UINT32)target - n*4);
      *(UINT32 *)(((UINT32)vecaddr)) = (UINT32)funcDispBase+*fp;

      /* Use next array entry */
      fp++;
      n++;
    }
  } else {
    /* If FuncDispBase is NULL it's an array of function pointers */
    void **fp=(void **)functionArray;
    /* -1 terminates the array */
    while(*fp!=(void *)-1)
    {
      vecaddr = (APTR)((UINT32)target - n*4);
      *(UINT32 *)(((UINT32)vecaddr)) = (UINT32)*fp;
      /* Use next array entry */
      fp++;
      n++;
    }
  }

  n = (n-1)*4;
}


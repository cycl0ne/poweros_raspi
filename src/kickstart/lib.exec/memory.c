#include "types.h"
#include "sysbase.h"
#include "memory.h"

#include "exec_funcs.h"

//64 Bit align for the ARM
#define ALIGN(x)        (((x)+8-1)&-8)
void lib_hexstrings(UINT32);

void lib_AddMemList(SysBase *SysBase, UINT32 size, UINT32 attribute, INT32 pri, APTR base, STRPTR name)
{
  struct MemHeader *mem;

  mem = (struct MemHeader *)base;
  mem->mh_Node.ln_Pri  = pri;
  mem->mh_Node.ln_Name = name;
  mem->mh_Node.ln_Type = NT_MEMORY;
  mem->mh_Attr = attribute;  
  mem->mh_First = NULL;
  
  mem->mh_Start = (struct MemChunk *)((UINT8 *)mem+(sizeof(struct MemHeader)));
  mem->mh_First = mem->mh_Start+1;
  mem->mh_Start->mc_Bytes = 0;
  mem->mh_Start->mc_Next = mem->mh_First;
  mem->mh_First->mc_Next = mem->mh_Start;
  mem->mh_First->mc_Bytes = ((((UINT8 *)base+size)) - ((UINT8*)base) -sizeof(MemChunk))/sizeof(MemChunk);
/*  
  mem->mh_First = (struct MemChunk *)((UINT8 *)mem+(sizeof(struct MemHeader)));
  mem->mh_First->mc_Next = NULL;
  mem->mh_First->mc_Bytes = size-(sizeof(struct MemHeader));
*/
  mem->mh_Lower = mem->mh_First;
  mem->mh_Upper = (APTR)((UINT8 *) base+size);
  mem->mh_Free  = mem->mh_First->mc_Bytes;//size-(sizeof(struct MemHeader));

  lib_hexstrings(0xd0d0cafe);
  lib_hexstrings((UINT32)mem->mh_Free );
  lib_hexstrings((UINT32)mem->mh_First->mc_Bytes);
  lib_hexstrings((UINT32)mem->mh_Upper );
  lib_hexstrings((UINT32)mem->mh_Lower );

  Disable();
  Enqueue(&SysBase->MemList, &mem->mh_Node);
  Enable();
}

APTR lib_Allocate(SysBase *SysBase, struct MemHeader *mh, UINT32 nbytes)
{  
    MemChunk *p;
    MemChunk *prev;

    if (!nbytes) return NULL;
    nbytes +=ALIGN(8);
    UINT32 nunits = ((nbytes + sizeof(MemChunk))-1)/sizeof(MemChunk) + 1;


	//Forbid();
  //lib_hexstrings(0xd0d0cafe);
  //lib_hexstrings((UINT32)mh->mh_Free );
  //lib_hexstrings((UINT32)nunits);
  //lib_hexstrings((UINT32)nbytes);

	if(mh->mh_Free>=nunits)
	{
		prev = mh->mh_Start;
		p = prev->mc_Next;
		while(1) {
			if (p->mc_Bytes >= nunits) {
				if (p->mc_Bytes == nunits) {
					//an exact fit!
					prev->mc_Next = p->mc_Next;
				} else 
				{
					//block is bigger than we need, split it
					p->mc_Bytes -= nunits;
					p += p->mc_Bytes;
					p->mc_Bytes = nunits;
				}
				mh->mh_Free-=nunits;
				//Permit();
				return (APTR)(p+1);
			}
				
			/*wrapped around - no block found */
			if (p == mh->mh_Start) return NULL;            
			prev = p;
			p = p->mc_Next;
		}
	}
	//Permit();
	return NULL;
}

void lib_Deallocate(SysBase *SysBase,struct MemHeader *mh, APTR memoryBlock)
{
    MemChunk* blk_hdr;
    MemChunk* prev;
    MemChunk* next;
    
    //printf("free %p\r\n", blk);
    if (!memoryBlock) return;
//	Forbid();
    blk_hdr = (MemChunk*)memoryBlock - 1;

    prev = mh->mh_Start;
    next = mh->mh_Start->mc_Next;
    do {
        if (next > blk_hdr) break;   //next is now pointing at the next free block after blk_hdr
        prev = next;
        next = next->mc_Next;
    }
    while (next != mh->mh_Start);
               
    blk_hdr->mc_Next = next;  
    prev->mc_Next = blk_hdr;
	mh->mh_Free += blk_hdr->mc_Bytes;

    //block is now back in the list
    //next we see if we can merge any adjacent blocks with this one
    
    if (blk_hdr + blk_hdr->mc_Bytes == next) {
        blk_hdr->mc_Bytes += blk_hdr->mc_Next->mc_Bytes;
        blk_hdr->mc_Next = blk_hdr->mc_Next->mc_Next;        
        
    }
    if ((prev != mh->mh_Start) && (prev + prev->mc_Bytes == blk_hdr)) 
	{
        prev->mc_Bytes += blk_hdr->mc_Bytes;
        prev->mc_Next = blk_hdr->mc_Next;        
    }
//	Permit();
}

#if 0
static void test_mask(size_t align)
{
    uintptr_t mask = ~(uintptr_t)(align - 1);
    void *mem = malloc(1024+align-1);
    void *ptr = (void *)(((uintptr_t)mem+align-1) & mask);
    assert((align & (align - 1)) == 0);
    printf("0x%08" PRIXPTR ", 0x%08" PRIXPTR "\n", mem, ptr);
    memset_16aligned(ptr, 0, 1024);
    free(mem);
}
#endif

APTR lib_AllocVec(SysBase *SysBase, UINT32 byteSize, UINT32 requirements)
{
    UINT8 *ret = NULL;
    struct MemHeader *mh=(struct MemHeader *)SysBase->MemList.lh_Head;
	while(mh->mh_Node.ln_Succ!=NULL)
	{
		if (mh->mh_Free >= byteSize)
		{
			Forbid();
			ret = Allocate(mh, byteSize);
			Permit();
		}
		mh=(struct MemHeader *)mh->mh_Node.ln_Succ;
	}
	if (ret == NULL) return ret;
	
   	if(requirements&MEMF_CLEAR) MemSet(ret, '\0', byteSize);
	return ret;
}

void lib_FreeVec(SysBase *SysBase, APTR memoryBlock)
{
    if(!memoryBlock) return;
    struct MemHeader *mh=(struct MemHeader *)SysBase->MemList.lh_Head;
    while(mh->mh_Node.ln_Succ)
    {
    	/* Test if the memory belongs to this MemHeader. */
	    if(mh->mh_Lower <= memoryBlock && mh->mh_Upper > memoryBlock)
	    {
			Forbid();
			Deallocate(mh, memoryBlock);
			Permit();
		}
		mh=(struct MemHeader *)mh->mh_Node.ln_Succ;
	}
}





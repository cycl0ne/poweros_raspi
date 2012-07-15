#ifndef memory_h
#define memory_h
#include "types.h"
#include "list.h"

typedef struct MemChunk
{
   struct MemChunk *mc_Next;
   UINT32           mc_Bytes;
}MemChunk;

typedef struct MemHeader
{
   struct Node       mh_Node;
   UINT32            mh_Attr;
   APTR              mh_Lower;
   APTR              mh_Upper;
   UINT32            mh_Free;
   struct MemChunk  *mh_First;
   struct MemChunk  *mh_Start;
}MemHeader;

#define MEMF_ANY		(0L)	/* Any type of memory will do */
#define MEMF_PUBLIC		(1L<<0)
#define MEMF_CHIP		(1L<<1)
#define MEMF_FAST		(1L<<2)

#define MEMF_CLEAR		(1L<<16)
#define MEMF_ALIGN8		(1L<<17)
#define MEMF_ALIGN16	(1L<<18)
#define MEMF_ALIGN32	(1L<<19)
#define MEMF_ALIGN64	(1L<<20)

#define STACK_ITEM_SIZE		4
#define STACK_ALIGNMENT		8 // For ARM
#define ALIGN_DOWN(s, a)  ((s) & ~((a) - 1))
#define ALIGN_UP(s, a)  (((s) + ((a) - 1)) & ~((a) - 1))

#define SP_DELTA  (0 + ALIGN_UP(STACK_ITEM_SIZE, STACK_ALIGNMENT))
#define context_set(c, _pc, stack, size) \
	do { \
		(c)->pc = (UINTPTR) (_pc); \
		(c)->sp = ((UINTPTR) (stack)) + (size) - SP_DELTA; \
		(c)->fp = 0; \
	} while (0)


#endif

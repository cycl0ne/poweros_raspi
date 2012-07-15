/*
*	list.h
*	
*	All functions for list manipulation. 
*
*/

#ifndef list_h
#define list_h

#include "types.h"

#define NT_UNKNOWN    0
#define NT_TASK       1
#define NT_DEVICE     2
#define NT_LIBRARY    3
#define NT_MEMORY     4
#define NT_INTERRUPT  5
#define NT_RESOURCE   6
#define NT_MSGPORT    7
#define NT_MESSAGE    8
#define NT_FREEMSG    9
#define NT_REPLYMSG  10
#define NT_SEMAPHORE 11
#define NT_SIGNALSEM 12
#define NT_HANDLER   13
#define NT_DOSLIST   14
#define NT_PROCESS   15
#define NT_RESIDENT  16
#define NT_PCILIST   17
#define NT_CLIPLIST  18
#define NT_FONT      19
#define NT_GRAPHICS  20


struct Node
{
   struct Node  *ln_Succ,
                *ln_Pred;
   UINT8        *ln_Name;
   UINT8         ln_Type;
   INT8          ln_Pri;
   INT16         ln_Pad;
};

struct MinNode
{
	struct MinNode *mln_Succ;
	struct MinNode *mln_Pred;
};

struct List
{
   struct Node  *lh_Head,
                *lh_Tail,
                *lh_TailPred;
   UINT8         lh_Type;
   UINT8         lh_Pad[3]; // Keep Structure Alligned
};

struct MinList
{
	struct MinNode *mlh_Head;
	struct MinNode *mlh_Tail;
	struct MinNode *mlh_TailPred;
};

typedef struct Node Node;
typedef struct MinNode MinNode;
typedef struct List List;
typedef struct MinList MinList;

#define IsListEmpty(l)\
        ( (((struct List*)l)->lh_TailPred) == (struct Node *)(l) )

#define ForeachNode(l,n) \
   for (n=(void *)(((struct List *)(l))->lh_Head); \
       ((struct Node *)(n))->ln_Succ; \
       n=(void *)(((struct Node *)(n))->ln_Succ))

#define ForeachNodeSafe(l,n,n2)                 \
for                                             \
(                                               \
    n=(void *)(((struct List *)(l))->lh_Head);  \
    (n2=(void *)((struct Node *)(n))->ln_Succ); \
    n=(void *)n2                                \
)

#endif

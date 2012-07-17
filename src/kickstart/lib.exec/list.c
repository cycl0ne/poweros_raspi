/*
*	list.c
*	
*	All functions for list manipulation. 
*
*/

#include "list.h"
#include "sysbase.h"

#include "exec_funcs.h"

void lib_hexstrings(UINT32);
void lib_Print_uart0(const char *s);

void lib_AddHead(SysBase *SysBase, List *list, Node *node);
void lib_AddTail(SysBase *SysBase, List *list, Node *node);
void lib_Enqueue(SysBase *SysBase, List *list, Node *node);
void lib_Insert(SysBase *SysBase, List *list, Node *node, Node *pred);
void lib_NewList(SysBase *SysBase, List *list);
void lib_NewListType(SysBase *SysBase, List *list, UINT8 type);
void lib_Remove(SysBase *SysBase, Node *node);
Node *lib_RemTail(SysBase *SysBase, List *list);
struct Node *lib_RemHead(SysBase *SysBase, struct List *list);
struct Node *lib_FindName(SysBase *SysBase, struct List *liste, STRPTR name);

void lib_AddHead(SysBase *SysBase, List *list, Node *node)
{
   // list o-o newnode o-o node
   node->ln_Succ = list->lh_Head;
   node->ln_Pred = (Node *)&list->lh_Head;
   list->lh_Head->ln_Pred = node;
   list->lh_Head = node;
}

void lib_AddTail(SysBase *SysBase, List *list, Node *node)
{
   //  list o-o node o-o node o-o node o-o newnode
   node->ln_Succ = (Node *)&list->lh_Tail;
   node->ln_Pred = list->lh_TailPred;
   list->lh_TailPred->ln_Succ = node;
   list->lh_TailPred          = node;
}

void lib_Enqueue(SysBase *SysBase, List *list, Node *node)
{
//	lib_Print_uart0("Enqueue\n");
  Node *tmpnode;

  tmpnode = (Node *)list->lh_Head;
  if (tmpnode->ln_Succ == NULL)	AddHead(list,node);
  else
  {
    INT8  pri = node->ln_Pri;
    while (tmpnode->ln_Succ != NULL)
    {
      if (pri > tmpnode->ln_Pri ) break;
      tmpnode=tmpnode->ln_Succ;
    }
    node->ln_Succ = tmpnode;
    node->ln_Pred = tmpnode->ln_Pred;
    tmpnode->ln_Pred->ln_Succ = node;
    tmpnode->ln_Pred = node;
  }
}

void lib_Insert(SysBase *SysBase, List *list, Node *node, Node *pred)
{
	if (pred == NULL)          {AddHead(list,node); return;}
	if (pred->ln_Succ == NULL) {AddTail(list,node); return;}

	node->ln_Succ          = pred->ln_Succ;
	node->ln_Pred          = pred;
	node->ln_Succ->ln_Pred = node;
	pred->ln_Succ          = node;
}

void lib_hexstrings(UINT32);

void lib_NewList(SysBase *SysBase, List *list)
{
    list->lh_Tail = NULL;
    list->lh_Head = (Node*)&list->lh_Tail;
    list->lh_TailPred = (Node*)&list->lh_Head;
}

void lib_NewListType(SysBase *SysBase, List *list, UINT8 type)
{
//	lib_Print_uart0("NewListType\n");
    list->lh_Tail = NULL;
    list->lh_Head = (Node*)&list->lh_Tail;
    list->lh_TailPred = (Node*)&list->lh_Head;
    list->lh_Type = type;
}

void lib_Remove(SysBase *SysBase, Node *node)
{
   node->ln_Succ->ln_Pred = node->ln_Pred;
   node->ln_Pred->ln_Succ = node->ln_Succ;
}

#define GetTail(_l)                                              \
({                                                               \
    struct List *_tmp = (struct List *)(_l),          \
                *l = _tmp;                            \
                                                                 \
    l->lh_TailPred->ln_Pred ? l->lh_TailPred : (struct Node *)0; \
})

Node *lib_RemTail(SysBase *SysBase, List *list)
{
    Node *node;
    if ( (node = GetTail (list)) )
    {
        list->lh_TailPred = node->ln_Pred;
        list->lh_TailPred->ln_Succ = node->ln_Succ;
        node->ln_Succ = NULL;  // Security Reason, we dont want the user to sniff around !
        node->ln_Pred = NULL;
   }
   return node;
}

struct Node *lib_RemHead(SysBase *SysBase, struct List *list)
{
	//if(IsListEmpty(list)) return NULL;
	struct Node *node;
	node = list->lh_Head->ln_Succ;
	if (node)
	{
		node->ln_Pred = (struct Node *)list;
		node = list->lh_Head;
		list->lh_Head = node->ln_Succ;
	}   
#if 0
   struct Node *node = list->lh_Head;
   if (node)
   {
      list->lh_Head = node->ln_Succ;
      list->lh_Head->ln_Pred = node->ln_Pred;
      node->ln_Succ = NULL;  // Security Reason, we dont want the user to sniff around !
      node->ln_Pred = NULL;
   }
#endif
   return node;
}

inline static int strcmp(char *s, char *t)
{
	for (; *s == *t;t++,s++) if (*s == '\0') return 0;
	return *s - *t;
}

struct Node *lib_FindName(SysBase *SysBase, struct List *liste, STRPTR name)
{
   struct Node *tmp=NULL;
   ForeachNode(liste, tmp)
   {
      if (!strcmp(tmp->ln_Name, name)) return tmp;
   }
   return NULL;
}



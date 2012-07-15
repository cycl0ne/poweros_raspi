#ifndef irq_h_ints
#define irq_h_ints

#include "types.h"
#include "list.h"

#include "arch_irq.h"

struct Interrupt {
    struct  Node is_Node;
	iroutine	is_Handler;
	UINT32		is_Cycles;
	UINT32		is_Count;
    APTR    	is_Data;		    /* server data segment  */
    //void    (*is_Code)();	    /* server code entry    */
};

#endif
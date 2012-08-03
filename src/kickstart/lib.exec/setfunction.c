/*
*   $Id: setfunction.c,v 1.2 2004/08/23 20:50:58 cycl0ne Exp $
*
*   NAME
*	SetFunction -- change a function vector in a library
*
*   SYNOPSIS
*	oldFunc = SetFunction(library, funcOffset, funcEntry)
*
*   FUNCTION
*	SetFunction is a functional way of changing those parts of
*	a library that are checksummed.  They are changed in such a
*	way that the summing process will never falsely declare a
*	library to be invalid.
*
*   INPUTS
*	library - a pointer to the library to be changed
*
*	funcOffset - the offset that FuncEntry should be put at. Negative!
*
*	funcEntry - pointer to new function
*
*/

#include "types.h"
#include "sysbase.h"
#include "library.h"
#include "exec_funcs.h"

APTR lib_SetFunction(struct SysBase *SysBase, struct Library *library, INT32 funcOffset, APTR newFunction)
{
	APTR ret;
	UINT32 *vecaddr;
	
	Forbid();
	library->lib_Flags |= LIBF_CHANGED;
	vecaddr = (UINT32 *)((UINT32)library + funcOffset);
	ret     = (APTR *)*(UINT32 *)(((UINT32)vecaddr));
	*(UINT32 *)(((UINT32)vecaddr)) = (UINT32)newFunction;
	ret = 
	Permit();
	//SumLibrary(library);
	return ret;
}
#ifndef expansion_funcs_h
#define expansion_funcs_h

#include "expansion.h"

BOOL AddDosNode(INT32 bootPri, UINT32 flags, struct DeviceNode *deviceNode);
struct DeviceNode *MakeDosNode(struct ExpDosNode *parameter);

#define AddDosNode(a,b,c)	(((BOOL(*)(APTR, INT32, UINT32, struct DeviceNode *)) 	_GETVECADDR(ExpansionBase,5)) (ExpansionBase, a,b,c))
#define MakeDosNode(a)		(((struct DeviceNode *(*)(APTR, struct ExpDosNode *))	_GETVECADDR(ExpansionBase,6)) (ExpansionBase, a))

#endif

#include "expansionbase.h"

BOOL exp_AddDosNode(struct ExpansionBase *ExpansionBase, INT32 bootPri, UINT32 flags, struct DeviceNode *deviceNode);
struct DeviceNode *exp_MakeDosNode(struct ExpansionBase *ExpansionBase, struct ExpDosNode *parameter);

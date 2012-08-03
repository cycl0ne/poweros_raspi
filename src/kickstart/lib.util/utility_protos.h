#include "utilitybase.h"
#include "utility.h"

UINT32 util_IsLower(struct UtilityBase *UtilityBase, UINT32 c);
UINT32 util_IsUpper(struct UtilityBase *UtilityBase, UINT32 c);
UINT32 util_ToUpper(struct UtilityBase *UtilityBase, UINT32 c);
UINT32 util_ToLower(struct UtilityBase *UtilityBase, UINT32 c);
UINT32 util_Strlen(struct UtilityBase *UtilityBase, const char * s);
UINT32 util_Strnlen(struct UtilityBase *UtilityBase, char const *s, UINT32 count);
UINT32 util_Strncasecmp(struct UtilityBase *UtilityBase, const char *s1, const char *s2, UINT32 n);
UINT32 util_Stricmp(struct UtilityBase *UtilityBase, const char *s1, const char *s2);
UINT32 util_Strnicmp(struct UtilityBase *UtilityBase, const char *s1, const char *s2, UINT32 n);
STRPTR util_Strchr(struct UtilityBase *UtilityBase, const char *s, int c);
STRPTR util_Strcpy(struct UtilityBase *UtilityBase, char *dst0, const char *src0);
char* util_Strrchr(struct UtilityBase *UtilityBase, const char *s, int i);
char *util_Strcat(struct UtilityBase *UtilityBase, char *s1, const char *s2);

struct TagItem *util_FindTagItem(struct UtilityBase *UtilityBase, Tag tagValue, struct TagItem *tagList);
struct TagItem *util_NextTagItem(struct UtilityBase *UtilityBase, struct TagItem **tagListPtr);
UINT32 util_GetTagData(struct UtilityBase *UtilityBase, Tag tagValue, IPTR defaultVal, const struct TagItem *tagList);
UINT32 util_PackBoolTags(struct UtilityBase *UtilityBase, UINT32 initialFlags, struct TagItem *tagList, struct TagItem *boolMap);

struct TagItem *util_AllocateTagItems(struct UtilityBase *UtilityBase, UINT32 num);
void util_FreeTagItems(struct UtilityBase *UtilityBase, struct TagItem *list);
struct TagItem *util_CloneTagItems(struct UtilityBase *UtilityBase, const struct TagItem *org);
void util_RefreshTagItemClones(struct UtilityBase *UtilityBase, struct TagItem *clone, const struct TagItem *org);
UINT32 util_FilterTagItems(struct UtilityBase *UtilityBase, struct TagItem *list, Tag *filterArray, UINT32 logic);
BOOL util_TagInArray(struct UtilityBase *UtilityBase, Tag tagValue, Tag *TagArray);
IPTR util_CallHookPtr(struct UtilityBase *UtilityBase, struct Hook *hook, APTR object, APTR paramPacket);

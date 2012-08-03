#include "utilitybase.h"
#include "utility.h"

UINT32 IsLower(UINT32 c);
UINT32 IsUpper(UINT32 c);
UINT32 ToUpper(UINT32 c);
UINT32 ToLower(UINT32 c);
UINT32 Strlen(const char * s);
UINT32 Strnlen(char const *s, UINT32 count);
UINT32 Strncasecmp(const char *s1, const char *s2, UINT32 n);
UINT32 Stricmp(const char *s1, const char *s2);
UINT32 Strnicmp(const char *s1, const char *s2, UINT32 n);
STRPTR Strchr(const char *s, int c);
STRPTR Strcpy(char *dst0, const char *src0);
STRPTR Strrchr(const char *s, int i);
char *Strcat(char *s1, const char *s2);
struct TagItem *FindTagItem(Tag tagValue, struct TagItem *tagList);
struct TagItem *NextTagItem(struct TagItem **tagListPtr);
IPTR GetTagData(Tag tagValue, IPTR defaultVal, const struct TagItem *tagList);
UINT32 PackBoolTags(UINT32 initialFlags, struct TagItem *tagList, struct TagItem *boolMap);

struct TagItem *AllocateTagItems(UINT32 num);
void FreeTagItems(struct TagItem *list);
struct TagItem *CloneTagItems(const struct TagItem *org);
void RefreshTagItemClones(struct TagItem *clone, const struct TagItem *org);
UINT32 FilterTagItems(struct TagItem *list, Tag *filterArray, UINT32 logic);
BOOL TagInArray(Tag tagValue, Tag *TagArray);
IPTR CallHookPtr(struct Hook *hook, APTR object, APTR paramPacket);

#define IsLower(a)				(((UINT32(*)(APTR, UINT32)) _GETVECADDR(UtilityBase,5)) (UtilityBase, a))
#define IsUpper(a)				(((UINT32(*)(APTR, UINT32)) _GETVECADDR(UtilityBase,6)) (UtilityBase, a))
#define ToUpper(a)				(((UINT32(*)(APTR, UINT32)) _GETVECADDR(UtilityBase,7)) (UtilityBase, a))
#define ToLower(a)				(((UINT32(*)(APTR, UINT32)) _GETVECADDR(UtilityBase,8)) (UtilityBase, a))
#define Strlen(a)				(((UINT32(*)(APTR, char const *)) _GETVECADDR(UtilityBase,9)) (UtilityBase, a))
#define Strnlen(a,b)			(((UINT32(*)(APTR, char const *, UINT32)) _GETVECADDR(UtilityBase,10))(UtilityBase, a, b))
#define Strncasecmp(a, b, c)	(((UINT32(*)(APTR, char const *, char const *, UINT32)) _GETVECADDR(UtilityBase,11))(UtilityBase, a, b, c))
#define Stricmp(a,b)			(((UINT32(*)(APTR, char const *, char const *)) _GETVECADDR(UtilityBase,12))(UtilityBase, a, b))
#define Strnicmp(a,b,c)			(((UINT32(*)(APTR, char const *, char const *, UINT32)) _GETVECADDR(UtilityBase,13))(UtilityBase, a, b, c))

#define Strchr(a,b)			(((STRPTR(*)(APTR, char const *, UINT32)) _GETVECADDR(UtilityBase,14))(UtilityBase, a, b))
#define Strcpy(a,b)			(((STRPTR(*)(APTR, char *, const char *)) _GETVECADDR(UtilityBase,15))(UtilityBase, a, b))
#define Strrchr(a,b)		(((STRPTR(*)(APTR, const char *, int)) _GETVECADDR(UtilityBase,16))(UtilityBase, a, b))
#define Strcat(a,b)			(((STRPTR(*)(APTR, char *, const char *)) _GETVECADDR(UtilityBase,17))(UtilityBase, a, b))

#define FindTagItem(a,b)		(((struct TagItem *(*)(APTR, Tag, struct TagItem *)) _GETVECADDR(UtilityBase,18))(UtilityBase, a, b))
#define NextTagItem(a)		(((struct TagItem *(*)(APTR, struct TagItem **)) _GETVECADDR(UtilityBase,19))(UtilityBase, a))
#define GetTagData(a,b,c)		(((IPTR (*)(APTR, Tag, IPTR, const struct TagItem *)) _GETVECADDR(UtilityBase,20))(UtilityBase, a, b,c))
#define PackBoolTags(a,b,c)		(((UINT32 (*)(APTR, UINT32, struct TagItem *, struct TagItem *)) _GETVECADDR(UtilityBase,21))(UtilityBase, a, b,c))

#define AllocateTagItems(a)		(((struct TagItem *(*)(APTR, UINT32)) _GETVECADDR(UtilityBase,22))(UtilityBase, a))
#define FreeTagItems(a)			(((void(*)(APTR, struct TagItem *)) _GETVECADDR(UtilityBase,23))(UtilityBase, a))
#define CloneTagItems(a)			(((struct TagItem *(*)(APTR, const struct TagItem *org)) _GETVECADDR(UtilityBase,24))(UtilityBase, a))
#define RefreshTagItemClones(a,b)	(((void(*)(APTR, struct TagItem *clone, const struct TagItem *org)) _GETVECADDR(UtilityBase,25))(UtilityBase, a, b))
#define FilterTagItems(a,b,c)		(((UINT32 (*)(APTR, struct TagItem *, Tag *, UINT32)) _GETVECADDR(UtilityBase,26))(UtilityBase, a, b,c))
#define TagInArray(a, b)			(((BOOL(*)(APTR, Tag, Tag *)) _GETVECADDR(UtilityBase,27))(UtilityBase, a, b))

#define CallHookPtr(a,b,c)		(((IPTR (*)(APTR, struct Hook *, APTR, APTR)) _GETVECADDR(UtilityBase,28))(UtilityBase, a, b,c))

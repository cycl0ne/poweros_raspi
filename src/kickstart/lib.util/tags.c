#include "utilitybase.h"
#include "utility.h"
#include "exec_funcs.h"
#include "utility_funcs.h"
#define SysBase UtilityBase->ut_SysBase

struct TagItem *util_NextTagItem(struct UtilityBase *UtilityBase, struct TagItem **tagListPtr)
{
    if (!(*tagListPtr)) return NULL;

    while(1)
    {
        switch((*tagListPtr)->ti_Tag)
        {
            case TAG_MORE:
                if (!((*tagListPtr) = (struct TagItem *)(*tagListPtr)->ti_Data)) return NULL;
                continue;
            case TAG_IGNORE:
                break;
            case TAG_END:
                (*tagListPtr) = 0;
                return NULL;
            case TAG_SKIP:
                (*tagListPtr) += (*tagListPtr)->ti_Data + 1;
                continue;
            default:
                return (*tagListPtr)++;
        }
        (*tagListPtr)++;
    }
}

struct TagItem *util_FindTagItem(struct UtilityBase *UtilityBase, Tag tagValue, struct TagItem *tagList)
{
    struct TagItem *tstate = (struct TagItem *)tagList;
    struct TagItem *tag;

    while ((tag = NextTagItem(&tstate)))
    {
	if (tag->ti_Tag == tagValue)
	    return tag;
    }

    return NULL;
}

UINT32 util_GetTagData(struct UtilityBase *UtilityBase, Tag tagValue, IPTR defaultVal, const struct TagItem *tagList)
{
    struct TagItem *tag = FindTagItem(tagValue, (struct TagItem *)tagList); 
    return tag ? tag->ti_Data : defaultVal;
}

UINT32 util_PackBoolTags(struct UtilityBase *UtilityBase, UINT32 initialFlags, struct TagItem *tagList, struct TagItem *boolMap)
{
    struct TagItem *current, *found, *tstate = tagList;

    while ((current = NextTagItem(&tstate)))
    {
		if ((found = FindTagItem (current->ti_Tag, boolMap)))
		{
			if (current->ti_Data == 0) initialFlags &= ~(found->ti_Data);
			else initialFlags |= found->ti_Data;
		}
    }

    return initialFlags;
}

struct TagItem *util_CloneTagItems(struct UtilityBase *UtilityBase, const struct TagItem *org)
{
	struct TagItem *newList;
	INT32 numTags = 1;

	if (org)
	{
		struct TagItem *tmp;
		tmp = (struct TagItem *)org;
		while (NextTagItem (&tmp) != NULL)
		numTags++;
	}
    if ((newList = AllocateTagItems(numTags))) RefreshTagItemClones(newList, org);
    return newList;
}

struct TagItem *util_AllocateTagItems(struct UtilityBase *UtilityBase, UINT32 num)
{
    struct TagItem *tags = NULL;
    if (num) tags = AllocVec( num * sizeof(struct TagItem) , MEMF_CLEAR | MEMF_FAST );
    return tags;
}

void util_FreeTagItems(struct UtilityBase *UtilityBase, struct TagItem *list)
{
    if( list ) FreeVec( list );
}

void util_RefreshTagItemClones(struct UtilityBase *UtilityBase, struct TagItem *clone, const struct TagItem *org)
{
    struct TagItem *current;

    if(clone)
	{
		if(org)
		{
			while ((current = NextTagItem ((struct TagItem **)&org)))
			{
				*clone = *current;
				clone++;
			}
			return;
		}
		clone->ti_Tag = TAG_DONE;
	}
}

UINT32 util_FilterTagItems(struct UtilityBase *UtilityBase, struct TagItem *list, Tag *filterArray, UINT32 logic)
{
    UINT32 valid = 0;
	if(list && filterArray)
	{
		struct TagItem *ti;

		while((ti = NextTagItem(&list)))
		{
			if(logic == TAGFILTER_AND)
			{
				if(TagInArray(ti->ti_Tag, filterArray)) valid++;
				else ti->ti_Tag = TAG_IGNORE;
			} else if(logic == TAGFILTER_NOT)
			{
				if(TagInArray(ti->ti_Tag, filterArray)) ti->ti_Tag = TAG_IGNORE;
				else valid++;
			}
		}
	}
	return valid;
}

BOOL util_TagInArray(struct UtilityBase *UtilityBase, Tag tagValue, Tag *tagArray)
{
    while(*tagArray != TAG_DONE)
    {
		if (*tagArray == tagValue) return TRUE;
		tagArray++;
    }
    return FALSE;
}

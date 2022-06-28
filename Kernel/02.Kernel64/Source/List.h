#ifndef __LIST_H__
#define __LIST_H__

#include "Types.h"

#pragma pack( push, 1)

typedef struct kListLinkStruct
{
    void* pvNext;
    QWORD qwID;
} LISTLINK;

struct kListNode
{
    LISTLINK stLink;

    int iData1;
    char cData2;
};

typedef struct kList
{
    int node_count;

    void* head;
    void* tail;
}LIST;

#pragma pack( pop )


void k_init_list(LIST* list);
int k_get_list_count(const LIST* list);
void k_add_list_to_tail(LIST* list, void* item);
void k_add_list_to_head(LIST* list, void* item);
void* k_remove_list(LIST* list, QWORD id);
void* k_remove_list_from_head(LIST* list);
void* k_remove_list_from_tail(LIST* list);
void* k_find_list(const LIST* list, QWORD id);
void* k_get_head_from_list(const LIST* list);
void* k_get_tail_from_list(const LIST* list);
void* k_get_next_from_list(const LIST* list, void* current);

#endif 
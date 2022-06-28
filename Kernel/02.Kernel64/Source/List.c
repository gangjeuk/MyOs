#include "List.h"

void k_init_list(LIST* list)
{
    list->node_count = 0;
    list->head = NULL;
    list->tail = NULL;
}

int k_get_list_count(const LIST* list)
{
    return list->node_count;
}

void k_add_list_to_tail(LIST* list, void* item)
{
    LISTLINK* link;

    link = (LISTLINK *) item;
    link->pvNext = NULL;

    if(list->head == NULL)
    {
        list->head = item;
        list->tail = item;
        list->node_count = 1;

        return;
    }

    link = (LISTLINK *) list->tail;
    link->pvNext = item;

    list->tail = item;
    list->node_count += 1;
}

void k_add_list_to_head(LIST* list, void* item)
{
    LISTLINK* link;

    link = (LISTLINK *) item;
    link->pvNext = list->head;

    if(list->head == NULL)
    {
        list->head = item;
        list->tail = item;
        list->node_count = 1;

        return;
    }

    list->head = item;
    list->node_count += 1;
}
void* k_remove_list(LIST* list, QWORD id)
{
    LISTLINK* link;
    LISTLINK* prev_link;

    prev_link = (LISTLINK*) list->head;
    for(link = prev_link; link != NULL; link = link->pvNext)
    {
        if(link->qwID == id)
        {
            if(link == list->head && link == list->tail)
            {
                list->head = NULL;
                list->tail = NULL;
            }
            else if(link == list->head)
            {
                list->head = link->pvNext;
            }
            else if(link == list->tail)
            {
                list->tail = prev_link;
            }
            else
            {
                prev_link->pvNext = link->pvNext;
            }

            list->node_count -= 1;
            return link;
        }
        prev_link = link;
    }
    return NULL;
}
void* k_remove_list_from_head(LIST* list)
{
    LISTLINK* link;

    if(list->node_count == 0)
    {
        return NULL;
    }
    link = (LISTLINK*) list->head;
    return k_remove_list(list, link->qwID);
}
void* k_remove_list_from_tail(LIST* list)
{
    LISTLINK* link;
    if(list->node_count == 0)
    {
        return NULL;
    }
    link = (LISTLINK*) list->tail;
    return k_remove_list(list, link->qwID);
}
void* k_find_list(const LIST* list, QWORD id)
{
    LISTLINK* link;

    if(list->node_count == 0)
    {
        return NULL;
    }
    
    for(link = (LISTLINK*)list->head; link != NULL; link = link->pvNext)
    {
        if(link->qwID == id)
        {
            return link;
        }
    }
    return NULL;
}
void* k_get_head_from_list(const LIST* list)
{
    return list->head;
}
void* k_get_tail_from_list(const LIST* list)
{
    return list->tail;
}
void* k_get_next_from_list(const LIST* list, void* current)
{
    return ((LISTLINK*)current)->pvNext;
}
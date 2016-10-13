#include <stdlib.h>
#include <stdio.h>

#include "list.h"

int list_add(list_t *e, val_t val)
{
    e->next = malloc(sizeof(list_t));
    e = e->next;
    e->next = NULL;
    e->data = val;
    return 0;
}

list_t *list_mid(list_t *list)
{
    if(!list)	return list;
    list_t *fast = list;
    list_t *slow = list;
    while(fast->next || fast->next->next != NULL) {
        fast = fast->next->next;
        slow = slow->next;
    }
    return slow;
}

void list_print(list_t *list)
{
    while(list) {
        printf("[%ld]",list->data);
        list = list->next;
    }
}

int list_size(list_t *list)
{
    int count = 0;
    while(list) {
        count++;
        list = list->next;
    }
    return count;
}

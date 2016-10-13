#ifndef LIST_H_
#define LIST_H_

#include <stdint.h>

typedef intptr_t val_t;

typedef struct __LIST_T {
    val_t data;
    struct node *next;
} list_t;

list_t *list_mid(list_t *list);
int list_add(list_t *list, val_t val);
void list_print(list_t *list);
int list_size(list_t *list)
#endif

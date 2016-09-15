#ifndef __LIST_H__
#define __LIST_H__

#include <stdbool.h>

typedef struct list list_t;

list_t* list_new();

void list_destroy(list_t* list);

void list_append(list_t* list, void* elem);

void list_prepend(list_t* list, void* elem);

bool list_insert(list_t* list, int index, void* elem);

bool list_remove(list_t* list, int index, void** elem);

void** list_get(list_t* list, int index);

void* list_first(list_t* list);

void* list_last(list_t* list);

int list_length(list_t* list);

#endif /* __LIST_H__ */



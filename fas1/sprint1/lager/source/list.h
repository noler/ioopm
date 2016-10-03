#ifndef __LIST_H__
#define __LIST_H__

#include <stdbool.h>

typedef struct list list_t;
typedef struct list_it list_it_t;

list_t* list_new();
void list_destroy(list_t* list);

int list_length(list_t* list);

void list_append(list_t* list, void* elem);
void list_prepend(list_t* list, void* elem);
bool list_insert(list_t* list, int index, void* elem);
bool list_remove(list_t* list, int index, void** elem);
void** list_get(list_t* list, int index);
void* list_first(list_t* list);
void* list_last(list_t* list);

list_it_t* list_it_new(list_t*);
void list_it_destroy(list_it_t*);

bool list_it_first(list_it_t*);
bool list_it_last(list_it_t*);

void** list_it_current(list_it_t*);
void** list_it_next(list_it_t*);
void** list_it_prev(list_it_t*);
void list_it_seek(list_it_t*, int index);
int list_it_index(list_it_t*);

#endif /* __LIST_H__ */



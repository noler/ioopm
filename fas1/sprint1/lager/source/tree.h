#ifndef __TREE_H__
#define __TREE_H__

#include <stdbool.h>

typedef struct tree tree_t;

typedef int (*comp_func)(void*, void*);

tree_t* tree_new();

void tree_destroy(tree_t*);

int tree_size(tree_t* tree);

int tree_depth(tree_t* tree);

void tree_insert(tree_t* tree, void* key, void* value, comp_func comp);

void* tree_remove(tree_t* tree, void** key, comp_func comp);

void* tree_search(tree_t* tree, void* key, comp_func comp);

int tree_comp_int(void* a, void* b);
int tree_comp_str(void* a, void* b);
int tree_comp_strp(void* a, void* b);

#endif /* __TREE_H__ */

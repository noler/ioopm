#ifndef __TREE_H__
#define __TREE_H__

#include <stdbool.h>

/**
 * @file
 * @author Andreas Olofsson
 * @version 1.0
 */

/**
 * @brief Represents a tree containing key-value pairs.
 * 
 * The tree contains key-value pairs where keys must be comparable and unique.
 */
typedef struct tree tree_t;

/**
 * @brief Respresents a traverser for a tree.
 *
 * The traverser can be pre-, post-, in- or level-order.
 * The traverser points to one of the nodes in the tree or is 'before'/'after' all nodes in the tree.
 */
typedef struct tree_tr tree_tr_t;

typedef enum {
	tree_tr_pre_order,
	tree_tr_in_order,
	tree_tr_post_order,
	tree_tr_level_order
} tree_tr_order;

/**
 * @brief A comparison function.
 *
 * Compares two value and returns
 * a negative value if a is less than b,
 * zero if a and b are equal and
 * a positive value if a is greater than b.
 */
typedef int (*comp_func)(void* a, void* b);

/**
 * @brief Creates a new empty tree.
 *
 * @return The new tree.
 */
tree_t* tree_new();

/**
 * @brief Destroys the given tree and frees any memory allocated for the tree.
 *
 * Any use of the tree after tree_destroy is called is not valid.
 *
 * Completes in @f$ O(n) @f$ time.
 *
 * @param tree The tree to destroy.
 */
void tree_destroy(tree_t* tree);

/**
 * @brief Gives the number of nodes in the given tree.
 *
 * Completes in @f$ O(n) @f$ time.
 *
 * @param tree The tree to count the nodes in.
 * @return The number of nodes in the tree.
 */
int tree_size(tree_t* tree);

/**
 * @brief Gives the longest path from the root node to any other node.
 *
 * Completes in @f$ O(n) @f$ time.
 *
 * @param tree The tree to measure the depth of.
 * @return The depth of the tree.
 */
int tree_depth(tree_t* tree);

/**
 * @brief Insert a key-value pair as a new node in the given tree.
 *
 * If the key equals any other key in the tree that keys associated value is replace by the given value.
 *
 * Completes in @f$ O(log n) @f$ time.
 *
 * @param tree The tree to insert into.
 * @param key The key of the node to insert.
 * @param value The value of the node to insert.
 * @param comp A function to compare the given key to any other key in the tree.
 */
void tree_insert(tree_t* tree, void* key, void* value, comp_func comp);

/**
 * @brief Removes the node from the tree with the given key.
 *
 * If the key doesn't exist in the tree then no change is made to the tree.
 *
 * Completes in @f$ O(log n) @f$ time.
 *
 * @param tree The tree to remove from.
 * @param key The key of the node to remove.
 * @param comp A function to compare the given key to any other key in the tree.
 */
void* tree_remove(tree_t* tree, void* key, comp_func comp);

/**
 * @brief Search for a value in the tree.
 *
 * Completes in @f$ O(log n) @f$ time.
 *
 * @param tree The tree to search in.
 * @param key The key of the node to find the value of.
 * @param comp A function to compare the given key to any other key in the tree.
 */
void* tree_search(tree_t* tree, void* key, comp_func comp);

tree_tr_t* tree_tr_new(tree_t* tree, tree_tr_order order);
void tree_tr_destroy(tree_tr_t* tr);

void** tree_tr_next(tree_tr_t* tr);
void** tree_tr_prev(tree_tr_t* tr); // Unimplemented
void** tree_tr_current_key(tree_tr_t* tr);
void** tree_tr_current_value(tree_tr_t* tr);

bool tree_tr_first(tree_tr_t* tr);
bool tree_tr_last(tree_tr_t* tr);
bool tree_tr_after(tree_tr_t* tr);

int tree_comp_eq(void* a, void* b);
int tree_comp_int(void* a, void* b);
int tree_comp_str(void* a, void* b);
int tree_comp_strp(void* a, void* b);

#endif /* __TREE_H__ */

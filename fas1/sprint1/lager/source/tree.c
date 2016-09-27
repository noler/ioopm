#include "tree.h"

#include <stdio.h> // DEBUG
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

struct tree;
struct tree_node;
typedef struct tree_node node_t;

struct tree_node {
	node_t* left, *right;
	bool red;
	void* key, *value;
};

struct tree {
	node_t* nodes;
};

void _debug_print_tree(node_t* node, int level, int* indent) {
	if(node == 0) {
		if(level == 0) {
			for(; *indent > 0; (*indent)--) {
				printf(" ");
			}
			printf(" -- ");
		} else {
			*indent += 4;
		}
	} else {
		_debug_print_tree(node->left, level - 1, indent);
		if(level == 0) {
			for(; *indent > 0; (*indent)--) {
				printf(" ");
			}
			printf(" %d%c ", *((int*) node->key), node->red ? 'R' : 'B');
		} else {
			*indent += 4;
		}
		_debug_print_tree(node->right, level - 1, indent);
	}
}

void debug_print_tree(tree_t* tree) {
	int depth = tree_depth(tree);
	for(int i = 0; i <= depth; i++) {
		int indent = 0;
		_debug_print_tree(tree->nodes, i, &indent);
		printf("\n");
	}
}

tree_t* tree_new() {
	tree_t* tree = malloc(sizeof(tree_t));
	tree->nodes = 0;
	return tree;
}

void _tree_destroy(node_t* node) {
	if(node == 0) {
		return;
	}

	_tree_destroy(node->left);
	_tree_destroy(node->right);

	free(node);
}

void tree_destroy(tree_t* tree) {
	_tree_destroy(tree->nodes);

	free(tree);
}

int _tree_size(node_t* node) {
	if(node == 0) {
		return 0;
	}

	return 1 + _tree_size(node->left) + _tree_size(node->right);
}

int tree_size(tree_t* tree) {
	return _tree_size(tree->nodes);
}

int _tree_depth(node_t* node) {
	if(node == 0) {
		return 0;
	}

	return 1 + max(_tree_depth(node->left), _tree_depth(node->right));
}

int tree_depth(tree_t* tree) {
	return _tree_depth(tree->nodes);
}

void _tree_balance(node_t** node_ptr) {
	node_t* node = *node_ptr;

	if(!node->red) {
		if(node->left != 0 && node->left->red &&
				node->left->left != 0 && node->left->left->red) {
			if(node->right != 0 && node->right->red) {
				node->red = true;
				node->left->red = false;
				node->right->red = false;
			} else {
				node_t* new_node = node->left;
				node_t* new_rl = new_node->right;
				new_node->right = node;
				new_node->right->left = new_rl;
				new_node->red = false;
				new_node->right->red = true;
				*node_ptr = new_node;
			}
		} else if(node->left != 0 && node->left->red &&
				node->left->right != 0 && node->left->right->red) {
			if(node->right != 0 && node->right->red) {
				node->red = true;
				node->left->red = false;
				node->right->red = false;
			} else {
				node_t* new_node = node->left->right;
				node_t* new_lr = new_node->left;
				new_node->left = node->left;
				new_node->left->right = new_lr;
				new_node->right = node;
				new_node->red = false;
				new_node->right->red = true;
				*node_ptr = new_node;
			}
		} else if(node->right != 0 && node->right->red &&
				node->right->right != 0 && node->right->right->red) {
			if(node->left != 0 && node->left->red) {
				node->red = true;
				node->right->red = false;
				node->left->red = false;
			} else {
				node_t* new_node = node->right;
				node_t* new_lr = new_node->left;
				new_node->left = node;
				new_node->left->right = new_lr;
				new_node->red = false;
				new_node->left->red = true;
				*node_ptr = new_node;
			}
		} else if(node->right != 0 && node->right->red &&
				node->right->left != 0 && node->right->left->red) {
			if(node->left != 0 && node->left->red) {
				node->red = true;
				node->right->red = false;
				node->left->red = false;
			} else {
				node_t* new_node = node->right->left;
				node_t* new_rl = new_node->right;
				new_node->right = node->right;
				new_node->right->left = new_rl;
				new_node->left = node;
				new_node->red = false;
				new_node->left->red = true;
				*node_ptr = new_node;
			}
		}
	}
}

node_t* _tree_insert(node_t* node, void* key, void* value, comp_func comp) {
	if(node == 0) {
		node = malloc(sizeof(node_t));
		node->red = true;
		node->left = node->right = 0;
		node->key = key;
		node->value = value;
		return node;
	} else {
		int side = comp(node->key, key);
		if(side == 0) {
			side = rand() % 2 == 0 ? 1 : -1;
		}
		if(side < 0) {
			node->left = _tree_insert(node->left, key, value, comp);
			_tree_balance(&node);
			return node;
		} else if(side > 0) {
			node->right = _tree_insert(node->right, key, value, comp);
			_tree_balance(&node);
			return node;
		}
	}
	return 0;
}

void tree_insert(tree_t* tree, void* key, void* value, comp_func comp) {
	tree->nodes = _tree_insert(tree->nodes, key, value, comp);
	tree->nodes->red = false;
}

node_t* _tree_remove(node_t* node, void** key, comp_func comp, bool* balance, void** value, bool inorder) {
	if(node == 0) {
		*value = 0;
		return 0;
	}

	int side;
	if(!inorder) {
		side = comp(node->key, *key);
	} else {
		side = node->right == 0 ? 0 : 1;
	}

	if(side == 0) {
		*key = node->key;
		*value = node->value;
		if(node->left == 0 && node->right == 0) {
			*balance = !node->red;
			free(node);
			return 0;
		} else if(node->left != 0 && node->right != 0) {
			void* new_key, *new_value;
			_tree_remove(node->left, &new_key, 0, balance, &new_value, true);
			node->key = new_key;
			node->value = new_value;
			*balance = false;
			return node;
		} else if(node->left != 0) {
			node->key = node->left->key;
			node->value = node->left->value;
			node->red = false;
			free(node->left);
			node->left = 0;
		} else { // node->right != 0
			node->key = node->right->key;
			node->value = node->right->value;
			node->red = false;
			free(node->right);
			node->right = 0;
		}

		return node;
	} else {
		*balance = false;

		if(side < 0) {
			node->left = _tree_remove(node->left, key, comp, balance, value, false);

			if(balance) {
				if(node->right == 0 || !node->right->red) {
					if(node->right->right != 0 && node->right->right->red) {
						node_t* new_node = node->right;
						node_t* new_lr = new_node->left;
						new_node->left = node;
						new_node->left->right = new_lr;
						new_node->right->red = false;
						*balance = false;
						return new_node;
					} else if(node->right->left != 0 && node->right->left->red) {
						node_t* new_node = node->right->left;
						node_t* new_lr = new_node->left;
						node_t* new_rl = new_node->right;
						new_node->left = node;
						new_node->right = node->right;
						new_node->left->right = new_lr;
						new_node->right->left = new_rl;
						new_node->red = false;
						*balance = false;
						return new_node;
					} else {
						node->right->red = true;
						return node;
					}
				} else {
					node_t* new_node = node->right;
					node_t* new_lr = new_node->left;
					new_node->left = node;
					new_node->left->right = new_lr;
					new_node->red = false;
					new_node->left->red = true;
					*balance = false;
					return new_node;
				}
			}
		} else {
			node->right = _tree_remove(node->right, key, comp, balance, value, false);

			if(balance) {
				if(node->left == 0 || !node->left->red) {
					if(node->left->left != 0 && node->left->left->red) {
						node_t* new_node = node->left;
						node_t* new_lr = new_node->right;
						new_node->right = node;
						new_node->right->left = new_lr;
						new_node->left->red = false;
						*balance = false;
						return new_node;
					} else if(node->left->right != 0 && node->left->right->red) {
						node_t* new_node = node->left->right;
						node_t* new_lr = new_node->right;
						node_t* new_rl = new_node->left;
						new_node->right = node;
						new_node->left = node->left;
						new_node->right->left = new_lr;
						new_node->left->right = new_rl;
						new_node->red = false;
						*balance = false;
						return new_node;
					} else {
						node->left->red = true;
						return node;
					}
				} else {
					node_t* new_node = node->left;
					node_t* new_lr = new_node->right;
					new_node->right = node;
					new_node->right->left = new_lr;
					new_node->red = false;
					new_node->right->red = true;
					*balance = false;
					return new_node;
				}
			}
		}

		return node;
	}

	return node;
}

void* tree_remove(tree_t* tree, void* key, comp_func comp) {
	bool balance;
	void* value;

	tree->nodes = _tree_remove(tree->nodes, &key, comp, &balance, &value, false);

	return value;
}

void* _tree_search(node_t* node, void* key, comp_func comp) {
	if(node == 0) return 0;

	int side = comp(node->key, key);

	if(side < 0) {
		return _tree_search(node->left, key, comp);
	} else if(side > 0) {
		return _tree_search(node->right, key, comp);
	} else {
		return node->value;
	}
}

void* tree_search(tree_t* tree, void* key, comp_func comp) {
	return _tree_search(tree->nodes, key, comp);
}

int tree_comp_int(void* a, void* b) {
	return *((int*) b) - *((int*) a);
}

int tree_comp_str(void* a, void* b) {
	return strcmp((char*) a, (char*) b);
}

int tree_comp_strp(void* a, void* b) {
	return strcmp(*(char**) a, *(char**) b);
}

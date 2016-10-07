#include "tree.h"

#include "list.h"

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

struct tree_tr {
	tree_t* tree;
	tree_tr_order order;
	struct tree_node* node;
	list_t* stack;
	node_t* first, last;
};

void tree_debug_print(tree_t*, int);
bool tree_debug_rb_validate(tree_t*);

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
				node_t* new_rl = new_node->right;
				new_node->left = node->left;
				new_node->left->right = new_lr;
				new_node->right = node;
				new_node->right->left = new_rl;
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
				node_t* new_lr = new_node->left;
				new_node->right = node->right;
				new_node->right->left = new_rl;
				new_node->left = node;
				new_node->left->right = new_lr;
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
			node->value = value;
		} else if(side < 0) {
			node->left = _tree_insert(node->left, key, value, comp);
			_tree_balance(&node);
		} else if(side > 0) {
			node->right = _tree_insert(node->right, key, value, comp);
			_tree_balance(&node);
		}
		return node;
	}
	return 0;
}

void tree_insert(tree_t* tree, void* key, void* value, comp_func comp) {
	tree->nodes = _tree_insert(tree->nodes, key, value, comp);
	tree->nodes->red = false;
}

node_t* _tree_balance_db(node_t* node, bool left, bool* balance) {
	if(!*balance) {
		return node;
	} else {
		if(left) {
			if(node->right == 0) {
				node->red = false;
				*balance = false;
				return node;
			} else if(!node->right->red) {
				if(node->right->right != 0 && node->right->right->red) {
					node_t* new_node = node->right;
					node_t* new_lr = new_node->left;
					new_node->left = node;
					new_node->left->right = new_lr;
					if(node->red) {
						new_node->red = true;
						new_node->left->red = false;
						new_node->right->red = false;
					} else {
						new_node->left->red = true;
					}
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
					if(node->red) {
						node->red = false;
						*balance = false;
					}
					return node;
				}
			} else {
				node_t* new_node = node->right;
				node_t* new_lr = new_node->left;
				new_node->left = node;
				new_node->left->right = new_lr;
				new_node->red = false;
				new_node->left->red = true;
				new_node->left = _tree_balance_db(new_node->left, true, balance);
				return new_node;
			}
		} else {
			if(node->left == 0) {
				node->red = false;
				*balance = false;
				return node;
			} else if(!node->left->red) {
				if(node->left->left != 0 && node->left->left->red) {
					node_t* new_node = node->left;
					node_t* new_lr = new_node->right;
					new_node->right = node;
					new_node->right->left = new_lr;
					if(node->red) {
						new_node->red = true;
						new_node->right->red = false;
						new_node->left->red = false;
					} else {
						new_node->right->red = true;
					}
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
					if(node->red) {
						node->red = false;
						*balance = false;
					}
					return node;
				}
			} else {
				node_t* new_node = node->left;
				node_t* new_rl = new_node->right;
				new_node->right = node;
				new_node->right->left = new_rl;
				new_node->red = false;
				new_node->right->red = true;
				new_node->right = _tree_balance_db(new_node->right, false, balance);
				return new_node;
			}
		}
	}
}

node_t* _tree_remove_inorder(node_t* node, void** key, void** value, bool* balance) {
	if(node->right == 0) {
		*key = node->key;
		*value = node->value;
		if(node->left == 0) {
			*balance = !node->red;
			free(node);
			return 0;
		} else {
			node->key = node->left->key;
			node->value = node->left->value;
			free(node->left);
			node->left = 0;
			return node;
		}
	} else {
		node->right = _tree_remove_inorder(node->right, key, value, balance);

		return _tree_balance_db(node, false, balance);
	}
}

node_t* _tree_remove(node_t* node, void* key, comp_func comp, bool* balance, void** value) {
	if(node == 0) {
		*value = 0;
		return 0;
	}

	int side = comp(node->key, key);
	if(side == 0) {
		*value = node->value;
		if(node->left == 0 && node->right == 0) {
			*balance = !node->red;
			free(node);
			return 0;
		} else if(node->left != 0 && node->right != 0) {
			node->left = _tree_remove_inorder(node->left, &node->key, &node->value, balance);

			return _tree_balance_db(node, true, balance);
		} else if(node->left != 0) { // node->right == 0
			node->key = node->left->key;
			node->value = node->left->value;
			free(node->left);
			node->left = 0;
			return node;
		} else { // node->left == 0 && node->right != 0
			node->key = node->right->key;
			node->value = node->right->value;
			free(node->right);
			node->right = 0;
			return node;
		}
	} else if(side < 0) {
		node->left = _tree_remove(node->left, key, comp, balance, value);

		return _tree_balance_db(node, true, balance);
	} else { /* side > 0 */
		node->right = _tree_remove(node->right, key, comp, balance, value);

		return _tree_balance_db(node, false, balance);
	}
}

void* tree_remove(tree_t* tree, void* key, comp_func comp) {
	void* result;
	bool balance = false;

	tree->nodes = _tree_remove(tree->nodes, key, comp, &balance, &result);

	return result;
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

tree_tr_t* tree_tr_new(tree_t* tree, tree_tr_order order) {
	tree_tr_t* tr = malloc(sizeof(tree_tr_t));
	tr->tree = tree;
	tr->order = order;
	tr->node = 0;
	tr->stack = list_new();
	tree_tr_next(tr);
	return tr;
}

void tree_tr_destroy(tree_tr_t* tr) {
	while(list_length(tr->stack)) {
		void* stack_item;
		list_remove(tr->stack, 0, &stack_item);
		free(stack_item);
	}

	list_destroy(tr->stack);
	free(tr);
}

typedef struct {
	int state;
	node_t* node;
} tree_tr_stack_item;

void** tree_tr_next(tree_tr_t* tr) {
	int state;

	if(tr->node == 0) {
		tr->node = tr->tree->nodes;
		state = 0;
	} else {
		state = 2 * (int) tr->order + 1;
	}

	while(state != 2 * (int) tr->order) {
		if(state == 1) {
			if(tr->node->left != 0) {
				tree_tr_stack_item* stack_item = malloc(sizeof(tree_tr_stack_item));
				stack_item->state = 2;
				stack_item->node = tr->node;
				list_append(tr->stack, stack_item);
				tr->node = tr->node->left;
				state = 0;
			} else {
				state++;
			}
		} else if(state == 3) {
			if(tr->node->right != 0) {
				tree_tr_stack_item* stack_item = malloc(sizeof(tree_tr_stack_item));
				stack_item->state = 4;
				stack_item->node = tr->node;
				list_append(tr->stack, stack_item);
				tr->node = tr->node->right;
				state = 0;
			} else {
				state++;
			}
		} else if(state == 5) {
			if(list_length(tr->stack) == 0) {
				tr->node = 0;
				break;
			}

			tree_tr_stack_item* stack_item;
			list_remove(tr->stack, list_length(tr->stack) - 1, (void**) &stack_item);
			tr->node = stack_item->node;
			state = stack_item->state;
			free(stack_item);
		} else {
			state++;
		}
	}
}

void** tree_tr_prev(tree_tr_t* tr) {
	// TODO
}

void** tree_tr_current_key(tree_tr_t* tr) {
	if(tr->node == 0) {
		return 0;
	}

	return &tr->node->key;
}

void** tree_tr_current_value(tree_tr_t* tr) {
	if(tr->node == 0) {
		return 0;
	}

	return &tr->node->value;
}

bool tree_tr_first(tree_tr_t* tr) {
	if(tr->node == 0 || tr->tree->nodes == 0) {
		return 0;
	}

	node_t* first = tr->tree->nodes;

	if(tr->order == tree_tr_in_order) {
		while(first->left != 0) {
			first = first->left;
		}
	} else if(tr->order == tree_tr_post_order) {
		while(first->left != 0 || first->right != 0) {
			first = first->left != 0 ? first->left : first->right;
		}
	}

	return tr->node == first;
}

bool tree_tr_last(tree_tr_t* tr) {
	if(tr->node == 0 || tr->tree->nodes == 0) {
		return 0;
	}

	node_t* last = tr->tree->nodes;

	if(tr->order == tree_tr_pre_order) {
		while(last->left != 0 || last->right != 0) {
			last = last->right != 0 ? last->right : last->left;
		}
	} else if(tr->order == tree_tr_in_order) {
		while(last->right != 0) {
			last = last->right;
		}
	}

	// TODO
	return tr->node == last;
}

bool tree_tr_after(tree_tr_t* tr) {
	return tr->node == 0;
}

int tree_comp_eq(void* a, void* b) {
	return 0;
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

void _tree_debug_print(node_t* node, int level, int* indent, int type) {
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
		_tree_debug_print(node->left, level - 1, indent, type);
		if(level == 0) {
			for(; *indent > 0; (*indent)--) {
				printf(" ");
			}
			if(type == 0) {
				printf(" %d%c ", *((int*) node->key), node->red ? 'R' : 'B');
			} else if(type == 1) {
				printf(" %s%c ", *((char**) node->key), node->red ? 'R' : 'B');
			}
		} else {
			if(type == 0) {
				*indent += 4;
			} else if(type == 1) {
				*indent += 4 + strlen(*((char**) node->key));
			}
		}
		_tree_debug_print(node->right, level - 1, indent, type);
	}
}

void tree_debug_print(tree_t* tree, int type) {
	int depth = tree_depth(tree);
	for(int i = 0; i <= depth; i++) {
		int indent = 0;
		_tree_debug_print(tree->nodes, i, &indent, type);
		printf("\n\n\n");
	}
}

bool _tree_debug_rb_validate(node_t* node, int* black_depth) {
	if(node == 0) {
		return true;
	}

	int l_depth = 0, r_depth = 0;

	if(node->red) {
		if(node->left != 0 && node->left->red) return false;
		if(node->right != 0 && node->right->red) return false;
	}

	if(!_tree_debug_rb_validate(node->left, &l_depth)) return false;
	if(!_tree_debug_rb_validate(node->right, &r_depth)) return false;
	if(l_depth != r_depth) return false;

	*black_depth += l_depth + (node->red ? 0 : 1);

	return true;
}

bool tree_debug_rb_validate(tree_t* tree) {
	if(tree->nodes == 0) {
		return true;
	}

	int black_depth;

	return _tree_debug_rb_validate(tree->nodes, &black_depth);
}

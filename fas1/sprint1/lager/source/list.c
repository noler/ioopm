#include "list.h"

#include <stdlib.h>

typedef struct list_element list_elem_t;

struct list_element {
	list_elem_t* prev, *next;
	void* elem;
};

struct list {
	int size;
	list_elem_t* first, *last;
};

struct list_it {
	list_t* list;
	list_elem_t* elem;
	int index;
};

list_t* list_new() {
	list_t* list = malloc(sizeof(list_t));
	list->size = 0;
	list->first = list->last = 0;
	return list;
}

void list_destroy(list_t* list) {
	list_elem_t* next_elem;
	for(list_elem_t* elem = list->first; elem != 0; elem = next_elem) {
		next_elem = elem->next;
		free(elem);
	}
	free(list);
}

int list_length(list_t* list) {
	return list->size;
}

void list_append(list_t* list, void* elem) {
	list_elem_t* new_elem = malloc(sizeof(list_elem_t));

	if(list->size == 0) {
		list->first = list->last = new_elem;
		new_elem->next = new_elem->prev = 0;
		new_elem->elem = elem;
		list->size++;
	} else {
		new_elem->prev = list->last;
		new_elem->elem = elem;
		new_elem->next = 0;

		list->last->next = new_elem;
		list->last = new_elem;

		list->size++;
	}
}

void list_prepend(list_t* list, void* elem) {
	if(list->size == 0) {
		list_append(list, elem);
	} else {
		list_elem_t* new_elem = malloc(sizeof(list_elem_t));

		new_elem->prev = 0;
		new_elem->elem = elem;
		new_elem->next = list->first;

		list->first->prev = new_elem;
		list->first = new_elem;

		list->size++;
	}
}

bool list_insert(list_t* list, int index, void* elem) {
	if(index < 0) return false;
	if(index > list->size) return false;

	if(index == 0) {
		list_prepend(list, elem);
		return true;
	}

	if(index == list->size) {
		list_append(list, elem);
		return true;
	}

	index--;
	for(list_elem_t* before = list->first;; before = before->next, index--) {
		if(index == 0) {
			list_elem_t* new_elem = malloc(sizeof(list_elem_t));
			new_elem->prev = before;
			new_elem->next = before->next;
			new_elem->elem = elem;

			before->next->prev = new_elem;
			before->next = new_elem;

			list->size++;
			return true;
		}
	}
}

bool list_remove(list_t* list, int index, void** elem) {
	if(index < 0 || index >= list->size)
		return false;

	if(index == 0) {
		list_elem_t* removed = list->first;

		list->first = removed->next;

		if(list->first != 0)
			list->first->prev = 0;

		if(elem != 0)
			*elem = removed->elem;

		free(removed);
		list->size--;

		return true;
	} else if(index == list->size - 1) {
		list_elem_t* removed = list->last;

		list->last = removed->prev;

		if(list->last != 0)
			list->last->next = 0;

		if(elem != 0)
			*elem = removed->elem;

		free(removed);
		list->size--;

		return true;
	}

	for(list_elem_t* list_elem = list->first;; list_elem = list_elem->next, index--) {
		if(index == 0) {
			list_elem_t* next = list_elem->next;
			list_elem_t* prev = list_elem->prev;

			next->prev = prev;
			prev->next = next;

			list->size--;

			if(elem != 0)
				*elem = list_elem->elem;

			free(list_elem);
			return true;
		}
	}
}

void** list_get(list_t* list, int index) {
	if(list->size == 0)
		return 0;

	if(index == 0) {
		return &list->first->elem;
	} else if(index > 0) {
		index %= list->size;

		for(list_elem_t* elem = list->first;; elem = elem->next, index--) {
			if(index == 0)
				return &elem->elem;
		}
	} else {
		index = (1 - index) % list->size;

		for(list_elem_t* elem = list->last;; elem = elem->prev, index--) {
			if(index == 0)
				return &elem->elem;
		}
	}
}

void* list_first(list_t* list) {
	return &list->first->elem;
}

void* list_last(list_t* list) {
	return &list->last->elem;
}

list_it_t* list_it_new(list_t* list) {
	list_it_t* it = malloc(sizeof(list_it_t));
	it->list = list;
	it->elem = list->first;
	it->index = it->elem == 0 ? -1 : 0;
	return it;
}

void list_it_destroy(list_it_t* it) {
	free(it);
}

bool list_it_first(list_it_t* it) {
	return it->index == 0;
}

bool list_it_last(list_it_t* it) {
	return it->index == it->list->size - 1;
}

void** list_it_current(list_it_t* it) {
	if(it->elem == 0) {
		return 0;
	}

	return &it->elem->elem;
}

void** list_it_next(list_it_t* it) {
	if(it->elem == 0) {
		return 0;
	}

	void** result = &it->elem->elem;
	it->elem = it->elem->next;
	it->index++;
	if(it->index >= it->list->size) {
		it->index -= it->list->size;
	}
	return result;
}

void** list_it_prev(list_it_t* it) {
	if(it->elem == 0) {
		return 0;
	}

	void** result = &it->elem->elem;
	it->elem = it->elem->prev;
	it->index--;
	if(it->index < 0) {
		it->index = it->list->size - it->index;
	}
	return result;
}

void list_it_seek(list_it_t* it, int index) {
	if(it->list->first == 0) {
		it->elem = 0;
		it->index = -1;
	} else {
		it->elem = it->list->first;
		for(it->index = 0; it->index < index; it->index++) {
			it->elem = it->elem->next;
		}
	}
}

int list_it_index(list_it_t* it) {
	return it->index;
}

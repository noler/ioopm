#include "db.h"

#include "list.h"
#include "tree.h"
#include "utils.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

extern char* strdup(const char*);

struct db {
	list_t* item_list;
	tree_t* item_tree;
	tree_t* shelf_tree;
};

struct db_item {
	char* name, *desc;
	int price;
	list_t* shelves;
};

struct db_shelf {
	char* name;
	int amount;
};

bool db_check_shelf(char* str) {
	if(!('A' <= *str && *str <= 'Z') &&
	   !('a' <= *str && *str <= 'z'))
		return false;

	return is_number(++str);
}

db_t* db_new() {
	db_t* db = (db_t*) malloc(sizeof(db_t));
	db->item_list = list_new();
	db->item_tree = tree_new();
	db->shelf_tree = tree_new();
	return db;
}

void db_destroy(db_t* db) {
	list_destroy(db->item_list);
	tree_destroy(db->item_tree);
	tree_destroy(db->shelf_tree);
	free(db);
}

/*db_t* db_read(FILE* file) {
	db_t* db = db_new();

	int i;
	char* name = 0, *desc = 0;
	int price;
	char* shelf = 0;
	int amount;
	while(1) {
		if(fread(&i, sizeof(int), 1, file) != 1) break;
		name = malloc((i + 1) * sizeof(char));
		if(fread(name, sizeof(char), i + 1, file) != i + 1) break;
		if(fread(&i, sizeof(int), 1, file) != 1) break;
		desc = malloc((i + 1) * sizeof(char));
		if(fread(desc, sizeof(char), i + 1, file) != i + 1) break;
		if(fread(&price, sizeof(int), 1, file) != 1) break;
		if(fread(&i, sizeof(int), 1, file) != 1) break;
		shelf = malloc((i + 1) * sizeof(char));
		if(fread(shelf, sizeof(char), i + 1, file) != i + 1) break;
		if(fread(&amount, sizeof(int), 1, file) != 1) break;

		item_t* item = db_item_new(name, desc, price, shelf, amount);
		db_add_item(db, item);

		free(name);
		free(desc);
		free(shelf);

		name = 0;
		desc = 0;
		shelf = 0;
	}

	free(name);
	free(desc);
	free(shelf);

	return db;
}

void db_write(FILE* file, db_t* db) {
	list_it_t* it = list_it_new(db->list);

	while(1) {
		item_t** item = (item_t**) list_it_next(it);

		if(item == 0) {
			break;
		}

		int i = strlen((*item)->name);
		fwrite(&i, sizeof(int), 1, file);
		fwrite((*item)->name, sizeof(char), i + 1, file);
		i = strlen((*item)->desc);
		fwrite(&i, sizeof(int), 1, file);
		fwrite((*item)->desc, sizeof(char), i + 1, file);
		fwrite(&(*item)->price, sizeof(int), 1, file);
		i = strlen((*item)->shelf);
		fwrite(&i, sizeof(int), 1, file);
		fwrite((*item)->shelf, sizeof(char), i + 1, file);
		fwrite(&(*item)->amount, sizeof(int), 1, file);
	}

	list_it_destroy(it);
}*/

int db_num_items(db_t* db) {
	return list_length(db->item_list);
}

void db_list(db_t* db) {
	list_it_t* it = list_it_new(db->item_list);
	for(int i = 0;; i++) {
		db_item_t** item = (db_item_t**) list_it_next(it);

		if(item == 0) {
			break;
		}

		printf("%d. %s\n", i + 1, (*item)->name);
	}
	list_it_destroy(it);

	puts("");
}

void db_add_item(db_t* db, db_item_t* item) {
	db_insert_item(db, item, db_num_items(db));
}

void db_insert_item(db_t* db, db_item_t* item, int index) {
	list_insert(db->item_list, index, item);
	tree_insert(db->item_tree, &item->name, item, tree_comp_strp);

	list_it_t* it = list_it_new(item->shelves);
	for(;;) {
		db_shelf_t** shelf = (db_shelf_t**) list_it_next(it);

		if(shelf == 0) {
			break;
		}

		db_item_t* shelf_item = tree_insert(db->shelf_tree, (*shelf)->name, item, tree_comp_str);

		if(shelf_item != 0) {
			int shelf_index = 0;

			list_it_t* it = list_it_new(shelf_item->shelves);
			for(int i = 0;; i++) {
				db_shelf_t** shelf2 = (db_shelf_t**) list_it_next(it);

				if(shelf2 == 0) {
					break;
				}

				if(strcmp((*shelf)->name, (*shelf2)->name) == 0) {
					shelf_index = i;
					break;
				}
			}
			list_it_destroy(it);

			db_shelf_t* shelf_remove;
			list_remove(shelf_item->shelves, shelf_index, (void**) &shelf_remove);
			db_shelf_destroy(shelf_remove);
		}
	}
	list_it_destroy(it);
}

db_item_t* db_replace_item(db_t* db, db_item_t* item, int index) {
	db_item_t* old_item = db_remove_item(db, index);
	db_insert_item(db, item, index);
	return old_item;
}

db_item_t* db_get_item(db_t* db, int index) {
	if(index < 0) return 0;
	if(index >= list_length(db->item_list)) return 0;

	return (db_item_t*) *list_get(db->item_list, index);
}

db_item_t* db_find_item_name(db_t* db, char* name) {
	return (db_item_t*) tree_search(db->item_tree, &name, tree_comp_strp);
}

db_item_t* db_find_item_shelf(db_t* db, char* name) {
	void** result = tree_search(db->shelf_tree, name, tree_comp_str);

	if(result == 0) {
		return 0;
	} else {
		return *(db_item_t**) result;
	}
}

db_item_t* db_remove_item(db_t* db, int index) {
	if(index < 0) return 0;
	if(index >= list_length(db->item_list)) return 0;

	db_item_t* item;
	list_remove(db->item_list, index, (void**) &item);
	char** name = &item->name;
	tree_remove(db->item_tree, (void**) &name, tree_comp_strp);

	list_it_t* it = list_it_new(item->shelves);
	for(;;) {
		db_shelf_t** shelf = (db_shelf_t**) list_it_next(it);

		if(shelf == 0) {
			break;
		}

		char* name = (*shelf)->name;
		tree_remove(db->shelf_tree, (void**) &name, tree_comp_str);
	}
	list_it_destroy(it);

	return item;
}

db_item_t* db_item_new(char* name, char* desc, int price, list_t* shelves) {
	db_item_t* item = (db_item_t*) malloc(sizeof(db_item_t));

	item->name = strdup(name);
	item->desc = strdup(desc);
	item->price = price;
	item->shelves = list_new();
	list_it_t* it = list_it_new(shelves);
	for(;;) {
		db_shelf_t** shelf = (db_shelf_t**) list_it_next(it);

		if(shelf == 0) {
			break;
		}

		list_append(item->shelves, db_shelf_copy(*shelf));
	}
	list_it_destroy(it);

	return item;
}

db_item_t* db_item_copy(db_item_t* item) {
	return db_item_new(
		item->name,
		item->desc,
		item->price,
		item->shelves);
}

db_item_t* db_item_input() {
	char* name = ask_question_string("Namn: ");
	char* desc = ask_question_string("Beskrivning: ");
	int price = ask_question_int("Pris: ");
	list_t* shelves = list_new();

	if(ask_question_bool("Vill du lägga till en hyllplats? (J/N)", "J", "N")) {
		do {
			db_shelf_t* shelf = db_shelf_input();
			list_append(shelves, shelf);

			if(!ask_question_bool("Vill du lägga till fler hyllplatser? (J/N)", "J", "N")) {
				break;
			}
		} while(1);
	}

	db_item_t* item = malloc(sizeof(db_item_t));
	item->name = name;
	item->desc = desc;
	item->price = price;
	item->shelves = shelves;

	return item;
}

void db_item_destroy(db_item_t* item) {
	free(item->name);
	free(item->desc);

	list_it_t* it = list_it_new(item->shelves);
	for(;;) {
		db_shelf_t** shelf = (db_shelf_t**) list_it_next(it);

		if(shelf == 0) {
			break;
		}

		free((*shelf)->name);
		free(*shelf);
	}
	list_it_destroy(it);
	list_destroy(item->shelves);

	free(item);
}

void db_item_print(db_item_t* item) {
	printf(
		"Namn: %s\n"
		"Beskrivning: %s\n"
		"Pris: %d\n",
		item->name,
		item->desc,
		item->price);
	list_it_t* it = list_it_new(item->shelves);
	for(;;) {
		db_shelf_t** shelf = (db_shelf_t**) list_it_next(it);

		if(shelf == 0) {
			break;
		}

		printf("Hylla: %s, Antal: %d\n", (*shelf)->name, (*shelf)->amount);
	}
	list_it_destroy(it);
}

char* db_item_name(db_item_t* item) {
	return item->name;
}

char* db_item_desc(db_item_t* item) {
	return item->desc;
}

int db_item_price(db_item_t* item) {
	return item->price;
}

list_t* db_item_shelves(db_item_t* item) {
	return item->shelves;
}

void db_item_set_name(db_item_t* item, char* name) {
	free(item->name);
	item->name = strdup(name);
}

void db_item_set_desc(db_item_t* item, char* desc) {
	free(item->desc);
	item->desc = strdup(desc);
}

void db_item_set_price(db_item_t* item, int price) {
	item->price = price;
}

db_shelf_t* db_shelf_new(char* name, int amount) {
	db_shelf_t* shelf = malloc(sizeof(db_shelf_t));
	shelf->name = strdup(name);
	shelf->amount = amount;
	return shelf;
}

db_shelf_t* db_shelf_copy(db_shelf_t* shelf) {
	return db_shelf_new(shelf->name, shelf->amount);
}

db_shelf_t* db_shelf_input() {
	char* shelf_name = ask_question("Hylla: ", db_check_shelf, (convert_func) strdup).s;
	int amount = ask_question_int("Antal: ");
	db_shelf_t* shelf = malloc(sizeof(db_shelf_t));
	shelf->name = shelf_name;
	shelf->amount = amount;
	return shelf;

}

void db_shelf_destroy(db_shelf_t* shelf) {
	free(shelf->name);
	free(shelf);
}

char* db_shelf_name(db_shelf_t* shelf) {
	return shelf->name;
}

int db_shelf_amount(db_shelf_t* shelf) {
	return shelf->amount;
}

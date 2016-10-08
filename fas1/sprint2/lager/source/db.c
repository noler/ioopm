#include "db.h"

#include "list.h"
#include "tree.h"
#include "utils.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

extern char* strdup(const char*);

struct item {
	char* name, *desc;
	int price;
	char* shelf;
	int amount;
};

struct db {
	list_t* list;
	tree_t* tree;
};

int db_tree_comp_name(void* a, void* b) {
	item_t* item_a = (item_t*) a;
	item_t* item_b = (item_t*) b;
	return strcmp(item_a->name, item_b->name);
}

bool db_check_shelf(char* str) {
	if(!('A' <= *str && *str <= 'Z') &&
	   !('a' <= *str && *str <= 'z'))
		return false;

	return is_number(++str);
}

db_t* db_new() {
	db_t* db = (db_t*) malloc(sizeof(db_t));
	db->list = list_new();
	db->tree = tree_new();
	return db;
}

void db_destroy(db_t* db) {
	list_destroy(db->list);
	tree_destroy(db->tree);
	free(db);
}

db_t* db_read(FILE* file) {
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
}

int db_num_items(db_t* db) {
	return list_length(db->list);
}

void db_list(db_t* db) {
	int list_len = list_length(db->list);

	for(int i = 0; i < list_len; i++) {
		item_t* item = (item_t*) *list_get(db->list, i);
		printf("%d. %s\n", i + 1, item->name);
	}

	puts("");
}

void db_add_item(db_t* db, item_t* item) {
	list_append(db->list, item);
	tree_insert(db->tree, &item->name, item, tree_comp_strp);
}

void db_insert_item(db_t* db, item_t* item, int index) {
	list_insert(db->list, index, item);
	tree_insert(db->tree, &item->name, item, tree_comp_strp);
}

item_t* db_replace_item(db_t* db, item_t* item, int index) {
	item_t* old_item;
	list_remove(db->list, index, (void**) &old_item);
	tree_remove(db->tree, &old_item->name, tree_comp_strp);

	list_insert(db->list, index, item);
	tree_insert(db->tree, &item->name, item, tree_comp_strp);

	return old_item;
}

item_t* db_get_item(db_t* db, int index) {
	if(index < 0) return 0;
	if(index >= list_length(db->list)) return 0;

	return (item_t*) *list_get(db->list, index);
}

item_t* db_find_item_name(db_t* db, char* name) {
	return (item_t*) tree_search(db->tree, &name, tree_comp_strp);
}

item_t* db_find_item_shelf(db_t* db, char* name) {
	list_it_t* it = list_it_new(db->list);
	while(1) {
		item_t** item = (item_t**) list_it_next(it);

		if(item == 0) {
			break;
		}

		if(strcmp((*item)->shelf, name) == 0) {
			list_it_destroy(it);
			return *item;
		}
	}
	list_it_destroy(it);

	return 0;
}

item_t* db_remove_item(db_t* db, int index) {
	if(index < 0) return 0;
	if(index >= list_length(db->list)) return 0;

	item_t* item;
	list_remove(db->list, index, (void**) &item);
	tree_remove(db->tree, (void**) &item->name, tree_comp_strp);
	return item;
}

item_t* db_item_new(char* name, char* desc, int price, char* shelf, int amount) {
	item_t* item = (item_t*) malloc(sizeof(item_t));

	item->name = strdup(name);
	item->desc = strdup(desc);
	item->price = price;
	item->shelf = strdup(shelf);
	item->amount = amount;

	return item;
}

item_t* db_item_input() {
	char* name = ask_question_string("Namn: ");
	char* desc = ask_question_string("Beskrivning: ");
	int price = ask_question_int("Pris: ");
	char* shelf = ask_question("Hylla: ", db_check_shelf, (convert_func) strdup).s;
	int amount = ask_question_int("Antal: ");

	printf("input item\n");

	item_t* item = db_item_new(name, desc, price, shelf, amount);

	free(name);
	free(desc);
	free(shelf);

	return item;
}

void db_item_destroy(item_t* item) {
	free(item->name);
	free(item->desc);
	free(item->shelf);
	free(item);
}

void db_item_print(item_t* item) {
	printf(
		"Namn: %s\n"
		"Beskrivning: %s\n"
		"Pris: %d\n"
		"Hylla: %s\n"
		"Antal: %d\n",
		item->name,
		item->desc,
		item->price,
		item->shelf,
		item->amount);
}

char* db_item_name(item_t* item) {
	return item->name;
}

char* db_item_desc(item_t* item) {
	return item->desc;
}

int db_item_price(item_t* item) {
	return item->price;
}

char* db_item_shelf(item_t* item) {
	return item->shelf;
}

int db_item_amount(item_t* item) {
	return item->amount;
}

void db_item_set_name(item_t* item, char* name) {
	free(item->name);
	item->name = strdup(name);
}

void db_item_set_shelf(item_t* item, char* shelf) {
	free(item->shelf);
	item->shelf = strdup(shelf);
}

void db_item_copy(item_t* from, item_t* to) {
	free(to->name);
	free(to->desc);
	free(to->shelf);
	to->name = strdup(from->name);
	to->desc = strdup(from->desc);
	to->price = from->price;
	to->shelf = strdup(from->shelf);
}

void db_print_item(item_t* item) {
	puts("--- Item ---");
	printf("Name: %s\n", item->name);
	printf("Desc: %s\n", item->desc);
	printf("Price: %d\n", item->price);
	printf("Shelf: %s\n", item->shelf);
}

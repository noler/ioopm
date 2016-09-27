#include "db.h"

#include "list.h"
#include "tree.h"
#include "utils.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// exern char* strdup(const char*);

char* strdup2(const char* str) {
	int len;
	for(len = 0; str[len] != 0; len++);
	len++;

	char* new_str = malloc(len);

	for(int i = 0; i < len; i++) {
		new_str[i] = str[i];
	}

	return new_str;
}

#define strdup strdup2

struct item {
	char* name, *desc;
	int price;
	char* shelf;
};

struct db {
	list_t* list;
	tree_t* tree;
};

bool check_shelf(char* str) {
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

item_t* db_get_item(db_t* db, int index) {
	if(index < 0) return 0;
	if(index >= list_length(db->list)) return 0;

	return (item_t*) *list_get(db->list, index);
}

item_t* db_find_item(db_t* db, char* name) {
	return (item_t*) tree_search(db->tree, name, tree_comp_strp);
}

/*item_t* db_remove_item(db_t* db, int index) {
	if(index < 0) return 0;
	if(index >= list_length(db->list)) return 0;

	item_t* item;
	list_remove(db->list, index, (void**) &item);
	return item;
}*/

item_t* db_item_new(char* name, char* desc, int price, char* shelf) {
	// TODO check inputs

	item_t* item = (item_t*) malloc(sizeof(sizeof(item_t)));

	item->name = strdup(name);
	item->desc = strdup(desc);
	item->price = price;
	item->shelf = strdup(shelf);

	return item;
}

item_t* db_item_input() {
	char* name = ask_question_string("Name: ");
	char* desc = ask_question_string("Desc: ");
	int price = ask_question_int("Price: ");
	char* shelf = ask_question("Shelf: ", check_shelf, (convert_func) strdup).s;

	item_t* item = db_item_new(name, desc, price, shelf);

	return item;
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

void db_item_destroy(item_t* item) {
	free(item->name);
	free(item->desc);
	free(item->shelf);
	free(item);
}

void db_print_item(item_t* item) {
	puts("--- Item ---");
	printf("Name: %s\n", item->name);
	printf("Desc: %s\n", item->desc);
	printf("Price: %d\n", item->price);
	printf("Shelf: %s\n", item->shelf);
}

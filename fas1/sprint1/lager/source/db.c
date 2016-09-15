#include "db.h"

#include "list.h"
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
};

struct db {
	list_t* list;
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
	return db;
}

void db_destroy(db_t* db) {
	free(db);
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
}

item_t* db_get_item(db_t* db, int index) {
	if(index < 0) return 0;
	if(index >= list_length(db->list)) return 0;

	return (item_t*) list_get(db->list, index);
}

item_t* db_remove_item(db_t* db, int index) {
	if(index < 0) return 0;
	if(index >= list_length(db->list)) return 0;

	item_t* item;
	list_remove(db->list, index, (void**) &item);
	return item;
}

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

void db_print_item(item_t* item) {
	puts("--- Item ---");
	printf("Name: %s\n", item->name);
	printf("Desc: %s\n", item->desc);
	printf("Price: %d\n", item->price);
	printf("Shelf: %s\n", item->shelf);
}

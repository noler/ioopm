#ifndef __DB_H__
#define __DB_H__

#include <stdbool.h>

typedef struct item item_t;

typedef struct db db_t;

db_t* db_new();
void db_destroy(db_t* db);

int db_num_items(db_t* db);

void db_list(db_t* db);

void db_add_item(db_t* db, item_t* item);

item_t* db_get_item(db_t* db, int index);
item_t* db_find_item(db_t* db, char* name);

// item_t* db_remove_item(db_t* db, int index);

item_t* db_item_new(char* name, char* desc, int price, char* shelf);
item_t* db_item_input();

void db_item_copy(item_t* from, item_t* to);

void db_item_destroy(item_t* item);

// DEBUG functions

void db_print_item(item_t*);

#endif /* __DB_H__ */

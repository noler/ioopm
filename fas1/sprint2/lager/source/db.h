#ifndef __DB_H__
#define __DB_H__

#include "list.h"

#include <stdio.h>
#include <stdbool.h>

typedef struct db db_t;
typedef struct db_item db_item_t;
typedef struct db_shelf db_shelf_t;

db_t* db_new();
void db_destroy(db_t* db);

db_t* db_read(FILE*);
void db_write(FILE*, db_t*);

int db_num_items(db_t* db);

void db_list(db_t* db);

void db_add_item(db_t* db, db_item_t* item);
void db_insert_item(db_t* db, db_item_t* item, int index);
db_item_t* db_replace_item(db_t* db, db_item_t* item, int index);

db_item_t* db_get_item(db_t* db, int index);
db_item_t* db_find_item_name(db_t* db, char* name);
db_item_t* db_find_item_shelf(db_t* db, char* shelf);

db_item_t* db_remove_item(db_t* db, int index);

db_item_t* db_item_new(char* name, char* desc, int price, list_t* shelves);
db_item_t* db_item_copy(db_item_t* item);
db_item_t* db_item_input();
void db_item_destroy(db_item_t* db_item);

void db_item_print(db_item_t*);

char* db_item_name(db_item_t*);
char* db_item_desc(db_item_t*);
int db_item_price(db_item_t*);
list_t* db_item_shelves(db_item_t*);

void db_item_set_name(db_item_t*, char* name);
void db_item_set_desc(db_item_t*, char* desc);
void db_item_set_price(db_item_t*, int price);

db_shelf_t* db_shelf_new(char* name, int amount);
db_shelf_t* db_shelf_copy(db_shelf_t* shelf);
db_shelf_t* db_shelf_input();
void db_shelf_destroy(db_shelf_t*);

char* db_shelf_name(db_shelf_t*);
int db_shelf_amount(db_shelf_t*);

#endif /* __DB_H__ */

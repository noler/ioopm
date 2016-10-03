#include "db.h"
#include "utils.h"
#include "list.h"
#include "tree.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <mcheck.h>

extern void debug_print_tree(tree_t*);

struct undo_item {
	enum {
		undo_added, undo_edited, undo_removed
	} type;
	int index;
	item_t* item;
};

void print_menu();
char get_menu_selection();
void store_db(db_t* db);
db_t* load_db();

int main(int argc, char* argv[]) {
	FILE* file;
	
	puts(
		"Välkommen till lagerhantering 1.0\n"
		"=================================\n");

	file = fopen("store.db", "rb");
	db_t* db = db_read(file);
	fclose(file);
	list_t* undo_history = list_new();

	bool loop = true;
	while(loop) {
		print_menu();
		char c = get_menu_selection();

		switch(c) {
			case 'L': {
				item_t* item = db_item_input();

				struct undo_item* undo = malloc(sizeof(struct undo_item));
				undo->type = undo_added;
				undo->index = db_num_items(db);
				undo->item = 0;
				list_append(undo_history, undo);

				db_add_item(db, item);
				puts("");
			}
			break;

			case 'T':
			{
				db_list(db);
				int index, limit = db_num_items(db);
				do {
					printf("Vilken vara ska tas bort (1-%d)\n", limit);
					index = ask_question_int(0);
				} while(!(0 < index && index <= limit));
				index--;

				struct undo_item* undo = malloc(sizeof(struct undo_item));
				undo->type = undo_removed;
				undo->index = index;
				undo->item = db_remove_item(db, index);
				list_append(undo_history, undo);

				puts("");
			}
			break;

			case 'R':
			{
				db_list(db);
				int index, limit = db_num_items(db);
				do {
					printf("Vilken vara ska ändras? (1-%d)\n", db_num_items(db));
					index = ask_question_int(0);
				} while(!(0 < index && index <= limit));
				index--;
				item_t* new_item = db_item_input();

				struct undo_item* undo = malloc(sizeof(struct undo_item));
				undo->type = undo_edited;
				undo->index = index;
				undo->item = db_replace_item(db, new_item, index);
				list_append(undo_history, undo);

				puts("");
			}
			break;

			case 'G':
			{
				if(list_length(undo_history) == 0) {
					puts("Ångra-historiken är tom. ");
				} else {
					struct undo_item* undo;
					list_remove(undo_history, list_length(undo_history) - 1, (void**) &undo);

					switch(undo->type) {
						case undo_added:
						{
							db_item_destroy(db_remove_item(db, undo->index));
						}
						break;

						case undo_removed:
						{
							db_insert_item(db, undo->item, undo->index);
						}
						break;

						case undo_edited:
						{
							db_item_destroy(db_replace_item(db, undo->item, undo->index));
						}
						break;
					}

					free(undo);
				}

				puts("");
			}
			break;

			case 'H':
				db_list(db);
				break;

			case 'A':
				loop = false;
				break;
		}
	}

	file = fopen("store.db", "wb");
	db_write(file, db);
	fclose(file);

	list_it_t* it;
	for(it = list_it_new(undo_history);; list_it_next(it)) {
		struct undo_item** undo = (struct undo_item**) list_it_current(it);

		if(undo == 0) {
			break;
		}

		if((*undo)->type == undo_removed || (*undo)->type == undo_edited) {
			db_item_destroy((*undo)->item);
		}

		free(*undo);
	}
	list_it_destroy(it);
	list_destroy(undo_history);

	while(db_num_items(db) > 0) {
		db_item_destroy(db_remove_item(db, 0));
	}

	db_destroy(db);
}

void print_menu() {
	puts(
		"[L]ägga till en vara\n"
		"[T]a bort en vara\n"
		"[R]edigera en vara\n"
		"Ån[g]ra senaste ändringen\n"
		"List [h]ela varukatalogen\n"
		"[A]vsluta\n");
}

char get_menu_selection() {
	char c;
	do {
		fputs("Vad vill du göra? ", stdout);
		fflush(stdout);
		c = toupper(getchar());
		if(c == EOF) {
			return EOF;
		}
		char c2 = getchar();
		if(c2 != '\n' && c2 != EOF) {
			c = 0;
		}
	} while(
		c != 'L' &&
		c != 'T' &&
		c != 'R' &&
		c != 'G' &&
		c != 'H' &&
		c != 'A');

	return c;
}

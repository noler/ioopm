#include "db.h"
#include "utils.h"
#include "list.h"
#include "tree.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <mcheck.h>

extern void tree_debug_print(tree_t*, int);
extern bool tree_debug_rb_validate(tree_t*);

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

struct db {
	list_t* db;
	tree_t* tree;
};

int* aint(int a) {
	int* result = malloc(sizeof(int));
	*result = a;
	return result;
}

int main() {
	tree_t* tree = tree_new();
	tree_insert(tree, aint(1), 0, tree_comp_int);
	tree_insert(tree, aint(2), 0, tree_comp_int);
	tree_insert(tree, aint(3), 0, tree_comp_int);
	tree_insert(tree, aint(4), 0, tree_comp_int);
	tree_insert(tree, aint(5), 0, tree_comp_int);

	for(tree_tr_order order = tree_tr_pre_order; order <= tree_tr_post_order; order++) {
		tree_tr_t* tr = tree_tr_new(tree, order);
		/*tree_tr_prev(tr);
		  tree_tr_prev(tr);*/
		for(; !tree_tr_after(tr); tree_tr_next(tr)) {
			printf("%d\n",  *(int*) *tree_tr_current_key(tr));
		}
		tree_tr_destroy(tr);

		printf("\n\n");
	}

	while(tree_size(tree) > 0) {
		tree_remove(tree, 0, tree_comp_eq);
	}

	tree_destroy(tree);

	return 0;
}

int main2(int argc, char* argv[]) {
	FILE* file;

	puts(
		"Välkommen till lagerhantering 1.0\n"
		"=================================\n");

	db_t* db;

	file = fopen("store.db", "rb");
	if(file != NULL) {
		db = db_read(file);
		fclose(file);
	} else {
		db = db_new();
	}
	list_t* undo_history = list_new();

	bool loop = true;
	while(loop) {
		print_menu();
		char c = get_menu_selection();

		switch(c) {
			case 'L': {
				item_t* item = db_item_input();

				item_t* existing_item;
				while((existing_item = db_find_item_shelf(db, db_item_shelf(item))) != 0) {
					printf("Varan \"%s\" finns redan på samma hyllplats, välj en ny plats: \n", db_item_name(existing_item));
					do {
						char* response = ask_question_string(0);
						if(!db_check_shelf(response)) {
							free(response);
							puts("Ange en giltig hyllplats (bokstav följt av siffror): ");
							continue;
						}
						db_item_set_shelf(item, response);
						free(response);
						break;
					} while(1);
				}

				while(db_find_item_name(db, db_item_name(item)) != 0) {
					char* response = ask_question_string("Det finns redan en vara med samma namn, välj ett nytt namn: ");
					db_item_set_name(item, response);
					free(response);
				}

				struct undo_item* undo = malloc(sizeof(struct undo_item));
				undo->type = undo_added;
				undo->index = db_num_items(db);
				undo->item = 0;
				list_append(undo_history, undo);

				db_add_item(db, item);
				printf("%s har lagts till\n\n", db_item_name(item));
			}
			break;

			case 'T':
			{
				int index, limit = db_num_items(db);
				if(limit == 0) {
					puts("Varukatalogen är tom. \n");
					break;
				} else if(limit == 1) {
					index = 0;
				} else {
					db_list(db);
					do {
						printf("Vilken vara ska tas bort (1-%d)\n", limit);
						index = ask_question_int(0);
					} while(!(0 < index && index <= limit));
					index--;
				}

				item_t* item = db_get_item(db, index);

				bool confirm;
				while(1) {
					printf("Är du säker på att du vill ta bort \"%s\"? (j/N)\n", db_item_name(item));
					char* response = ask_question_string(0);
					if((response[0] == 'J' || response[0] == 'j') && response[1] == 0) {
						confirm = true;
						free(response);
						break;
					} else if((response[0] == 'N' || response[0] == 'n') && response[1] == 0) {
						confirm = false;
						free(response);
						break;
					}
					free(response);
				}

				if(!confirm) {
					puts("");
					break;
				}

				struct undo_item* undo = malloc(sizeof(struct undo_item));
				undo->type = undo_removed;
				undo->index = index;
				undo->item = db_remove_item(db, index);
				list_append(undo_history, undo);

				printf("\"%s\" har tagits bort. \n\n", db_item_name(undo->item));
			}
			break;

			case 'R':
			{
				int index, limit = db_num_items(db);
				if(limit == 0) {
					puts("Varukatalogen är tom. \n");
					break;
				} else {
					db_list(db);
					do {
						printf("Vilken vara ska ändras? (1-%d)\n", db_num_items(db));
						index = ask_question_int(0);
					} while(!(0 < index && index <= limit));
					index--;
				}
				item_t* new_item = db_item_input();

				item_t* old_item = db_get_item(db, index);

				bool confirm;
				while(1) {
					printf("Är du säker på att du vill ersätta \"%s\" med \"%s\"? (j/N)\n", db_item_name(new_item), db_item_name(old_item));
					char* response = ask_question_string(0);
					if((response[0] == 'J' || response[0] == 'j') && response[1] == 0) {
						confirm = true;
						free(response);
						break;
					} else if((response[0] == 'N' || response[0] == 'n') && response[1] == 0) {
						confirm = false;
						free(response);
						break;
					}
					free(response);
				}

				if(!confirm) {
					puts("");
					break;
				}

				struct undo_item* undo = malloc(sizeof(struct undo_item));
				undo->type = undo_edited;
				undo->index = index;
				undo->item = db_replace_item(db, new_item, index);
				list_append(undo_history, undo);

				printf("\"%s\" har ersatts med \"%s\"\n\n", db_item_name(old_item), db_item_name(new_item));
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

			case 'H': {
				if(db_num_items(db) == 0) {
					puts("Varukatalogen är tom. \n");
				} else {
					db_list(db);
				}
			}
			break;

			case 'S':
			{
				if(db_num_items(db) == 0) {
					puts("Varukatalogen är tom. \n");
					break;
				}
				db_list(db);
				int index, limit = db_num_items(db);
				do {
					printf("Vilken vara ska skrivas ut? (1-%d)\n", db_num_items(db));
					index = ask_question_int(0);
				} while(!(0 < index && index <= limit));
				index--;

				puts("");
				db_item_print(db_get_item(db, index));
				puts("");
			}
			break;

			case 'A':
				loop = false;
				break;

			case 'D':
			{
				tree_debug_print(db->tree, 1);
				for(int i = 0; i < list_length(undo_history); i++) {
					struct undo_item* undo = (struct undo_item*) *list_get(undo_history, i);
					if(undo->item != 0) {
						printf("undo %s\n", db_item_name(undo->item));
					}
				}
			}
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

	return 0;
}

void print_menu() {
	puts(
		"[L]ägga till en vara\n"
		"[T]a bort en vara\n"
		"[R]edigera en vara\n"
		"Ån[g]ra senaste ändringen\n"
		"List [h]ela varukatalogen\n"
		"[S]kriv ut information om en vara\n"
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
		c != 'S' &&
		c != 'A' &&
		c != 'D');

	return c;
}

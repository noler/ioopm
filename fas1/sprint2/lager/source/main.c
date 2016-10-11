#include "db.h"
#include "utils.h"

#include <ctype.h>
#include <stdlib.h>

typedef struct {
	enum {
		undo_added, undo_edited, undo_removed
	} type;
	int index;
	db_item_t* item;
} undo_item;

void print_menu();
char get_menu_selection(char* choices);
void edit_item(db_t* db, list_t* undo_stack, int index);
int select_item_index(db_t* db, char* question);

int main(int argc, char* argv[]) {
	puts(
		"Välkommen till lagerhantering 1.0\n"
		"=================================\n");

	db_t* db = db_new();
	list_t* undo_stack = list_new();

	bool loop = true;
	while(loop) {
		print_menu();
		char c = get_menu_selection("LTRGHSA");

		switch(c) {
			case 'L':
			{
				db_item_t* new_item = db_item_input();

				bool shelf_exists = false;
				list_it_t* it = list_it_new(db_item_shelves(new_item));
				for(;;) {
					db_shelf_t** shelf = (db_shelf_t**) list_it_next(it);

					if(shelf == 0) {
						break;
					}

					db_item_t* shelf_item = db_find_item_shelf(db, db_shelf_name(*shelf));
					if(shelf_item != 0) {
						printf("Varan \"%s\" finns redan på hyllan %s.\n", db_item_name(shelf_item), db_shelf_name(*shelf));
						shelf_exists = true;
						break;
					}
				}
				list_it_destroy(it);

				if(ask_question_bool(
					   shelf_exists ?
					   "Vill du skriva över varan på denna/dessa hyllor? (J/N)" :
					   "Är du säker på att du vill lägga till varan? (J/N)", "J", "N")) {
					undo_item* undo = malloc(sizeof(undo_item));
					undo->type = undo_added;
					undo->index = db_num_items(db);
					list_append(undo_stack, undo);

					db_add_item(db, new_item);
				}
			}
			break;

			case 'T':
			{
				int selection = select_item_index(db, "Vilken vara ska tas bort?");
				if(selection == -1) {
					continue;
				}

				db_item_t* old_item = db_remove_item(db, selection);

				undo_item* undo = malloc(sizeof(undo_item));
				undo->type = undo_removed;
				undo->index = selection;
				undo->item = old_item;
				list_append(undo_stack, undo);
			}
			break;

			case 'R':
			{
				int selection = select_item_index(db, "Vilken vara ska ändras?");

				if(selection == -1) {
					continue;
				}

				puts("");
				edit_item(db, undo_stack, selection);
			}
			break;

			case 'G':
			{
				if(list_length(undo_stack) == 0) {
					puts("Ångra-historiken är tom. ");
				} else {
					undo_item* undo;
					list_remove(undo_stack, list_length(undo_stack) - 1, (void**) &undo);

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
			}
			break;

			case 'H':
				db_list(db);
				break;

			case 'S':
			{
				int selection = select_item_index(db, "Vilken vara ska skrivas ut?");

				if(selection == -1) {
					continue;
				}

				puts("");
				db_item_print(db_get_item(db, selection));
				puts("");
			}
			break;

			case 'A':
				loop = false;
				break;
		}
	}

	list_it_t* it = list_it_new(undo_stack);
	for(;;) {
		undo_item** undo = (undo_item**) list_it_next(it);

		if(undo == 0) {
			break;
		}

		if((*undo)->type == undo_removed ||
		   (*undo)->type == undo_edited) {
			db_item_destroy((*undo)->item);
		}

		free(*undo);
	}
	list_it_destroy(it);
	list_destroy(undo_stack);

	while(db_num_items(db) > 0) {
		db_item_destroy(db_remove_item(db, 0));
	}

	db_destroy(db);
}

void print_menu() {
	puts("[L]ägga till en vara\n"
		 "[T]a bort en vara\n"
		 "[R]edigera en vara\n"
		 "Ån[g]ra senaste ändringen\n"
		 "List [h]ela varukatalogen\n"
		 "[S]kriv ut information om en vara\n"
		 "[A]vsluta\n");
}

char get_menu_selection(char* choices) {
	char buffer[1024];
	read_string(buffer, 1024);

	if(buffer[0] != 0 && buffer[1] == 0) {
		buffer[0] = toupper(buffer[0]);
		for(char* choice = choices; *choice != 0; choice++) {
			if(toupper(*choice) == buffer[0]) {
				return buffer[0];
			}
		}
	}

	return 0;
}

void edit_item(db_t* db, list_t* undo_stack, int index) {
	db_item_t* old_item = db_get_item(db, index);
	db_item_print(old_item);
	puts("");
	db_item_t* new_item = db_item_copy(old_item);

	bool edit = true;
	while(edit) {
		puts("[N]amn | [B]eskrivning | [P]ris | [H]yllor | [S]lutför ändringarna | [A]vbryt");
		char c = get_menu_selection("NBPHSA");

		switch(c) {
			case 'N':
			{
				char* new_name = ask_question_string("Namn: ");
				db_item_set_name(new_item, new_name);
				free(new_name);
				puts("");
			}
			break;

			case 'B':
			{
				char* new_desc = ask_question_string("Beskrivning: ");
				db_item_set_desc(new_item, new_desc);
				free(new_desc);
				puts("");
			}
			break;

			case 'P':
			{
				int new_price = ask_question_int("Pris: ");
				db_item_set_price(new_item, new_price);
				puts("");
			}
			break;

			case 'H':
			{
				list_t* shelves = db_item_shelves(new_item);
				int num_shelves = list_length(shelves);
				if(num_shelves == 0) {
					puts("Varan finns inte på någon hylla. ");

					if(ask_question_bool("Vill du lägga till en hyllplats? (J/N)", "J", "N")) {
						db_shelf_t* shelf = db_shelf_input();
					}
				} else {
					puts(num_shelves == 1 ? "Varan finns på hyllan: " : "Varan finns på hyllorna: ");
					list_it_t* it = list_it_new(shelves);
					for(int i = 1;; i++) {
						db_shelf_t** shelf = (db_shelf_t**) list_it_next(it);

						if(shelf == 0) {
							break;
						}

						printf("%d. Hylla: %s, Antal: %d\n", i, db_shelf_name(*shelf), db_shelf_amount(*shelf));
					}
					list_it_destroy(it);

					char c = 0;
					while(c == 0) {
						puts("[L]ägg till | [T]a bort | Ändra [A]ntal | A[v]bryt");
						c = get_menu_selection("LTA");
					}
				}
			}
			break;

			case 'S':
			{
				if(ask_question_bool("Är du säker på att du vill skriva ändringarna till databasen? (J/N)", "J", "N")) {
					db_replace_item(db, new_item, index);

					undo_item* undo = malloc(sizeof(undo_item));
					undo->type = undo_edited;
					undo->index = index;
					undo->item = old_item;
					list_append(undo_stack, undo);
				} else {
					
				}

				return;
			}

			case 'A':
				edit = false;
				break;
		}
	}

	db_item_destroy(new_item);
}

int index_limit;
bool is_index(char* str) {
	if(is_number(str)) {
		int number = atoi(str);
		return 1 <= number && number <= index_limit;
	} else if((str[0] == 'a' || str[0] == 'A') && str[1] == 0) {
		return true;
	} else {
		return false;
	}
}

answer_t make_answer_index(char* str) {
	if((str[0] == 'a' || str[0] == 'A') && str[1] == 0) {
		return (answer_t) 0;
	} else {
		return (answer_t) atoi(str);
	}
}

int select_item_index(db_t* db, char* question) {
	// TODO limit to 20 items

	db_list(db);

	index_limit = db_num_items(db);
	int selection = ask_question(question, is_index, make_answer_index).i;
	return selection - 1;
}

#include "db.h"
#include "utils.h"
#include "list.h"
#include "tree.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <mcheck.h>

extern void debug_print_tree(tree_t*);

void print_menu();
char get_menu_selection();

int main(int argc, char* argv[]) {
	puts(
		"Välkommen till lagerhantering 1.0\n"
		"=================================\n");

	db_t* db = db_new();

	bool loop = true;
	while(loop) {
		print_menu();
		char c = get_menu_selection();

		switch(c) {
			case 'L': {
				item_t* item = db_item_input();
				db_add_item(db, item);
			}
			break;

			case 'T':
			{
				// TODO
			}
			break;

			case 'R':
			{
				db_list(db);
				printf("Vilken vara ska ändras? (1-%d)\n", db_num_items(db));
				// TODO input id
				int index = 0;
				item_t* old_item = db_get_item(db, index);
				item_t* new_item = db_item_input();
				db_item_copy(new_item, old_item);
				db_item_destroy(new_item);
			}
			break;

			case 'G':
			{
				// TODO
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

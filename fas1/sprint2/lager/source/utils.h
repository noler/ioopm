#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef union {
	bool b;
	int i;
	float f;
	char* s;
} answer_t;

typedef bool (*check_func)(char*);
typedef answer_t (*convert_func)(char*);

int read_string(char* buf, int buf_size);

bool is_number(char* str);
bool not_empty(char* str);
bool is_float(char* str);

answer_t ask_question(char* question, check_func check, convert_func convert);
int ask_question_int(char* question);
char* ask_question_string(char* question);
float ask_question_float(char* question);
bool ask_question_bool(char* question, char* true_response, char* false_response);

#endif /* __UTILS_H__ */

#include "utils.h"

#include <stdlib.h>

int read_string(char* buf, int buf_size) {
	int i;
	for(i = 0; i < buf_size - 1; i++) {
		if((buf[i] = getchar()) == '\n' || buf[i] == EOF)
			break;
	}
	buf[i] = 0;
	return i;
}

bool is_number(char* str) {
	if(*str == 0) return false;
	if(*str == '-') {
		str++;
		if(*str == 0) return false;
	}

	for(; *str != 0; str++) {
		if(!('0' <= *str && *str <= '9')) return false;
	}

	return true;
}

bool not_empty(char* str) {
	return *str != 0;
}

bool is_float(char* str) {
	if(*str == 0) return false;
	if(*str == '-') {
		str++;
		if(*str == 0) return false;
	}

	bool point = false;

	for(; *str != 0; str++) {
		if(*str == '.') {
			if(point) return false;
			point = true;
			continue;
		}

		if(!('0' <= *str && *str <= '9')) return false;
	}

	return true;
}

answer_t make_answer_str(char* str) {
	return (answer_t) strdup(str);
}

answer_t make_answer_float(char* str) {
	bool negative = false;

	if(*str == '-') {
		negative = true;
		str++;
	}

	float value = 0;

	for(; *str != 0; str++) {
		if(*str == '.') {
			str++;

			float digitFactor = 0.1;
			for(; *str != 0; str++) {
				value += (*str - '0') * digitFactor;
				digitFactor *= 0.1;
			}

			break;
		}

		value *= 10;
		value += *str - '0';
	}

	if(negative)
		value = -value;

	return (answer_t) value;
}

answer_t ask_question(char* question, check_func check, convert_func convert) {
	char buffer[512];

	do {
		puts(question);
		read_string(buffer, 512);
	} while(!check(buffer));

	return convert(buffer);
}

int ask_question_int(char* question) {
	answer_t result = ask_question(question, is_number, (convert_func) atoi);
	return result.i;
}

char* ask_question_string(char* question) {
	answer_t result = ask_question(question, not_empty, make_answer_str);
	return result.s;
}

float ask_question_float(char* question) {
	answer_t result = ask_question(question, is_float, make_answer_float);
	return result.f;
}

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string.h>
#include <stdlib.h>

#include "commands.h"
#include "message.h"
#include "node.h"

#define MAX_QUERY_LENGTH 512
#define MAX_COMMAND_LENGTH 128

void replace_char(char *str, char old_char, char new_char);

int parse_arguments(char *query, message_t *message);

void print_message(message_t message);

void print_answer(answer_t *answer);

#endif // STRING_UTILS_H
#include <stdio.h>
#include <stdio.h>

#include "string_utils.h"
#include "commands.h"
#include "node.h"

void replace_char(char *str, char old_char, char new_char) 
{
    for (int i = 0; i < strlen(str); i++) 
    {
        if (str[i] == old_char)
        {
            str[i] = new_char;
        }
    }
}

int check_node_number(message_t *message)
{
    if (message->target < 0 || message->target > MAX_NODES)
    {
        printf("invalid node number\n");
        return -1;
    }
    return 0;
}

int parse_arguments(char *query, message_t *message)
{
	char *token;
    char *string_ptr = &query[0];
    while ((token = strsep(&string_ptr, " ")) != NULL )
    {
        replace_char(token, '\n', '\0');  
        if (strcmp(token, "create") == 0)
        {
            message->command = create;
            token = strsep(&string_ptr, " ");
            message->target = atoi(token);
        }
        else if (strcmp(token, "remove") == 0)
        {
            message->command = delete;
            token = strsep(&string_ptr, " ");
            message->target = atoi(token);
        }
        else if (strcmp(token, "exec") == 0)
        {
            message->command = exec;
            token = strsep(&string_ptr, " ");
            message->target = atoi(token);   

            token = strsep(&string_ptr, " ");
            replace_char(token, '\n', '\0');  

            if (strcmp(token, "start") == 0)
            {
                message->subcommand = start;
            }
            else if (strcmp(token, "stop") == 0)
            {
                message->subcommand = stop;
            }
            else if (strcmp(token, "time") == 0)
            {
                message->subcommand = show_time;
            }
            else 
            {
                printf("No such subcommand [%s]\n", token);
                return -1;
            }
        }
        else 
        {
            printf("No such command [%s]\n", token);
            return -1;
        }
    }
    if (check_node_number(message) != 0)
    {
        return -1;
    }
    return 0;
}


void print_answer(answer_t *answer)
{
    switch (answer->status)
    {
    case created:
        printf("OK: (%d)[%d]\n", answer->target, answer->pid);
        break;

    case already_exists:
        printf("Error: (%d) Already exists\n", answer->target);
        break;

    case deleted:
        printf("OK: (%d) removed\n", answer->target);
        break;
    
    case not_found:
        printf("Error: (%d) not found\n", answer->target);
        break;
    
    case unavailable:
        printf("node[%d] unavailable\n", answer->target);
        break;

    case creation_error:
        printf("Error: [%d] creation error\n", answer->target);
        break;

    case started_timer:
        printf("Started: (%d)\n", answer->target);
        break;

    case not_started:
        printf("Error: (%d) not started\n", answer->target);
        break;

    case already_started:
        printf("Error: (%d) already started\n", answer->target);
        break;

    case showed_time:
        printf("OK: local time is %d\n", answer->calculated_time);
        break;

    case stopped_timer:
        printf("Stopped timer (%d)\n", answer->target);
        break;

    default:
        printf("unknown answer (%d)\n", answer->status);
        break;
    }
}
/*

HANDLE USER INPUT

*/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <zmq.h>
#include <sys/types.h>
#include <signal.h>

#include "message.h"
#include "string_utils.h"
#include "zmq_tools.h"


pid_t create_root(node_t *root, node_id_t root_number)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        printf("Error while creating root process\n");
        return -1;
    }
    else if (pid == 0)
    {
        char parent_str[5];
        char node_str[5];
        sprintf(node_str, "%d", root_number);        
        sprintf(parent_str, "%d", -1);
        char *args[] = {"node", node_str, parent_str, NULL};
        execv("node", args);
    }
    else 
    {
        root->id = root_number;
        root->pid = pid;
        printf("root created\n");
    }
}


int main(int argc, char **argv)
{
    void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    char *address = create_address(5555, TCP_LOCALHOST_PATTERN);

    zmq_connect(requester, address);

    char query[MAX_QUERY_LENGTH];
    command_t command;  
    node_id_t node_number;
    subcommand_t subcommand;

    node_t root;    
    bool root_created = false;

    while (fgets(query, MAX_QUERY_LENGTH, stdin) != NULL)
    {
        message_t message;
        answer_t answer;  
        if (parse_arguments(query, &message) != 0)
        {
            continue;
        }
        if (message.command == create && !root_created)
        {
            create_root(&root, message.target);
            root_created = true;
            continue;
        }
        if (root_created)
        {
            zmq_send(requester, &message, sizeof(message), 0);
            zmq_recv(requester, &answer, sizeof(answer), 0);
            
            if (answer.target == root.id && answer.status == deleted)
            {
                root_created = false;
            }
            print_answer(&answer);
        }
        else
        {
            printf("Error: root not created\n");
        }
    }

    zmq_close(requester);
    zmq_ctx_destroy(context);

    return 0;
}
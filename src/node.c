/*

WORKING NODE

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <zmq.h>       
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#include "message.h"
#include "commands.h"
#include "node.h"
#include "zmq_tools.h"
#include "string_utils.h"


#define TIMER_INTERVEL_MS 100

// Timer variables
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static bool timer_running = false;
static int timer_time = 0;


void *timer_thread(void *arg)
 {
    struct timespec ts = {0, TIMER_INTERVEL_MS * 1000000L};
    struct timespec rem;

    while (timer_running) 
    {
        nanosleep(&ts, &rem);
        pthread_mutex_lock(&mutex);
        timer_time += TIMER_INTERVEL_MS;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

pid_t create_child(node_id_t child_id, node_id_t parent_id)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        printf("Error while creating root process\n");
        return -1;
    }
    else if (pid == 0)
    {
        char parent_str[10];
        char node_str[10];
        sprintf(node_str, "%d", child_id);        
        sprintf(parent_str, "%d", parent_id);
        fflush(stdout);
        char *args[] = {"node", node_str, parent_str, NULL};
        execv("node", args);
    }
    return pid;
}


// Args = {node ID, parent ID}
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("not enough arguments\n");
        return -1;
    }
    // timer
    pthread_t thread_id;

    node_id_t current_id = atoi(argv[1]);
    node_id_t parent_id = atoi(argv[2]);    
    
    node_t left_child;
    node_t right_child;
    bool left_child_created = false;
    bool right_child_created = false;

    int parent_port = BASE_PORT + parent_id * 2;
    if (current_id > parent_id)
    {
        parent_port += 1;
    }

    // Parent is IO nod (creating root node)
    if (parent_id == -1)
    {
        parent_port = BASE_PORT;
    }
    char *parent_address = create_address(parent_port, TCP_PATTERN);
    
    int left_port =     BASE_PORT + current_id * 2;
    int right_port =    BASE_PORT + current_id * 2 + 1;

    char *left_req_address = create_address(left_port, TCP_LOCALHOST_PATTERN);
    char *right_req_address = create_address(right_port, TCP_LOCALHOST_PATTERN);

    void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP);

    int rc = zmq_bind(responder, parent_address);
    assert(rc == 0);

    void *left_child_requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(left_child_requester, left_req_address);

    void *right_child_requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(right_child_requester, right_req_address);

    message_t message;
    answer_t answer;

    bool awake = true;
    while (awake)
    {
        zmq_recv(responder, &message, sizeof(message), 0);
        switch (message.command)
        {
        case create:

            if (message.target == current_id)
            {
                answer.target = current_id;
                answer.status = already_exists;
            }
            else if (message.target < current_id)
            {
                if (left_child_created)
                {
                    zmq_send(left_child_requester, &message, sizeof(message), 0);
                    zmq_recv(left_child_requester, &answer, sizeof(answer), 0);
                }
                else
                {
                    pid_t pid = create_child(message.target, current_id);
                    sleep(1);
                    
                    left_child_created = ping(left_port);
                    if (left_child_created) 
                    {
                        left_child.id = message.target;
                        left_child.pid = pid;
                        answer.status = created;
                        answer.pid = pid;
                    }
                    else
                    {
                        answer.status = creation_error;
                    }
                    answer.target = message.target;
                }
            }
            else
            {
                if (right_child_created)
                {
                    zmq_send(right_child_requester, &message, sizeof(message), 0);
                    zmq_recv(right_child_requester, &answer, sizeof(answer), 0);
                }
                else
                {
                    pid_t pid = create_child(message.target, current_id);
                    sleep(1);

                    right_child_created = ping(right_port);
                    if (right_child_created) 
                    {
                        right_child.id = message.target;
                        right_child.pid = pid;
                        answer.status = created;
                        answer.pid = pid;
                    }
                    else
                    {
                        answer.status = creation_error;
                    }
                    answer.target = message.target;
                }
            }
            
            break;
        
        case delete:    
            
            if (current_id == message.target)
            {
                answer.target = message.target;
                answer.status = deleted;

                zmq_send(responder, &answer, sizeof(answer), 0);
                zmq_close(responder);
                zmq_close(right_child_requester);
                zmq_close(left_child_requester);
                zmq_ctx_destroy(context);
                kill(getpid(), SIGKILL);
            }
            else if ((message.target < current_id) && left_child_created)
            {
                zmq_send(left_child_requester, &message, sizeof(message), 0);
                zmq_recv(left_child_requester, &answer, sizeof(answer), 0);
                if (answer.target == left_child.id)
                {
                    left_child_created = false;
                }

            }
            else if ((message.target > current_id) && right_child_created)
            {
                zmq_send(right_child_requester, &message, sizeof(message), 0);
                zmq_recv(right_child_requester, &answer, sizeof(answer), 0);

                if (answer.target == right_child.id)
                {
                    right_child_created = false;
                }
            }
            else
            {
                answer.target = message.target;
                answer.status = not_found;
            }
            break;

        case exec:

            answer.target = message.target;

            if (message.target == current_id)
            {
                if (message.subcommand == start)
                {
                    if (timer_running)
                    {
                        answer.status = already_started;
                    }
                    else
                    {
                        timer_running = true;
                        int status = pthread_create(&thread_id, NULL, timer_thread, NULL);
                        if (status) 
                        {
                            printf("Error creating thread: %d\n", status);
                            fflush(stdout);
                            return 1;
                        }
                        answer.status = started_timer;
                    }
                }
                else if (message.subcommand == stop)
                {
                    if (timer_running)
                    {
                        pthread_mutex_lock(&mutex);
                        timer_running = false;
                        pthread_mutex_unlock(&mutex);
                        pthread_join(thread_id, NULL);
                        answer.status = stopped_timer;
                    }
                    else 
                    {
                        answer.status = not_started;
                    }
                }
                else if (message.subcommand == show_time)
                {
                    answer.calculated_time = timer_time;
                    answer.status = showed_time;
                }
            }
            else if (message.target < current_id && left_child_created)
            {
                zmq_send(left_child_requester, &message, sizeof(message), 0);
                zmq_recv(left_child_requester, &answer, sizeof(answer), 0);
            }
            else if (message.target > current_id && right_child_created)
            {
                zmq_send(right_child_requester, &message, sizeof(message), 0);
                zmq_recv(right_child_requester, &answer, sizeof(answer), 0);
            }       
            else
            {
                answer.status = not_found;
            }

            break;

        default:
            break;
        }

        zmq_send(responder, &answer, sizeof(answer), 0);
    }

    return 0;
}
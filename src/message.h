#ifndef MESSAGE_H
#define MESSAGE_H

#include "commands.h"
#include "node.h"

typedef struct {
    command_t       command;
    node_id_t       target;
    subcommand_t    subcommand;
} message_t;

typedef enum {
    created,
    creation_error,
    already_exists,
    deleted,
    delete_error,
    not_found,
    unavailable,
    started_timer,
    already_started,
    not_started,
    calculated,
    stopped_timer,
    showed_time,
} status_t;

typedef struct {
    status_t status;
    node_id_t target;
    pid_t pid;
    time_t calculated_time;
} answer_t;

#endif // MESSAGE_H

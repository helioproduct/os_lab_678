#ifndef COMMANDS_H
#define COMMANDS_H

// commands constants
#define MAX_QUERY_LENGTH 512
#define MAX_COMMAND_LENGTH 128

typedef enum {  
    create,
    delete,
    exec,
    heartbeat,
} command_t;

// Timer
typedef enum {
    start,
    stop,
    show_time,
} subcommand_t;

#endif // COMMANDS_H
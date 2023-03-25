#ifndef ZMQ_TOOLS_H
#define ZMQ_TOOLS_H

#include <zmq.h>
#include <stdbool.h>
#include <sys/types.h>

#include "node.h"

#define ADDRESS_MAX_LEN         64
#define PORT_MAX_LEN            10 

#define TCP_PATTERN             "tcp://*:"
#define TCP_LOCALHOST_PATTERN   "tcp://localhost:"
#define PING_PATTERN            "inproc://ping"

#define BASE_PORT               5555

typedef char* pattern_t;

int get_port(node_id_t node_number);

char* create_address(unsigned int port, pattern_t pattern);

bool ping(int id);


#endif // ZMQ_TOOLS_H
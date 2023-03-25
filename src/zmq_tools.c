#include <sys/types.h>
#include <string.h>
#include <zmq.h>
#include <stdlib.h>
#include <unistd.h>
#include "zmq_tools.h"

int get_port(node_id_t node_number)
{
    return 5555 + node_number;
    // return MIN_PORT + node_number;
}

char *create_address(unsigned int port, pattern_t pattern)
{
    char *address = malloc(sizeof(char) * ADDRESS_MAX_LEN);
    memset(address, 0, ADDRESS_MAX_LEN); 
    memcpy(address, pattern, strlen(pattern));
    char *port_str = malloc(sizeof(char) * PORT_MAX_LEN);
    sprintf(port_str, "%d", port);
    strcat(address, port_str);
    return address;
}

bool ping(int port) 
{
    char *monitor_addr =  create_address(port, PING_PATTERN);       
    char *connection_addr = create_address(port, TCP_LOCALHOST_PATTERN);

    void* context = zmq_ctx_new();
    void* requester = zmq_socket(context, ZMQ_REQ);
    void* socket = zmq_socket(context, ZMQ_PAIR);
    
    int events = ZMQ_EVENT_CONNECTED | ZMQ_EVENT_CONNECT_RETRIED;

    zmq_socket_monitor(requester, monitor_addr, events);
    
    zmq_connect(socket, monitor_addr);
    zmq_connect(requester, connection_addr);

    zmq_msg_t msg;
    zmq_msg_init(&msg);
    zmq_msg_recv(&msg, socket, 0);
    uint8_t* data = (uint8_t*)zmq_msg_data(&msg);
    uint16_t event = *(uint16_t*)(data);                                        

    zmq_close(requester);
    zmq_close(socket);
    zmq_msg_close(&msg);
    zmq_ctx_destroy(context);

    if (event == ZMQ_EVENT_CONNECT_RETRIED) 
    {
        return false;
    }
    return true;
}

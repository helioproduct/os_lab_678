#include <stdio.h>
#include <stdlib.h>
#include <zmq.h>


#include "zmq_tools.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("not enough arguments\n");
        return -1;
    }
    int port = atoi(argv[1]);
    bool result = ping(port);
    printf("result: %d\n", result);
    return  0;
}
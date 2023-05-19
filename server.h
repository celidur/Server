//
// Created by Gaetan on 2022-10-16.
//

#ifndef SERVER__SERVER_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stddef.h>
#include "rsa.h"

#include <string.h>
#include "List.h"
#define SERVER__SERVER_H

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define TRUE             1
#define FALSE            0
#define SERVER_PORT  12345

enum state{
    continue_state,
    close_state,
    break_state
};

struct data {
    int return_code;
    char *buffer;
    size_t size;
};
typedef struct data data;

data server(char *buffer, key_pair key);

int send_client(data* d, struct pollfd fds);

int recv_client(char** buffer, struct pollfd fds);

int config(struct pollfd* fds);

#endif //SERVER__SERVER_H


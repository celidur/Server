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

#include <string.h>
#define SERVER__SERVER_H

#endif //SERVER__SERVER_H

struct data {
    int return_code;
    char *buffer;
};
typedef struct data data;
data server(char* buffer);
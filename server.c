//
// Created by Gaetan on 2022-10-16.
//

#include "server.h"

data server(char* buffer) {
    char * buffer2 = malloc(sizeof(char) * 100);
    data   d;
    d.return_code = 0;
    d.buffer = buffer2;

    printf("  %s\n", buffer);
    if(strcmp(buffer, "exit") == 0)
    {
        memset(buffer2, 0, sizeof(*buffer2));
        strcpy(buffer2, "Server closed");
        d.return_code = 1;
    } else if (strcmp(buffer, "key") == 0)
    {
        memset(buffer2, 0, sizeof(*buffer2));
        strcpy(buffer2, "private key");
    } else
    {
        // define buffer2 with message
        memset(buffer2, 0, sizeof(*buffer2));
        strcpy(buffer2, "Hello from server");
    }
    return d;
}
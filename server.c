//
// Created by Gaetan on 2022-10-16.
//

#include "server.h"

data server(char *buffer, List *list, unsigned int *hash) {
    char *buffer2 = get_buffer(100 * sizeof(char));
    char *buffer3 = split(buffer, '|');
    data d;
    d.return_code = 0;
    d.buffer = buffer2;
    if (strlen(buffer) == 0 & strlen(buffer3) != 0) {
        memset(buffer, 0, sizeof(buffer3));
        strcpy(buffer, buffer3);
        free(buffer3);
        buffer3 = get_buffer(0);
    }
    printf(" client send : %s\n", buffer);
    if (strlen(buffer3) != 0) {
        printf(" client send : %s\n", buffer3);
    }
    free(buffer3);
    if (strcmp(buffer, "exit") == 0) {
        memset(buffer2, 0, sizeof(*buffer2));
        strcpy(buffer2, "Server closed");
        d.return_code = 1;
    } else if (strcmp(buffer, "key") == 0) {
        memset(buffer2, 0, sizeof(*buffer2));
        char *key = IntToString(*hash);
        if (key == NULL) {
            printf("Error key\n");
            exit(EXIT_FAILURE);
        }
        strcpy(buffer2, key);
        Client client = {key, 55677};
        list->append(list, &client);
        d.return_code = 2;
    } else {
        // define buffer2 with message
        memset(buffer2, 0, sizeof(*buffer2));
        strcpy(buffer2, "Hello from server");
    }

    return d;
}
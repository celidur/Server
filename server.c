//
// Created by Gaetan on 2022-10-16.
//

#include "server.h"

data server(char* buffer, key_pair key) {
    char* buffer3 = split(buffer, '|');
    char* buffer2;
    data d;
    d.return_code = 0;
    if (buffer3 != NULL && strlen(buffer) == 0 && strlen(buffer3) != 0) {
        memset(buffer, 0, sizeof(buffer3));
        strcpy(buffer, buffer3);
        free(buffer3);
        buffer3 = NULL;
    }
    printf("  client send : %s\n", buffer);
    if (buffer3 != NULL && strlen(buffer3) != 0) {
        printf("  client send : %s\n", buffer3);
    }
    if (strcmp(buffer, "exit") == 0) {
        buffer2 = get_buffer(25);
        strcpy(buffer2, "Server closed");
        d.size = strlen(buffer2);
        d.return_code = 1;
    } else if (strcmp(buffer, "key") == 0) {
        size_t size = (mpz_sizeinbase(key.pub->n, 2) + CHAR_BIT - 1) / CHAR_BIT;
        buffer2 = (char*) malloc(size);
        mpz_export(buffer2, &size, 1, 1, 0, 0, key.pub->n);
        d.size = size;
    } else {
        buffer2 = get_buffer(25);
        strcpy(buffer2, "Hello from server");
        d.size = strlen(buffer2);
    }
    d.buffer = buffer2;
    free(buffer);


//        char *key = IntToString(*hash);
//        if (key == NULL) {
//            printf("Error key\n");
//            exit(EXIT_FAILURE);
//        }
//        strcpy(buffer2, key);
//        Client client = {key, 55677};
//        list->append(list, &client);


    return d;
}


int send_client(data* d, struct pollfd fds) {
    if (d->buffer == NULL) {
        printf("  Error on read\n");
        return TRUE;
    }
    ssize_t rc = send(fds.fd, &d->size, sizeof(unsigned long long), 0);
    if (rc == SOCKET_ERROR) {
        perror("  send() failed");
        free(d->buffer);
        d->buffer = NULL;
        return TRUE;
    }
    rc = send(fds.fd, d->buffer, d->size, 0);
    free(d->buffer);
    d->buffer = NULL;
    if (rc == SOCKET_ERROR) {
        perror("  send() failed");
        return TRUE;
    }
    return FALSE;
}

int recv_client(char** buffer, struct pollfd fds) {
    unsigned long long header_size = 0;
    ssize_t rc = recv(fds.fd, &header_size, sizeof(unsigned long long), 0);
    if (rc < 0) {
        if (errno != EWOULDBLOCK) {
            perror("  recv() failed");
            return close_state;
        }
        return break_state;
    }
    if (rc == 0) {
        printf("  Connection closed\n");
        return close_state;
    }


    *buffer = get_buffer(header_size);
    rc = recv(fds.fd, *buffer, header_size, 0);
    if (rc < 0) {
        if (errno != EWOULDBLOCK) {
            perror("  recv() failed");
            return close_state;
        }
        free(*buffer);
        *buffer = NULL;
        return break_state;
    }
    if (rc == 0) {
        printf("  Connection closed\n");
        free(*buffer);
        *buffer = NULL;
        return close_state;
    }
    return continue_state;
}

int config(struct pollfd* fds) {
    int on = 1;
    struct sockaddr_in addr;

    int listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sd == INVALID_SOCKET) {
        perror("socket");
        return EXIT_FAILURE;
    }
    // Allow socket descriptor to be reuseable
    ssize_t rc = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof(on));
    if (rc == SOCKET_ERROR) {
        perror("setsockopt");
        return EXIT_FAILURE;
    }
    // set nonblocking connection for socket
    rc = ioctl(listen_sd, FIONBIO, (char*) &on);
    if (rc == SOCKET_ERROR) {
        perror("ioctl() failed");
        close(listen_sd);
        return EXIT_FAILURE;
    }
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  // * Adresse IP automatique * //
    addr.sin_family = AF_INET;                 // * Protocole familial (IP) * //
    addr.sin_port = htons(SERVER_PORT);
    rc = bind(listen_sd, (struct sockaddr*) &addr, sizeof(addr));
    if (rc == SOCKET_ERROR) {
        perror("bind() failed");
        close(listen_sd);
        return EXIT_FAILURE;
    }
    rc = listen(listen_sd, 32);
    if (rc == SOCKET_ERROR) {
        perror("listen() failed");
        close(listen_sd);
        return EXIT_FAILURE;
    }
    fds[0].fd = listen_sd;
    fds[0].events = POLLIN;
    return EXIT_SUCCESS;
}
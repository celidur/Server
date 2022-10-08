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
#define SERVER_PORT  12345
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define TRUE             1
#define FALSE            0

int main ()
{
    int    on = 1;
    long   rc;
    int    listen_sd, new_sd;
    int    end_server = FALSE, compress_array = FALSE;
    int    close_conn;
    //define the integer header_size size to contain 18446744073709551615
    unsigned long long* header_size = NULL;

    char*  buffer = NULL;
    char   buffer2[80];
    struct sockaddr_in   addr;
    int    timeout;
    struct pollfd fds[200];
    int    nfds = 1, current_size, i, j;
    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sd == INVALID_SOCKET)
    {
        perror("socket");
        return EXIT_FAILURE;
    }
    // Allow socket descriptor to be reuseable
    rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
    if (rc == SOCKET_ERROR)
    {
        perror("setsockopt");
        return EXIT_FAILURE;
    }
    // set nonblocking connection for socket
    rc = ioctl(listen_sd, FIONBIO, (char *)&on);
    if (rc == SOCKET_ERROR)
    {
        perror("ioctl() failed");
        close(listen_sd);
        return EXIT_FAILURE;
    }
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  // * Adresse IP automatique * //
    addr.sin_family = AF_INET;                 // * Protocole familial (IP) * //
    addr.sin_port = htons(SERVER_PORT);
    rc = bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr));
    if (rc == SOCKET_ERROR)
    {
        perror("bind() failed");
        close(listen_sd);
        return EXIT_FAILURE;
    }
    rc = listen(listen_sd, 32);
    if (rc == SOCKET_ERROR)
    {
        perror("listen() failed");
        close(listen_sd);
        return EXIT_FAILURE;
    }
    memset(fds, 0 , sizeof(fds));
    fds[0].fd = listen_sd;
    fds[0].events = POLLIN;

    // ************************************************************* //
    // Initialize the timeout to 3 minutes. If no                //
    // activity after 3 minutes this program will end.           //
    // timeout value is based on milliseconds.                   //
    // ************************************************************* //
    timeout = (-3 * 60 * 1000);
    do
    {
        // *********************************************************** //
        // Call poll() and wait 3 minutes for it to complete.      //
        // *********************************************************** //
        rc = poll(fds, nfds, timeout);
        if (rc == SOCKET_ERROR)
        {
            perror("  poll() failed");
            break;
        }
        // *********************************************************** /
        // * Check to see if the 3 minute time out expired.          * //
        // *********************************************************** /
        if (rc == 0)
        {
            printf("  poll() timed out.  End program.\n");
            break;
        }
        current_size = nfds;
        for (i = 0; i < current_size; i++)
        {
            // ********************************************************* /
            // * Loop through to find the descriptors that returned    * /
            // * POLLIN and determine whether it's the listening       * /
            // * or the active connection.                             * /
            // ********************************************************* /
            if (fds[i].revents == 0)
                continue;

            // ********************************************************* /
            // * If revents is not POLLIN, it's an unexpected result,  * /
            // * log and end the server.                               * /
            // ********************************************************* /
            if(fds[i].revents != POLLIN)
            {
                if(fds[i].revents == 17) // close connexion
                {
                    compress_array = TRUE;
                }
                else
                {
                    printf("  Error on socket %d: %s\n", fds[i].fd, strerror(errno));
                    end_server = TRUE;
                    break;
                }

            }
            if (fds[i].fd == listen_sd) //new connexion
            {
                printf("  Listening socket is readable\n");
                do
                {
                    new_sd = accept(listen_sd, NULL, NULL);
                    if (new_sd == INVALID_SOCKET)
                    {
                        if (errno != EWOULDBLOCK)
                        {
                            perror("  accept() failed");
                            end_server = TRUE;
                        }
                        break;
                    }
                    printf("  New incoming connection \n");
                    fds[nfds].fd = new_sd;
                    fds[nfds].events = POLLIN;
                    nfds++;
                } while (1);
            }
            else // existing connexion
            {
                //printf("  Descriptor %d is readable\n", fds[i].fd);
                close_conn = FALSE;

                do
                {
                    // reset buffer

                    //printf("  Receiving data from client");
                    if (header_size == NULL)
                    {
                        header_size = malloc(sizeof(unsigned long long));
                        if (header_size == NULL)
                        {
                            perror("malloc");
                            return EXIT_FAILURE;
                        }
                        *header_size = 0;
                        rc = recv(fds[i].fd, header_size, sizeof(unsigned long long), 0);
                        if (rc < 0)
                        {
                            if (errno != EWOULDBLOCK)
                            {
                                perror("  recv() failed");
                                close_conn = TRUE;
                            }
                            break;
                        }
                        if (rc == 0)
                        {
                            printf("  Connection closed\n");
                            close_conn = TRUE;
                            break;
                        }
                    } else {
                        buffer = (char*) malloc(*header_size);
                        rc = recv(fds[i].fd, buffer, *header_size, 0);
                        if (rc < 0)
                        {
                            if (errno != EWOULDBLOCK)
                            {
                                perror("  recv() failed");
                                close_conn = TRUE;
                            }
                            break;
                        }
                        if (rc == 0)
                        {
                            printf("  Connection closed\n");
                            close_conn = TRUE;
                            break;
                        }
                        printf("  %s\n", buffer);
                        if(strcmp(buffer, "exit") == 0)
                        {
                            memset(buffer2, 0, sizeof(buffer2));
                            strcpy(buffer2, "Server closed");
                            end_server = TRUE;
                        } else if (strcmp(buffer, "key") == 0)
                        {
                            memset(buffer2, 0, sizeof(buffer2));
                            strcpy(buffer2, "private key");
                        } else
                        {
                            // define buffer2 with message
                            memset(buffer2, 0, sizeof(buffer2));
                            strcpy(buffer2, "Hello from server");
                        }
                        free(buffer);
                        *header_size = strlen(buffer2);
                        rc = send(fds[i].fd, header_size, sizeof(unsigned long long), 0);
                        if (rc == SOCKET_ERROR)
                        {
                            perror("  send() failed");
                            close_conn = TRUE;
                            break;
                        }
                        rc = send(fds[i].fd, buffer2, strlen(buffer2), 0);
                        if (rc == SOCKET_ERROR)
                        {
                            perror("  send() failed");
                            close_conn = TRUE;
                            break;
                        }
                        free(header_size);
                        header_size = NULL;
                    }

                } while(TRUE);
                if (close_conn)
                {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    compress_array = TRUE;
                }
            }  // End of existing connection is readable
        } // End of loop through pollable descriptors
        // delete closed connexion
        if (compress_array)
        {
            compress_array = FALSE;
            for (i = 0; i < nfds; i++)
            {
                if (fds[i].fd == -1)
                {
                    for(j = i; j < nfds; j++)
                    {
                        fds[j].fd = fds[j+1].fd;
                    }
                    i--;
                    nfds--;
                }
            }
        }

    } while (end_server == FALSE); // End of serving running.
    // close all connexion
    for (i = 0; i < nfds; i++)
    {
        if(fds[i].fd >= 0)
            close(fds[i].fd);
    }
}

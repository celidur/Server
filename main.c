#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include "server.h"
#include "rsa.h"
#define SERVER_NAME "server"
#define SERVER_PASSWORD "password"
#define TIMEOUT 180000  // 3 minutes


int main ()
{
    key_pair key = generate_key(SERVER_NAME, SERVER_PASSWORD, 0);
    int end_server = FALSE, compress_array = FALSE;
    int close_conn;
    struct pollfd fds[200];
    memset(fds, 0 , sizeof(fds));

    int nfds = 1;

    int res = config(fds);
    if(res == EXIT_FAILURE)
        return EXIT_FAILURE;

    int listen_sd = fds[0].fd;

    do
    {
        // *********************************************************** //
        // Call poll() and wait 3 minutes for it to complete.      //
        // *********************************************************** //
        ssize_t rc = poll(fds, nfds, TIMEOUT);
        if (rc == SOCKET_ERROR)
        {
            perror("  poll() failed");
            break;
        }
        if (rc == 0) //timeout
        {
            printf("  poll() timed out.  End program.\n");
            break;
        }
        int current_size = nfds;
        for (int i = 0; i < current_size; i++)
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
                do
                {
                    int new_sd = accept(listen_sd, NULL, NULL);
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
                    char * buffer = NULL;

                    int state = recv_client(&buffer, fds[i]);
                    if (state == close_state)
                        close_conn = TRUE;
                    if (state != continue_state)
                        break;

                    data d = server(buffer, key);

                    if (d.return_code < 0)
                    {
                        printf("  Error on read\n");
                        close_conn = TRUE;
                        if (d.buffer != NULL) {
                            free(d.buffer);
                            d.buffer = NULL;
                        }
                        break;
                    } else if (d.return_code == 1) {
                        printf("Server closed\n");
                        close_conn = TRUE;
                        end_server = TRUE;
                    }
                    close_conn = send_client(&d, fds[i]);
                    if (close_conn)
                        break;
                } while(TRUE);
                if (close_conn)
                {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    compress_array = TRUE;
                }
            }
        }
        // delete closed connexion
        if (compress_array)
        {
            compress_array = FALSE;
            for (int i = 0; i < nfds; i++)
            {
                if (fds[i].fd == -1)
                {
                    for(int j = i; j < nfds; j++)
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
    for (int i = 0; i < nfds; i++)
    {
        if(fds[i].fd >= 0)
            close(fds[i].fd);
    }
}

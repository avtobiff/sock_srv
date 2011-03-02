/*
 * Copyright © 2011 Per Andersson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sock_srv.h"


#define TCP_BACKLOG         5


void
start_sock_srv (struct sockaddr_in srv_addr, accept_handler_t handler)
{
    int                     sock, cli_sock;
    int                     optval;
    struct sockaddr_in      cli_addr;
    pid_t                   pid;
    socklen_t               cli_len;


    /* prevent zombie processes */
    signal (SIGCHLD, SIG_IGN);

    /* create socket */
    if ( (sock = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        perror ("socket error");
        exit (EXIT_FAILURE);
    }

    /* set reuse address socket option */
    optval = 1;
    if (setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)) < 0) {
        perror ("socket option error");
        exit (EXIT_FAILURE);
    }

    /* bind socket to addr */
    if (bind (sock, (struct sockaddr *) &srv_addr, sizeof (srv_addr)) < 0) {
        perror ("bind error");
        exit (EXIT_FAILURE);
    }

    /* listen on socket */
    if (listen (sock, TCP_BACKLOG) < 0) {
        perror ("listen error");
        exit (EXIT_FAILURE);
    }

    /* forever fork on incoming connections and fire off
       supplied function pointer.
     */
    for (;;) {
        cli_len = sizeof (cli_addr);
        cli_sock = accept (sock, (struct sockaddr *) &cli_addr, &cli_len);
        if (cli_sock < 0 ) {
            perror ("accept error");
            exit (EXIT_FAILURE);
        }

        pid = fork();

        /* fork error */
        if (pid < 0) {
            perror("fork error");
            exit (EXIT_FAILURE);
        }
        /* let child handle accepted socket */
        else if (pid == 0) {
            close (sock);
            handler (cli_sock);
            exit (EXIT_SUCCESS);
        }
        /* close child socket when done */
        else {
            close (cli_sock);
        }
    }
}

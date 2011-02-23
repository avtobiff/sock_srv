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
start_sock_srv (int port, accept_handler_fp handler)
{
    int                     sock, cli_sock;
    struct sockaddr_in      srv_addr, cli_addr;
    pid_t                   pid;
    socklen_t               cli_len;


    /* prevent zombie processes */
    signal (SIGCHLD, SIG_IGN);

    /* create socket */
    if ( (sock = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        perror ("socket error");
        exit (1);
    }

    /* create addr struct */
    memset (&srv_addr, 0, sizeof (srv_addr));
    srv_addr.sin_family      = AF_INET;
    srv_addr.sin_port        = htons(port);
    srv_addr.sin_addr.s_addr = INADDR_ANY;

    /* bind socket to addr */
    if (bind (sock, (struct sockaddr *) &srv_addr, sizeof (srv_addr)) < 0) {
        perror ("bind error");
        exit (1);
    }

    /* listen on socket */
    if (listen (sock, TCP_BACKLOG) < 0) {
        perror ("listen error");
        exit (1);
    }

    /* forever fork on incoming connections and fire off
       supplied function pointer.
     */
    for (;;) {
        cli_len = sizeof (cli_addr);
        cli_sock = accept (sock, (struct sockaddr *) &cli_addr, &cli_len);
        if (cli_sock < 0 ) {
            perror ("accept error");
            exit (1);
        }

        pid = fork();

        /* fork error */
        if (pid < 0) {
            perror("fork error");
            exit (1);
        }
        /* let child handle accepted socket */
        else if (pid == 0) {
            close (sock);
            handler (cli_sock);
            exit (0);
        }
        /* close child socket when done */
        else {
            close (cli_sock);
        }
    }
}

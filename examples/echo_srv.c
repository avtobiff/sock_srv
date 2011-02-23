#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sock_srv.h"


#define PORT        8000
#define BUF_LEN     256


static void
echo (int sock)
{
    char            buf[BUF_LEN];
    ssize_t         bytes_r, bytes_w;
    pid_t           pid;


    pid = getpid();

    fprintf (stdout, "%d: echo\n", pid);

    memset (buf, 0, BUF_LEN);
    while ( (bytes_r = read (sock, buf, BUF_LEN)) ) {
        fprintf (stdout, "%d: echoing data\n", pid);

        if (bytes_r < 0) {
            perror("socket read error");
            exit (1);
        }

        bytes_w = write (sock, buf, (size_t) bytes_r);
        if (bytes_w < bytes_r) {
            perror("socket write error");
            exit (1);
        }
    }

    fprintf (stdout, "%d: exiting...\n", pid);
}


static void
signal_handler (int sig)
{
    char            c;

    signal (sig, SIG_IGN);
    fprintf (stdout, "\nquit server? [y/N] ");
    c = getchar ();
    if (c == 'y' || c == 'Y') {
        exit (0);
    }

    signal(SIGINT, signal_handler);
}


int
main (int argc, char** argv)
{
    signal (SIGINT, signal_handler);

    fprintf (stdout, "starting echo_srv on port %d...\n", PORT);
    start_sock_srv(PORT, echo);

    return 0;
}

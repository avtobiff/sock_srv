#ifndef _SOCK_SRV_H
#define _SOCK_SRV_H 1

typedef void (*accept_handler_fp) (int sock);

void
start_sock_srv (int port, accept_handler_fp handler);

#endif /* sock_srv.h */

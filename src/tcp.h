#ifndef TCP_H
#define TCP_H 1

#include <stddef.h>

struct tcp_connection
{
    char *remote_host;
    char *remote_port;
    char *buffer;
    size_t buffer_size;
    int sockfd;
};

/*
 * Establish a TCP connection
 */
int tcp_connect(struct tcp_connection *conn);

/*
 * Send bytes from a connection buffer through an established connection
 */
int tcp_send(struct tcp_connection *conn, size_t len);

/*
 * Receive bytes into a connection buffer from an established connection
 * It is an error if len bytes can not be received for any reason
 * (e.g. the connection was orderly closed by its remote peer).
 */
int tcp_receive(struct tcp_connection *conn, size_t len);

/*
 * Close write half of an established connection
 */
int tcp_close_write(struct tcp_connection *conn);

/*
 * Close an established connection
 */
int tcp_close(struct tcp_connection *conn);

#endif /* tcp.h */

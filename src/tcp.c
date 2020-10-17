#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include "log.h"
#include "tcp.h"

int tcp_connect(struct tcp_connection *conn)
{
    struct addrinfo addr_hints, *first_addr, *addr;
    int errcode;

    log_debug(__func__, "called to connect to %s:%s",
            conn->remote_host, conn->remote_port);

    memset(&addr_hints, 0, sizeof(addr_hints));
    addr_hints.ai_family = AF_UNSPEC; /* Allow both IPv4 and IPv6 */
    addr_hints.ai_socktype = SOCK_STREAM; /* Should select TCP protocol */
    addr_hints.ai_protocol = 0; /* Select protocol automatically */
    addr_hints.ai_flags = AI_ADDRCONFIG; /* Only get addresses of families
                                            configured on this host */

    log_debug(__func__,
            "resolving using family %i, socktype %i, protocol %i, flags %#08x",
            addr_hints.ai_family, addr_hints.ai_socktype,
            addr_hints.ai_protocol, addr_hints.ai_flags);

    errcode = getaddrinfo(conn->remote_host, conn->remote_port,
            &addr_hints, &first_addr);
    if (errcode != 0) {
        log_gai_error(__func__, errcode, "unable to resolve %s:%s",
                conn->remote_host, conn->remote_port);
        return -1;
    }

    for (addr = first_addr; addr != NULL; addr = addr->ai_next) {

        /* TODO: implement convertion of address from binary to string */
        char *addr_string = "[not implemented]";

        log_debug(__func__,
                "resolved into family %i, socktype %i, protocol %i, addr %s",
                addr->ai_family, addr->ai_socktype,
                addr->ai_protocol, addr_string);

        conn->sockfd = socket(addr->ai_family, addr->ai_socktype,
                addr->ai_protocol);
        if (conn->sockfd == -1) {
            log_sys_warning(__func__, "unable to create socket with family %i, socktype %i, protocol %i",
                    addr->ai_family, addr->ai_socktype, addr->ai_protocol);
            continue; /* This address failed, try next */
        }

        log_debug(__func__, "socket created (sockfd %i)", conn->sockfd);

        if (connect(conn->sockfd, addr->ai_addr, addr->ai_addrlen) == 0) {
            log_debug(__func__, "connection established");
            break; /* This address succeeded, stop trying */
        }

        log_sys_info(__func__, "address %s failed, trying next (if any)",
                addr_string);

        if (close(conn->sockfd) != 0) {
            log_sys_error(__func__, "unable to close socket (sockfd %i)",
                    conn->sockfd);
            return -1;
        }

        log_debug(__func__, "socket closed (sockfd %i)", conn->sockfd);
    }

    freeaddrinfo(first_addr);

    /* If no address succeeded */
    if (addr == NULL) {
        log_sys_error(__func__, "unable to connect to %s:%s",
                conn->remote_host, conn->remote_port);
        return -1;
    }

    log_debug(__func__, "connected to %s:%s, return",
            conn->remote_host, conn->remote_port);
    return 0;
}

int tcp_send(struct tcp_connection *conn, size_t len)
{
    size_t total = 0;
    ssize_t count;

    log_debug(__func__, "called to send %zu bytes to %s:%s",
            len, conn->remote_host, conn->remote_port);

    if (len > conn->buffer_size) {
        log_error(__func__,
                "bytes count to send (%zu) > connection buffer size (%zu)",
                len, conn->buffer_size);
        return -1;
    }

    if (len == 0) {
        log_error(__func__, "bytes count to send is 0");
        return -1;
    }

    while (total < len) {
        count = send(conn->sockfd, conn->buffer + total, len - total, 0);
        if (count < 0) {
            log_sys_error(__func__, "unable to send %zu bytes to %s:%s",
                    len - total, conn->remote_host, conn->remote_port);
            return -1;
        }

        total += count;
        log_debug(__func__, "%zi bytes sent at once (%zu in total)",
                count, total);
    }

    log_debug(__func__, "sent %zu bytes to %s:%s, return",
            total, conn->remote_host, conn->remote_port);

    return 0;
}

int tcp_receive(struct tcp_connection *conn, size_t len)
{
    ssize_t count;

    log_debug(__func__, "called to receive %zu bytes from %s:%s",
            len, conn->remote_host, conn->remote_port);

    if (len > conn->buffer_size) {
        log_error(__func__,
                "bytes count to receive (%zu) > connection buffer size (%zu)",
                len, conn->buffer_size);
        return -1;
    }

    if (len == 0) {
        log_error(__func__, "bytes count to receive is 0");
        return -1;
    }

    count = recv(conn->sockfd, conn->buffer, len, 0);
    if (count < 0) {
        log_sys_error(__func__, "unable to receive %zu bytes from %s:%s",
                len, conn->remote_host, conn->remote_port);
        return -1;
    }

    log_debug(__func__, "received %zu bytes from %s:%s, return",
            count, conn->remote_host, conn->remote_port);

    return count;
}

int tcp_close_write(struct tcp_connection *conn)
{
    log_debug(__func__, "called to close write half of connection to %s:%s",
            conn->remote_host, conn->remote_port);

    if (shutdown(conn->sockfd, SHUT_WR) != 0) {
        log_sys_error(__func__,
                "unable to close write half of connection to %s:%s",
                conn->remote_host, conn->remote_port);
        return -1;
    }

    log_debug(__func__, "closed write half of connection to %s:%s, return",
            conn->remote_host, conn->remote_port);

    return 0;
}

int tcp_close(struct tcp_connection *conn)
{
    log_debug(__func__, "called to close connection to %s:%s",
            conn->remote_host, conn->remote_port);

    if (close(conn->sockfd) != 0) {
        log_sys_error(__func__, "unable to close connection to %s:%s",
                conn->remote_host, conn->remote_port);
        return -1;
    }

    log_debug(__func__, "closed connection to %s:%s, return",
            conn->remote_host, conn->remote_port);

    return 0;
}

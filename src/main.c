#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "tcp.h"

#define BUFFER_SIZE 10240

char buffer[BUFFER_SIZE];

void printUsageAndExit()
{
    puts("Usage: babycurl <host_name> <http_port>");
    exit(1);
}

int create_http_get_request(char *host, char *buffer, size_t buffer_size)
{
    int count;

    count = snprintf(buffer, buffer_size,
            "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
    if (count >= buffer_size) {
        log_error(__func__,
                "request size (%zu) + 1 (for '\\0') > buffer size (%zu)",
                count, buffer_size);
    } else if (count < 0) {
        log_error(__func__, "unable to format request for host '%s'", host);
    }

    return count;
}

int main(int argc, char **argv)
{
    char *host, *port;
    struct tcp_connection conn;
    int count;

    if ( argc < 3 ) {
        printUsageAndExit();
    }
    host = argv[1];
    port = argv[2];

    log_set_exit_on_error(1);
    log_set_min_log_level(LL_DEBUG);

    conn.remote_host = host;
    conn.remote_port = port;
    conn.buffer = buffer;
    conn.buffer_size = BUFFER_SIZE;
    tcp_connect(&conn);

    count = create_http_get_request(host, buffer, BUFFER_SIZE);
    tcp_send(&conn, count);
    tcp_close_write(&conn);

    // HACK: just read one buffer without parsing headers (Content-Length)
    count = tcp_receive(&conn, BUFFER_SIZE);
    fwrite(conn.buffer, 1, count, stdout);

    tcp_close(&conn);
    exit(EXIT_SUCCESS);
}

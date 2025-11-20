#ifndef NET_TRANSPORT_H
#define NET_TRANSPORT_H

#include <stddef.h>

/*
 * Network Transport Layer
 * 
 * Raw network I/O abstraction.
 * Wraps NNG sockets or provides fake implementations for testing.
 * 
 * Design principles:
 * - Minimal interface (connect, send, receive, close)
 * - Timeout support
 * - Error handling
 * - Can be implemented with NNG, raw sockets, or fakes
 */

/* Forward declaration */
typedef struct net_transport net_transport_t;

/* Transport types */
typedef enum {
    NET_TRANSPORT_TCP,
    NET_TRANSPORT_IPC,
    NET_TRANSPORT_FAKE
} net_transport_type_t;

/*
 * Transport structure
 */
struct net_transport {
    net_transport_type_t type;
    void *impl_data;  /* Implementation-specific data (e.g., nng_socket) */
    int connected;
};

/*
 * Transport API
 */

/* Create transport */
net_transport_t* net_transport_create(net_transport_type_t type);

/* Connect to remote endpoint */
int net_transport_connect(net_transport_t *transport, const char *url, int timeout_ms);

/* Send data */
int net_transport_send(net_transport_t *transport, const void *data, size_t len, int timeout_ms);

/* Receive data (returns bytes received, or -1 on error) */
int net_transport_recv(net_transport_t *transport, void *buffer, size_t buffer_size, int timeout_ms);

/* Close connection */
void net_transport_close(net_transport_t *transport);

/* Destroy transport */
void net_transport_destroy(net_transport_t *transport);

/*
 * Server-side transport (for listening)
 */

typedef struct net_transport_listener net_transport_listener_t;

/* Callback for accepting connections */
typedef void (*net_transport_accept_callback_t)(net_transport_t *client, void *context);

struct net_transport_listener {
    net_transport_type_t type;
    void *impl_data;
    int listening;
};

/* Create listener */
net_transport_listener_t* net_transport_listener_create(net_transport_type_t type);

/* Start listening on URL */
int net_transport_listener_listen(net_transport_listener_t *listener, const char *url);

/* Accept connection (blocking) */
net_transport_t* net_transport_listener_accept(net_transport_listener_t *listener, int timeout_ms);

/* Stop listening */
void net_transport_listener_stop(net_transport_listener_t *listener);

/* Destroy listener */
void net_transport_listener_destroy(net_transport_listener_t *listener);

#endif /* NET_TRANSPORT_H */

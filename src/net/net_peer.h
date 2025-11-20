#ifndef NET_PEER_H
#define NET_PEER_H

#include "net_protocol.h"

/*
 * Network Peer Layer
 * 
 * High-level peer abstraction for Chord node communication.
 * Wraps transport layer and provides async RPC operations.
 * 
 * Design principles:
 * - Abstract interface that can be faked for testing
 * - Async operations with callbacks
 * - Timeout handling
 * - Connection pooling/reuse
 */

/* Forward declarations */
typedef struct net_peer net_peer_t;
typedef struct net_peer_iface net_peer_iface_t;

/* Callback for async operations */
typedef void (*net_peer_callback_t)(void *context, const net_message_t *response, int error);

/*
 * Peer interface (vtable pattern for testability)
 * 
 * This allows us to inject fake implementations for unit tests
 * while using real network implementations in production.
 */
struct net_peer_iface {
    /* Connect to a remote peer */
    int (*connect)(net_peer_t *peer, const char *url);
    
    /* Send request and receive response (synchronous for now) */
    int (*send_request)(net_peer_t *peer, const net_message_t *request, 
                        net_message_t *response, int timeout_ms);
    
    /* Send request asynchronously (future enhancement) */
    int (*send_request_async)(net_peer_t *peer, const net_message_t *request,
                              net_peer_callback_t callback, void *context, int timeout_ms);
    
    /* Close connection */
    void (*close)(net_peer_t *peer);
    
    /* Destroy peer */
    void (*destroy)(net_peer_t *peer);
};

/*
 * Peer structure
 */
struct net_peer {
    const net_peer_iface_t *iface;  /* Interface vtable */
    void *impl_data;                 /* Implementation-specific data */
    char remote_url[NET_PROTOCOL_MAX_URL];
    int connected;
};

/*
 * Peer API (calls through interface)
 */

/* Create a peer (implementation-specific) */
net_peer_t* net_peer_create(const net_peer_iface_t *iface);

/* Connect to remote peer */
int net_peer_connect(net_peer_t *peer, const char *url);

/* Send synchronous request */
int net_peer_send_request(net_peer_t *peer, const net_message_t *request,
                          net_message_t *response, int timeout_ms);

/* Send asynchronous request */
int net_peer_send_request_async(net_peer_t *peer, const net_message_t *request,
                                net_peer_callback_t callback, void *context, int timeout_ms);

/* Close connection */
void net_peer_close(net_peer_t *peer);

/* Destroy peer */
void net_peer_destroy(net_peer_t *peer);

/*
 * High-level Chord RPC helpers
 * 
 * These wrap the low-level send_request with Chord-specific logic.
 */

/* Find successor for a key */
int net_peer_find_successor(net_peer_t *peer, int key, net_node_addr_t *result, int timeout_ms);

/* Get predecessor */
int net_peer_get_predecessor(net_peer_t *peer, net_node_addr_t *result, int *has_predecessor, int timeout_ms);

/* Get successor */
int net_peer_get_successor(net_peer_t *peer, net_node_addr_t *result, int timeout_ms);

/* Notify of potential predecessor */
int net_peer_notify(net_peer_t *peer, const net_node_addr_t *node, int timeout_ms);

/* Get closest preceding node */
int net_peer_closest_preceding(net_peer_t *peer, int key, net_node_addr_t *result, int timeout_ms);

/* Ping peer */
int net_peer_ping(net_peer_t *peer, int *alive, int *state, int timeout_ms);

#endif /* NET_PEER_H */

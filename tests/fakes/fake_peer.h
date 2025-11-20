#ifndef FAKE_PEER_H
#define FAKE_PEER_H

#include "../../src/net/net_peer.h"

/*
 * Fake Peer Implementation for Unit Tests
 * 
 * Provides in-memory peer communication without actual network I/O.
 * Allows testing Chord logic in isolation.
 * 
 * Features:
 * - In-memory message passing
 * - Configurable responses
 * - Error injection for testing failure scenarios
 * - Request/response history for verification
 */

/* Maximum recorded requests */
#define FAKE_PEER_MAX_HISTORY 100

/* Fake peer implementation data */
typedef struct {
    /* Configuration */
    int inject_error;           /* Error code to inject (0 = no error) */
    int inject_timeout;         /* Simulate timeout if non-zero */
    
    /* Canned responses (for testing) */
    net_node_addr_t canned_node;
    int canned_has_node;
    int canned_alive;
    int canned_state;
    
    /* Request history (for verification) */
    net_message_t request_history[FAKE_PEER_MAX_HISTORY];
    int request_count;
    
    /* Response history */
    net_message_t response_history[FAKE_PEER_MAX_HISTORY];
    int response_count;
} fake_peer_data_t;

/*
 * Fake peer API
 */

/* Create fake peer */
net_peer_t* fake_peer_create(void);

/* Configure canned responses */
void fake_peer_set_canned_node(net_peer_t *peer, const char *id, int key, const char *url);
void fake_peer_set_canned_has_node(net_peer_t *peer, int has_node);
void fake_peer_set_canned_alive(net_peer_t *peer, int alive, int state);

/* Configure error injection */
void fake_peer_inject_error(net_peer_t *peer, int error_code);
void fake_peer_inject_timeout(net_peer_t *peer, int enable);

/* Query request history */
int fake_peer_get_request_count(net_peer_t *peer);
const net_message_t* fake_peer_get_request(net_peer_t *peer, int index);

/* Query response history */
int fake_peer_get_response_count(net_peer_t *peer);
const net_message_t* fake_peer_get_response(net_peer_t *peer, int index);

/* Reset history */
void fake_peer_reset_history(net_peer_t *peer);

/* Get fake peer interface (for creating peers) */
const net_peer_iface_t* fake_peer_get_interface(void);

#endif /* FAKE_PEER_H */

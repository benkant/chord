#include "net_peer.h"
#include <stdlib.h>
#include <string.h>

/*
 * Peer API implementation (calls through interface vtable)
 */

net_peer_t* net_peer_create(const net_peer_iface_t *iface) {
    net_peer_t *peer = (net_peer_t*)malloc(sizeof(net_peer_t));
    if (!peer) return NULL;
    
    memset(peer, 0, sizeof(net_peer_t));
    peer->iface = iface;
    peer->connected = 0;
    
    return peer;
}

int net_peer_connect(net_peer_t *peer, const char *url) {
    if (!peer || !peer->iface || !peer->iface->connect) {
        return NET_ERR_INTERNAL;
    }
    return peer->iface->connect(peer, url);
}

int net_peer_send_request(net_peer_t *peer, const net_message_t *request,
                          net_message_t *response, int timeout_ms) {
    if (!peer || !peer->iface || !peer->iface->send_request) {
        return NET_ERR_INTERNAL;
    }
    return peer->iface->send_request(peer, request, response, timeout_ms);
}

int net_peer_send_request_async(net_peer_t *peer, const net_message_t *request,
                                net_peer_callback_t callback, void *context, int timeout_ms) {
    if (!peer || !peer->iface || !peer->iface->send_request_async) {
        return NET_ERR_INTERNAL;
    }
    return peer->iface->send_request_async(peer, request, callback, context, timeout_ms);
}

void net_peer_close(net_peer_t *peer) {
    if (peer && peer->iface && peer->iface->close) {
        peer->iface->close(peer);
    }
}

void net_peer_destroy(net_peer_t *peer) {
    if (peer && peer->iface && peer->iface->destroy) {
        peer->iface->destroy(peer);
    }
}

/*
 * High-level Chord RPC helpers
 */

int net_peer_find_successor(net_peer_t *peer, int key, net_node_addr_t *result, int timeout_ms) {
    net_message_t request, response;
    
    memset(&request, 0, sizeof(net_message_t));
    request.header.version = NET_PROTOCOL_VERSION;
    request.header.msg_type = NET_MSG_FIND_SUCCESSOR;
    request.header.request_id = (uint32_t)rand();  /* Simple request ID */
    request.payload.find_successor_req.key = key;
    
    int err = net_peer_send_request(peer, &request, &response, timeout_ms);
    if (err != NET_ERR_OK) {
        return err;
    }
    
    if (response.header.msg_type != NET_MSG_FIND_SUCCESSOR_RESPONSE) {
        return NET_ERR_INVALID_MESSAGE;
    }
    
    memcpy(result, &response.payload.find_successor_resp.node, sizeof(net_node_addr_t));
    return NET_ERR_OK;
}

int net_peer_get_predecessor(net_peer_t *peer, net_node_addr_t *result, int *has_predecessor, int timeout_ms) {
    net_message_t request, response;
    
    memset(&request, 0, sizeof(net_message_t));
    request.header.version = NET_PROTOCOL_VERSION;
    request.header.msg_type = NET_MSG_GET_PREDECESSOR;
    request.header.request_id = (uint32_t)rand();
    
    int err = net_peer_send_request(peer, &request, &response, timeout_ms);
    if (err != NET_ERR_OK) {
        return err;
    }
    
    if (response.header.msg_type != NET_MSG_GET_PREDECESSOR_RESPONSE) {
        return NET_ERR_INVALID_MESSAGE;
    }
    
    *has_predecessor = response.payload.get_node_resp.has_node;
    if (*has_predecessor) {
        memcpy(result, &response.payload.get_node_resp.node, sizeof(net_node_addr_t));
    }
    
    return NET_ERR_OK;
}

int net_peer_get_successor(net_peer_t *peer, net_node_addr_t *result, int timeout_ms) {
    net_message_t request, response;
    
    memset(&request, 0, sizeof(net_message_t));
    request.header.version = NET_PROTOCOL_VERSION;
    request.header.msg_type = NET_MSG_GET_SUCCESSOR;
    request.header.request_id = (uint32_t)rand();
    
    int err = net_peer_send_request(peer, &request, &response, timeout_ms);
    if (err != NET_ERR_OK) {
        return err;
    }
    
    if (response.header.msg_type != NET_MSG_GET_SUCCESSOR_RESPONSE) {
        return NET_ERR_INVALID_MESSAGE;
    }
    
    if (!response.payload.get_node_resp.has_node) {
        return NET_ERR_NODE_NOT_FOUND;
    }
    
    memcpy(result, &response.payload.get_node_resp.node, sizeof(net_node_addr_t));
    return NET_ERR_OK;
}

int net_peer_notify(net_peer_t *peer, const net_node_addr_t *node, int timeout_ms) {
    net_message_t request, response;
    
    memset(&request, 0, sizeof(net_message_t));
    request.header.version = NET_PROTOCOL_VERSION;
    request.header.msg_type = NET_MSG_NOTIFY;
    request.header.request_id = (uint32_t)rand();
    memcpy(&request.payload.notify_req.node, node, sizeof(net_node_addr_t));
    
    int err = net_peer_send_request(peer, &request, &response, timeout_ms);
    if (err != NET_ERR_OK) {
        return err;
    }
    
    if (response.header.msg_type != NET_MSG_NOTIFY_RESPONSE) {
        return NET_ERR_INVALID_MESSAGE;
    }
    
    return response.payload.notify_resp.success ? NET_ERR_OK : NET_ERR_INTERNAL;
}

int net_peer_closest_preceding(net_peer_t *peer, int key, net_node_addr_t *result, int timeout_ms) {
    net_message_t request, response;
    
    memset(&request, 0, sizeof(net_message_t));
    request.header.version = NET_PROTOCOL_VERSION;
    request.header.msg_type = NET_MSG_CLOSEST_PRECEDING;
    request.header.request_id = (uint32_t)rand();
    request.payload.closest_preceding_req.key = key;
    
    int err = net_peer_send_request(peer, &request, &response, timeout_ms);
    if (err != NET_ERR_OK) {
        return err;
    }
    
    if (response.header.msg_type != NET_MSG_CLOSEST_PRECEDING_RESPONSE) {
        return NET_ERR_INVALID_MESSAGE;
    }
    
    memcpy(result, &response.payload.closest_preceding_resp.node, sizeof(net_node_addr_t));
    return NET_ERR_OK;
}

int net_peer_ping(net_peer_t *peer, int *alive, int *state, int timeout_ms) {
    net_message_t request, response;
    
    memset(&request, 0, sizeof(net_message_t));
    request.header.version = NET_PROTOCOL_VERSION;
    request.header.msg_type = NET_MSG_PING;
    request.header.request_id = (uint32_t)rand();
    
    int err = net_peer_send_request(peer, &request, &response, timeout_ms);
    if (err != NET_ERR_OK) {
        return err;
    }
    
    if (response.header.msg_type != NET_MSG_PING_RESPONSE) {
        return NET_ERR_INVALID_MESSAGE;
    }
    
    *alive = response.payload.ping_resp.alive;
    *state = response.payload.ping_resp.state;
    
    return NET_ERR_OK;
}

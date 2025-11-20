#include "fake_peer.h"
#include <stdlib.h>
#include <string.h>

/*
 * Fake peer implementation
 */

static int fake_peer_connect_impl(net_peer_t *peer, const char *url) {
    (void)url;
    peer->connected = 1;
    strncpy(peer->remote_url, url, NET_PROTOCOL_MAX_URL - 1);
    peer->remote_url[NET_PROTOCOL_MAX_URL - 1] = '\0';
    return 0;
}

static int fake_peer_send_request_impl(net_peer_t *peer, const net_message_t *request,
                                       net_message_t *response, int timeout_ms) {
    (void)timeout_ms;
    
    fake_peer_data_t *data = (fake_peer_data_t*)peer->impl_data;
    
    /* Record request */
    if (data->request_count < FAKE_PEER_MAX_HISTORY) {
        memcpy(&data->request_history[data->request_count], request, sizeof(net_message_t));
        data->request_count++;
    }
    
    /* Check for injected timeout */
    if (data->inject_timeout) {
        return NET_ERR_TIMEOUT;
    }
    
    /* Check for injected error */
    if (data->inject_error != 0) {
        return data->inject_error;
    }
    
    /* Generate canned response based on request type */
    memset(response, 0, sizeof(net_message_t));
    response->header.version = NET_PROTOCOL_VERSION;
    response->header.request_id = request->header.request_id;
    
    switch (request->header.msg_type) {
        case NET_MSG_FIND_SUCCESSOR:
            response->header.msg_type = NET_MSG_FIND_SUCCESSOR_RESPONSE;
            memcpy(&response->payload.find_successor_resp.node, 
                   &data->canned_node, sizeof(net_node_addr_t));
            break;
            
        case NET_MSG_GET_PREDECESSOR:
            response->header.msg_type = NET_MSG_GET_PREDECESSOR_RESPONSE;
            response->payload.get_node_resp.has_node = data->canned_has_node;
            if (data->canned_has_node) {
                memcpy(&response->payload.get_node_resp.node,
                       &data->canned_node, sizeof(net_node_addr_t));
            }
            break;
            
        case NET_MSG_GET_SUCCESSOR:
            response->header.msg_type = NET_MSG_GET_SUCCESSOR_RESPONSE;
            response->payload.get_node_resp.has_node = 1;
            memcpy(&response->payload.get_node_resp.node,
                   &data->canned_node, sizeof(net_node_addr_t));
            break;
            
        case NET_MSG_NOTIFY:
            response->header.msg_type = NET_MSG_NOTIFY_RESPONSE;
            response->payload.notify_resp.success = 1;
            break;
            
        case NET_MSG_CLOSEST_PRECEDING:
            response->header.msg_type = NET_MSG_CLOSEST_PRECEDING_RESPONSE;
            memcpy(&response->payload.closest_preceding_resp.node,
                   &data->canned_node, sizeof(net_node_addr_t));
            break;
            
        case NET_MSG_PING:
            response->header.msg_type = NET_MSG_PING_RESPONSE;
            response->payload.ping_resp.alive = data->canned_alive;
            response->payload.ping_resp.state = data->canned_state;
            break;
            
        default:
            return NET_ERR_INVALID_MESSAGE;
    }
    
    /* Record response */
    if (data->response_count < FAKE_PEER_MAX_HISTORY) {
        memcpy(&data->response_history[data->response_count], response, sizeof(net_message_t));
        data->response_count++;
    }
    
    return NET_ERR_OK;
}

static int fake_peer_send_request_async_impl(net_peer_t *peer, const net_message_t *request,
                                             net_peer_callback_t callback, void *context, int timeout_ms) {
    /* For now, just call synchronous version and invoke callback */
    net_message_t response;
    int result = fake_peer_send_request_impl(peer, request, &response, timeout_ms);
    
    if (callback) {
        callback(context, &response, result);
    }
    
    return result;
}

static void fake_peer_close_impl(net_peer_t *peer) {
    peer->connected = 0;
}

static void fake_peer_destroy_impl(net_peer_t *peer) {
    if (peer->impl_data) {
        free(peer->impl_data);
        peer->impl_data = NULL;
    }
    free(peer);
}

/* Interface vtable */
static const net_peer_iface_t fake_peer_iface = {
    .connect = fake_peer_connect_impl,
    .send_request = fake_peer_send_request_impl,
    .send_request_async = fake_peer_send_request_async_impl,
    .close = fake_peer_close_impl,
    .destroy = fake_peer_destroy_impl
};

const net_peer_iface_t* fake_peer_get_interface(void) {
    return &fake_peer_iface;
}

net_peer_t* fake_peer_create(void) {
    net_peer_t *peer = (net_peer_t*)malloc(sizeof(net_peer_t));
    if (!peer) return NULL;
    
    fake_peer_data_t *data = (fake_peer_data_t*)malloc(sizeof(fake_peer_data_t));
    if (!data) {
        free(peer);
        return NULL;
    }
    
    memset(peer, 0, sizeof(net_peer_t));
    memset(data, 0, sizeof(fake_peer_data_t));
    
    peer->iface = &fake_peer_iface;
    peer->impl_data = data;
    peer->connected = 0;
    
    /* Set default canned responses */
    data->canned_has_node = 1;
    data->canned_alive = 1;
    data->canned_state = 1;  /* NODE_STATE_RUNNING */
    
    return peer;
}

void fake_peer_set_canned_node(net_peer_t *peer, const char *id, int key, const char *url) {
    fake_peer_data_t *data = (fake_peer_data_t*)peer->impl_data;
    strncpy(data->canned_node.id, id, NET_PROTOCOL_MAX_NODE_ID - 1);
    data->canned_node.id[NET_PROTOCOL_MAX_NODE_ID - 1] = '\0';
    data->canned_node.key = key;
    strncpy(data->canned_node.url, url, NET_PROTOCOL_MAX_URL - 1);
    data->canned_node.url[NET_PROTOCOL_MAX_URL - 1] = '\0';
}

void fake_peer_set_canned_has_node(net_peer_t *peer, int has_node) {
    fake_peer_data_t *data = (fake_peer_data_t*)peer->impl_data;
    data->canned_has_node = has_node;
}

void fake_peer_set_canned_alive(net_peer_t *peer, int alive, int state) {
    fake_peer_data_t *data = (fake_peer_data_t*)peer->impl_data;
    data->canned_alive = alive;
    data->canned_state = state;
}

void fake_peer_inject_error(net_peer_t *peer, int error_code) {
    fake_peer_data_t *data = (fake_peer_data_t*)peer->impl_data;
    data->inject_error = error_code;
}

void fake_peer_inject_timeout(net_peer_t *peer, int enable) {
    fake_peer_data_t *data = (fake_peer_data_t*)peer->impl_data;
    data->inject_timeout = enable;
}

int fake_peer_get_request_count(net_peer_t *peer) {
    fake_peer_data_t *data = (fake_peer_data_t*)peer->impl_data;
    return data->request_count;
}

const net_message_t* fake_peer_get_request(net_peer_t *peer, int index) {
    fake_peer_data_t *data = (fake_peer_data_t*)peer->impl_data;
    if (index < 0 || index >= data->request_count) {
        return NULL;
    }
    return &data->request_history[index];
}

int fake_peer_get_response_count(net_peer_t *peer) {
    fake_peer_data_t *data = (fake_peer_data_t*)peer->impl_data;
    return data->response_count;
}

const net_message_t* fake_peer_get_response(net_peer_t *peer, int index) {
    fake_peer_data_t *data = (fake_peer_data_t*)peer->impl_data;
    if (index < 0 || index >= data->response_count) {
        return NULL;
    }
    return &data->response_history[index];
}

void fake_peer_reset_history(net_peer_t *peer) {
    fake_peer_data_t *data = (fake_peer_data_t*)peer->impl_data;
    data->request_count = 0;
    data->response_count = 0;
}

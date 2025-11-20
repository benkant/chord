#ifndef NET_PROTOCOL_H
#define NET_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>

/*
 * Network Protocol Layer
 * 
 * Defines wire format for Chord RPC messages.
 * Handles message encoding/decoding, versioning, and error codes.
 * 
 * Design principles:
 * - Simple, fixed-size headers for efficiency
 * - JSON payloads for debuggability (can switch to binary later)
 * - Version field for protocol evolution
 * - Request/response correlation via request_id
 */

/* Protocol version */
#define NET_PROTOCOL_VERSION 1

/* Maximum message sizes */
#define NET_PROTOCOL_MAX_PAYLOAD 4096
#define NET_PROTOCOL_MAX_NODE_ID 64
#define NET_PROTOCOL_MAX_URL 256

/* Message types */
typedef enum {
    NET_MSG_FIND_SUCCESSOR = 1,
    NET_MSG_FIND_SUCCESSOR_RESPONSE = 2,
    NET_MSG_GET_PREDECESSOR = 3,
    NET_MSG_GET_PREDECESSOR_RESPONSE = 4,
    NET_MSG_GET_SUCCESSOR = 5,
    NET_MSG_GET_SUCCESSOR_RESPONSE = 6,
    NET_MSG_NOTIFY = 7,
    NET_MSG_NOTIFY_RESPONSE = 8,
    NET_MSG_CLOSEST_PRECEDING = 9,
    NET_MSG_CLOSEST_PRECEDING_RESPONSE = 10,
    NET_MSG_PING = 11,
    NET_MSG_PING_RESPONSE = 12,
    NET_MSG_ERROR = 255
} net_msg_type_t;

/* Error codes */
typedef enum {
    NET_ERR_OK = 0,
    NET_ERR_INVALID_MESSAGE = 1,
    NET_ERR_TIMEOUT = 2,
    NET_ERR_NODE_NOT_FOUND = 3,
    NET_ERR_INTERNAL = 4,
    NET_ERR_VERSION_MISMATCH = 5
} net_error_t;

/* Node address structure (replaces Node* for remote references) */
typedef struct {
    char id[NET_PROTOCOL_MAX_NODE_ID];
    int key;
    char url[NET_PROTOCOL_MAX_URL];
} net_node_addr_t;

/* Message header (fixed size) */
typedef struct {
    uint8_t version;
    uint8_t msg_type;
    uint16_t payload_len;
    uint32_t request_id;
} net_msg_header_t;

/* Find successor request */
typedef struct {
    int key;
} net_find_successor_req_t;

/* Find successor response */
typedef struct {
    net_node_addr_t node;
} net_find_successor_resp_t;

/* Get predecessor/successor response */
typedef struct {
    net_node_addr_t node;
    int has_node;  /* 0 if NULL, 1 if valid */
} net_get_node_resp_t;

/* Notify request */
typedef struct {
    net_node_addr_t node;
} net_notify_req_t;

/* Notify response */
typedef struct {
    int success;
} net_notify_resp_t;

/* Closest preceding request */
typedef struct {
    int key;
} net_closest_preceding_req_t;

/* Closest preceding response */
typedef struct {
    net_node_addr_t node;
} net_closest_preceding_resp_t;

/* Ping response */
typedef struct {
    int alive;
    int state;
} net_ping_resp_t;

/* Error message */
typedef struct {
    net_error_t error_code;
    char error_msg[256];
} net_error_msg_t;

/* Generic message structure */
typedef struct {
    net_msg_header_t header;
    union {
        net_find_successor_req_t find_successor_req;
        net_find_successor_resp_t find_successor_resp;
        net_get_node_resp_t get_node_resp;
        net_notify_req_t notify_req;
        net_notify_resp_t notify_resp;
        net_closest_preceding_req_t closest_preceding_req;
        net_closest_preceding_resp_t closest_preceding_resp;
        net_ping_resp_t ping_resp;
        net_error_msg_t error;
        char raw_payload[NET_PROTOCOL_MAX_PAYLOAD];
    } payload;
} net_message_t;

/*
 * Protocol API
 */

/* Initialize a message with header */
void net_protocol_init_message(net_message_t *msg, net_msg_type_t type, uint32_t request_id);

/* Serialize message to buffer (returns bytes written, or -1 on error) */
int net_protocol_serialize(const net_message_t *msg, void *buffer, size_t buffer_size);

/* Deserialize buffer to message (returns 0 on success, error code on failure) */
int net_protocol_deserialize(net_message_t *msg, const void *buffer, size_t buffer_size);

/* Create error message */
void net_protocol_create_error(net_message_t *msg, uint32_t request_id, 
                                net_error_t error_code, const char *error_msg);

/* Helper to copy node address */
void net_protocol_copy_node_addr(net_node_addr_t *dest, const char *id, int key, const char *url);

/* Validate message (returns 0 if valid, error code otherwise) */
int net_protocol_validate(const net_message_t *msg);

#endif /* NET_PROTOCOL_H */

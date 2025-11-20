#include <stdio.h>
#include <string.h>
#include "../chord_test.h"
#include "../fakes/fake_peer.h"

/*
 * Unit tests for net_peer with fake implementation
 * 
 * Tests cover:
 * - Fake peer creation and destruction
 * - Canned response configuration
 * - Request/response recording
 * - Error injection
 * - High-level RPC helpers
 */

static void test_fake_peer_creation(void) {
    CHORD_TEST("fake_peer_create and destroy");
    
    net_peer_t *peer = fake_peer_create();
    CHORD_TEST_ASSERT_NOT_NULL(peer, "Peer created successfully");
    CHORD_TEST_ASSERT_EQ(peer->connected, 0, "Peer not connected initially");
    
    net_peer_destroy(peer);
}

static void test_fake_peer_connect(void) {
    CHORD_TEST("fake_peer_connect sets URL");
    
    net_peer_t *peer = fake_peer_create();
    
    int result = net_peer_connect(peer, "tcp://localhost:5555");
    CHORD_TEST_ASSERT_EQ(result, 0, "Connect succeeds");
    CHORD_TEST_ASSERT_EQ(peer->connected, 1, "Peer marked as connected");
    CHORD_TEST_ASSERT_STR_EQ(peer->remote_url, "tcp://localhost:5555", "URL stored correctly");
    
    net_peer_destroy(peer);
}

static void test_fake_peer_find_successor(void) {
    CHORD_TEST("fake_peer find_successor with canned response");
    
    net_peer_t *peer = fake_peer_create();
    net_peer_connect(peer, "tcp://localhost:5555");
    
    /* Configure canned response */
    fake_peer_set_canned_node(peer, "node1", 42, "tcp://node1:5555");
    
    /* Call find_successor */
    net_node_addr_t result;
    int err = net_peer_find_successor(peer, 100, &result, 5000);
    
    CHORD_TEST_ASSERT_EQ(err, NET_ERR_OK, "find_successor succeeds");
    CHORD_TEST_ASSERT_STR_EQ(result.id, "node1", "Result ID matches");
    CHORD_TEST_ASSERT_EQ(result.key, 42, "Result key matches");
    CHORD_TEST_ASSERT_STR_EQ(result.url, "tcp://node1:5555", "Result URL matches");
    
    /* Verify request was recorded */
    int req_count = fake_peer_get_request_count(peer);
    CHORD_TEST_ASSERT_EQ(req_count, 1, "One request recorded");
    
    const net_message_t *req = fake_peer_get_request(peer, 0);
    CHORD_TEST_ASSERT_NOT_NULL(req, "Request retrieved");
    CHORD_TEST_ASSERT_EQ((int)req->header.msg_type, NET_MSG_FIND_SUCCESSOR, "Request type correct");
    CHORD_TEST_ASSERT_EQ(req->payload.find_successor_req.key, 100, "Request key correct");
    
    net_peer_destroy(peer);
}

static void test_fake_peer_get_predecessor(void) {
    CHORD_TEST("fake_peer get_predecessor");
    
    net_peer_t *peer = fake_peer_create();
    net_peer_connect(peer, "tcp://localhost:5555");
    
    /* Configure canned response */
    fake_peer_set_canned_node(peer, "pred", 10, "tcp://pred:5555");
    fake_peer_set_canned_has_node(peer, 1);
    
    /* Call get_predecessor */
    net_node_addr_t result;
    int has_pred;
    int err = net_peer_get_predecessor(peer, &result, &has_pred, 5000);
    
    CHORD_TEST_ASSERT_EQ(err, NET_ERR_OK, "get_predecessor succeeds");
    CHORD_TEST_ASSERT_EQ(has_pred, 1, "Has predecessor");
    CHORD_TEST_ASSERT_STR_EQ(result.id, "pred", "Predecessor ID matches");
    
    net_peer_destroy(peer);
}

static void test_fake_peer_get_predecessor_null(void) {
    CHORD_TEST("fake_peer get_predecessor returns NULL");
    
    net_peer_t *peer = fake_peer_create();
    net_peer_connect(peer, "tcp://localhost:5555");
    
    /* Configure no predecessor */
    fake_peer_set_canned_has_node(peer, 0);
    
    /* Call get_predecessor */
    net_node_addr_t result;
    int has_pred;
    int err = net_peer_get_predecessor(peer, &result, &has_pred, 5000);
    
    CHORD_TEST_ASSERT_EQ(err, NET_ERR_OK, "get_predecessor succeeds");
    CHORD_TEST_ASSERT_EQ(has_pred, 0, "No predecessor");
    
    net_peer_destroy(peer);
}

static void test_fake_peer_notify(void) {
    CHORD_TEST("fake_peer notify");
    
    net_peer_t *peer = fake_peer_create();
    net_peer_connect(peer, "tcp://localhost:5555");
    
    /* Call notify */
    net_node_addr_t node;
    strncpy(node.id, "new_node", NET_PROTOCOL_MAX_NODE_ID - 1);
    node.key = 50;
    strncpy(node.url, "tcp://new:5555", NET_PROTOCOL_MAX_URL - 1);
    
    int err = net_peer_notify(peer, &node, 5000);
    
    CHORD_TEST_ASSERT_EQ(err, NET_ERR_OK, "notify succeeds");
    
    /* Verify request */
    const net_message_t *req = fake_peer_get_request(peer, 0);
    CHORD_TEST_ASSERT_EQ((int)req->header.msg_type, NET_MSG_NOTIFY, "Request type correct");
    CHORD_TEST_ASSERT_STR_EQ(req->payload.notify_req.node.id, "new_node", "Notify node ID correct");
    
    net_peer_destroy(peer);
}

static void test_fake_peer_ping(void) {
    CHORD_TEST("fake_peer ping");
    
    net_peer_t *peer = fake_peer_create();
    net_peer_connect(peer, "tcp://localhost:5555");
    
    /* Configure canned response */
    fake_peer_set_canned_alive(peer, 1, 1);
    
    /* Call ping */
    int alive, state;
    int err = net_peer_ping(peer, &alive, &state, 5000);
    
    CHORD_TEST_ASSERT_EQ(err, NET_ERR_OK, "ping succeeds");
    CHORD_TEST_ASSERT_EQ(alive, 1, "Node is alive");
    CHORD_TEST_ASSERT_EQ(state, 1, "Node state correct");
    
    net_peer_destroy(peer);
}

static void test_fake_peer_error_injection(void) {
    CHORD_TEST("fake_peer error injection");
    
    net_peer_t *peer = fake_peer_create();
    net_peer_connect(peer, "tcp://localhost:5555");
    
    /* Inject error */
    fake_peer_inject_error(peer, NET_ERR_TIMEOUT);
    
    /* Call should fail with injected error */
    net_node_addr_t result;
    int err = net_peer_find_successor(peer, 100, &result, 5000);
    
    CHORD_TEST_ASSERT_EQ(err, NET_ERR_TIMEOUT, "Injected error returned");
    
    net_peer_destroy(peer);
}

static void test_fake_peer_timeout_injection(void) {
    CHORD_TEST("fake_peer timeout injection");
    
    net_peer_t *peer = fake_peer_create();
    net_peer_connect(peer, "tcp://localhost:5555");
    
    /* Inject timeout */
    fake_peer_inject_timeout(peer, 1);
    
    /* Call should timeout */
    net_node_addr_t result;
    int err = net_peer_find_successor(peer, 100, &result, 5000);
    
    CHORD_TEST_ASSERT_EQ(err, NET_ERR_TIMEOUT, "Timeout error returned");
    
    net_peer_destroy(peer);
}

int main(void) {
    CHORD_TEST_INIT();
    
    /* Run all tests */
    CHORD_RUN_TEST(test_fake_peer_creation);
    CHORD_RUN_TEST(test_fake_peer_connect);
    CHORD_RUN_TEST(test_fake_peer_find_successor);
    CHORD_RUN_TEST(test_fake_peer_get_predecessor);
    CHORD_RUN_TEST(test_fake_peer_get_predecessor_null);
    CHORD_RUN_TEST(test_fake_peer_notify);
    CHORD_RUN_TEST(test_fake_peer_ping);
    CHORD_RUN_TEST(test_fake_peer_error_injection);
    CHORD_RUN_TEST(test_fake_peer_timeout_injection);
    
    CHORD_TEST_FINI();
}

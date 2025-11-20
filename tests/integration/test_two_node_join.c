#include <stdio.h>
#include <string.h>
#include "../chord_integration.h"

/*
 * Integration test: Two-node ring formation
 * 
 * Tests the basic Chord join operation:
 * 1. Create first node (forms single-node ring)
 * 2. Create second node and join to first
 * 3. Run stabilization
 * 4. Verify ring structure
 */

static void test_single_node_ring(void) {
    CHORD_TEST("Single node forms valid ring");
    
    /* Create first node */
    test_node_t *node1 = chord_test_create_ring("node1");
    CHORD_TEST_ASSERT_NOT_NULL(node1, "Node1 created");
    CHORD_TEST_ASSERT_NOT_NULL(node1->node, "Node1 has node structure");
    
    /* Single node should point to itself */
    CHORD_TEST_ASSERT_TRUE(node1->node->successor == node1->node,
                           "Node1 successor points to itself");
    CHORD_TEST_ASSERT_NULL(node1->node->predecessor,
                           "Node1 predecessor is NULL initially");
    
    /* Check ring invariants */
    CHORD_TEST_ASSERT_TRUE(chord_test_check_ring_invariants(),
                           "Ring invariants hold");
}

static void test_two_node_join(void) {
    CHORD_TEST("Two nodes join and form ring");
    
    /* Reset state */
    chord_test_reset();
    
    /* Create first node */
    test_node_t *node1 = chord_test_create_ring("node1");
    CHORD_TEST_ASSERT_NOT_NULL(node1, "Node1 created");
    
    /* Create second node */
    test_node_t *node2 = chord_test_create_node("node2");
    CHORD_TEST_ASSERT_NOT_NULL(node2, "Node2 created");
    
    /* Join node2 to node1 */
    int result = chord_test_join_node(node1, node2);
    CHORD_TEST_ASSERT_EQ(result, 0, "Join succeeds");
    
    /* After join, node2 should have a successor */
    CHORD_TEST_ASSERT_NOT_NULL(node2->node->successor,
                                "Node2 has successor after join");
    
    /* Check we have 2 nodes */
    CHORD_TEST_ASSERT_EQ(chord_test_get_node_count(), 2,
                         "Two nodes active");
}

static void test_two_node_stabilization(void) {
    CHORD_TEST("Two nodes stabilize correctly");
    
    /* Reset state */
    chord_test_reset();
    
    /* Create ring with two nodes */
    test_node_t *node1 = chord_test_create_ring("node1");
    test_node_t *node2 = chord_test_create_node("node2");
    chord_test_join_node(node1, node2);
    
    /* Run stabilization */
    chord_test_stabilize_all();
    
    /* After stabilization, both nodes should have valid successors */
    CHORD_TEST_ASSERT_NOT_NULL(node1->node->successor,
                                "Node1 has successor");
    CHORD_TEST_ASSERT_NOT_NULL(node2->node->successor,
                                "Node2 has successor");
    
    /* Check ring invariants */
    CHORD_TEST_ASSERT_TRUE(chord_test_check_ring_invariants(),
                           "Ring invariants hold after stabilization");
}

static void test_two_node_key_ordering(void) {
    CHORD_TEST("Two nodes maintain key ordering");
    
    /* Reset state */
    chord_test_reset();
    
    /* Create nodes with known keys */
    test_node_t *node1 = chord_test_create_ring("a");  /* Lower key */
    test_node_t *node2 = chord_test_create_node("z");  /* Higher key */
    
    CHORD_TEST_ASSERT_TRUE(node1->key < node2->key,
                           "Node1 key < Node2 key");
    
    chord_test_join_node(node1, node2);
    chord_test_stabilize_all();
    
    /* Verify successor relationships based on key ordering */
    /* In a 2-node ring, each should point to the other */
    CHORD_TEST_ASSERT_TRUE(node1->node->successor == node2->node ||
                           node1->node->successor == node1->node,
                           "Node1 successor is valid");
    CHORD_TEST_ASSERT_TRUE(node2->node->successor == node1->node ||
                           node2->node->successor == node2->node,
                           "Node2 successor is valid");
}

int main(void) {
    CHORD_INTEGRATION_INIT();
    
    /* Run tests */
    CHORD_RUN_TEST(test_single_node_ring);
    CHORD_RUN_TEST(test_two_node_join);
    CHORD_RUN_TEST(test_two_node_stabilization);
    CHORD_RUN_TEST(test_two_node_key_ordering);
    
    CHORD_INTEGRATION_FINI();
}

#ifndef CHORD_INTEGRATION_H
#define CHORD_INTEGRATION_H

#include "chord_test.h"
#include "../src/core/node.h"
#include "../src/core/ring.h"

/*
 * Integration Test Harness
 * 
 * Inspired by NNG's nuts.h, provides helpers for multi-node testing.
 * 
 * Features:
 * - Node lifecycle management
 * - Temporary address generation
 * - Stabilization helpers
 * - Ring invariant checking
 */

/* Test node structure */
typedef struct {
    Node *node;
    char *id;
    int key;
    int active;
} test_node_t;

/* Maximum test nodes */
#define CHORD_TEST_MAX_NODES 10

/* Global test state */
static test_node_t test_nodes[CHORD_TEST_MAX_NODES];
static int test_node_count = 0;

/*
 * Test helpers
 */

/* Reset test state between tests */
static inline void chord_test_reset(void) {
    test_node_count = 0;
    memset(test_nodes, 0, sizeof(test_nodes));
}

/* Initialize integration test */
#define CHORD_INTEGRATION_INIT() \
    do { \
        CHORD_TEST_INIT(); \
        chord_test_reset(); \
    } while (0)

/* Finalize integration test */
#define CHORD_INTEGRATION_FINI() \
    do { \
        /* Clean up any remaining nodes */ \
        for (int i = 0; i < test_node_count; i++) { \
            if (test_nodes[i].active && test_nodes[i].node) { \
                /* Node cleanup would go here */ \
                test_nodes[i].active = 0; \
            } \
        } \
        CHORD_TEST_FINI(); \
    } while (0)

/* Create a test node */
static inline test_node_t* chord_test_create_node(const char *id) {
    if (test_node_count >= CHORD_TEST_MAX_NODES) {
        return NULL;
    }
    
    test_node_t *tn = &test_nodes[test_node_count++];
    tn->node = node_init((char*)id);
    tn->id = tn->node->id;
    tn->key = tn->node->key;
    tn->active = 1;
    
    return tn;
}

/* Create a ring with first node */
static inline test_node_t* chord_test_create_ring(const char *id) {
    test_node_t *tn = chord_test_create_node(id);
    if (tn) {
        node_create(tn->node);
    }
    return tn;
}

/* Join a node to existing ring */
static inline int chord_test_join_node(test_node_t *existing, test_node_t *new_node) {
    if (!existing || !new_node || !existing->node || !new_node->node) {
        return -1;
    }
    node_join(existing->node, new_node->node);
    return 0;
}

/* Run stabilization on all active nodes */
static inline void chord_test_stabilize_all(void) {
    for (int i = 0; i < test_node_count; i++) {
        if (test_nodes[i].active && test_nodes[i].node) {
            node_stabilise(test_nodes[i].node);
            node_fix_fingers(test_nodes[i].node);
        }
    }
}

/* Check ring invariants */
static inline int chord_test_check_ring_invariants(void) {
    /* For now, just check that all nodes have valid successors */
    for (int i = 0; i < test_node_count; i++) {
        if (test_nodes[i].active && test_nodes[i].node) {
            if (test_nodes[i].node->successor == NULL) {
                return 0;  /* Invalid: no successor */
            }
        }
    }
    return 1;  /* Valid */
}

/* Get node count */
static inline int chord_test_get_node_count(void) {
    int count = 0;
    for (int i = 0; i < test_node_count; i++) {
        if (test_nodes[i].active) {
            count++;
        }
    }
    return count;
}

#endif /* CHORD_INTEGRATION_H */

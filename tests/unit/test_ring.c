#include <stdio.h>
#include <stdint.h>
#include "../chord_test.h"
#include "../../src/core/ring.h"
#include "../../src/core/chord_types.h"

/*
 * Unit tests for ring.c - ring operations
 * 
 * Tests cover:
 * - ring_get() initialization
 * - ring_size() returns correct size
 * - ring_key_max() returns correct keyspace size
 */

static void test_ring_get_initialization(void) {
    CHORD_TEST("ring_get initializes ring singleton");
    
    Ring *r = ring_get();
    
    CHORD_TEST_ASSERT_NOT_NULL(r, "ring_get returns non-NULL");
    CHORD_TEST_ASSERT_EQ((int)r->size, 0, "Initial ring size is 0");
    CHORD_TEST_ASSERT_NULL(r->first_node, "Initial first_node is NULL");
}

static void test_ring_key_max(void) {
    CHORD_TEST("ring_key_max returns correct keyspace size");
    
    int max = ring_key_max();
    int expected = 1 << KEY_BITS;  /* 2^8 = 256 for 8-bit keyspace */
    
    CHORD_TEST_ASSERT_EQ(max, expected, "ring_key_max returns 2^KEY_BITS");
}

static void test_ring_size_empty(void) {
    CHORD_TEST("ring_size returns 0 for empty ring");
    
    /* Get fresh ring (ensures initialization) */
    (void)ring_get();
    
    /* Size should be 0 initially */
    int size = ring_size();
    CHORD_TEST_ASSERT_EQ(size, 0, "Empty ring has size 0");
}

int main(void) {
    CHORD_TEST_INIT();
    
    /* Run all tests */
    CHORD_RUN_TEST(test_ring_get_initialization);
    CHORD_RUN_TEST(test_ring_key_max);
    CHORD_RUN_TEST(test_ring_size_empty);
    
    CHORD_TEST_FINI();
}

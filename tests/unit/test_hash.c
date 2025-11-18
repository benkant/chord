#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../chord_test.h"
#include "../../hash.h"
#include "../../chord_types.h"

/*
 * Unit tests for hash.c - consistent hashing functions
 * 
 * Tests cover:
 * - Deterministic hashing (same input = same output)
 * - Range validation (hash values within keyspace bounds)
 * - Distribution properties
 * - Edge cases (empty string, special characters)
 */

static void test_chord_hash_deterministic(void) {
    CHORD_TEST("chord_hash is deterministic");
    
    int hash1 = chord_hash("node1");
    int hash2 = chord_hash("node1");
    
    CHORD_TEST_ASSERT_EQ(hash1, hash2, "Same input produces same hash");
}

static void test_chord_hash_range(void) {
    CHORD_TEST("chord_hash produces values in valid range");
    
    int max_key = 1 << KEY_BITS;  /* 2^8 = 256 for 8-bit keyspace */
    
    int hash1 = chord_hash("node1");
    int hash2 = chord_hash("node2");
    int hash3 = chord_hash("test_node_with_long_name");
    
    CHORD_TEST_ASSERT_TRUE(hash1 >= 0 && hash1 < max_key, 
                           "hash1 is within [0, 256)");
    CHORD_TEST_ASSERT_TRUE(hash2 >= 0 && hash2 < max_key, 
                           "hash2 is within [0, 256)");
    CHORD_TEST_ASSERT_TRUE(hash3 >= 0 && hash3 < max_key, 
                           "hash3 is within [0, 256)");
}

static void test_chord_hash_distribution(void) {
    CHORD_TEST("chord_hash distributes different inputs");
    
    int hash1 = chord_hash("node1");
    int hash2 = chord_hash("node2");
    int hash3 = chord_hash("node3");
    
    /* Different inputs should produce different hashes (in most cases) */
    CHORD_TEST_ASSERT_NE(hash1, hash2, "node1 and node2 have different hashes");
    CHORD_TEST_ASSERT_NE(hash2, hash3, "node2 and node3 have different hashes");
    CHORD_TEST_ASSERT_NE(hash1, hash3, "node1 and node3 have different hashes");
}

static void test_chord_hash_empty_string(void) {
    CHORD_TEST("chord_hash handles empty string");
    
    int max_key = 1 << KEY_BITS;
    int hash_empty = chord_hash("");
    
    CHORD_TEST_ASSERT_TRUE(hash_empty >= 0 && hash_empty < max_key,
                           "Empty string hash is within valid range");
    CHORD_TEST_ASSERT_EQ(hash_empty, 0, "Empty string hashes to 0");
}

static void test_chord_hash_special_chars(void) {
    CHORD_TEST("chord_hash handles special characters");
    
    int max_key = 1 << KEY_BITS;
    
    int hash1 = chord_hash("node-1");
    int hash2 = chord_hash("node_1");
    int hash3 = chord_hash("node.1");
    
    CHORD_TEST_ASSERT_TRUE(hash1 >= 0 && hash1 < max_key,
                           "Hash with dash is valid");
    CHORD_TEST_ASSERT_TRUE(hash2 >= 0 && hash2 < max_key,
                           "Hash with underscore is valid");
    CHORD_TEST_ASSERT_TRUE(hash3 >= 0 && hash3 < max_key,
                           "Hash with dot is valid");
    
    /* Special characters should affect the hash */
    CHORD_TEST_ASSERT_NE(hash1, hash2, "Dash vs underscore produces different hash");
    CHORD_TEST_ASSERT_NE(hash2, hash3, "Underscore vs dot produces different hash");
}

static void test_chord_hash_collision_resistance(void) {
    CHORD_TEST("chord_hash has reasonable collision resistance");
    
    /* Test a small set of similar strings */
    int hash_a = chord_hash("a");
    int hash_b = chord_hash("b");
    int hash_aa = chord_hash("aa");
    int hash_ab = chord_hash("ab");
    
    /* These should all be different (though collisions are possible in 8-bit space) */
    int unique_count = 4;
    if (hash_a == hash_b) unique_count--;
    if (hash_a == hash_aa) unique_count--;
    if (hash_a == hash_ab) unique_count--;
    if (hash_b == hash_aa) unique_count--;
    if (hash_b == hash_ab) unique_count--;
    if (hash_aa == hash_ab) unique_count--;
    
    /* We expect at least 3 unique hashes out of 4 inputs */
    CHORD_TEST_ASSERT_TRUE(unique_count >= 3,
                           "At least 3 out of 4 similar inputs have unique hashes");
}

int main(void) {
    CHORD_TEST_INIT();
    
    /* Initialize ring singleton required by chord_hash */
    ring_get();
    
    /* Run all tests */
    CHORD_RUN_TEST(test_chord_hash_deterministic);
    CHORD_RUN_TEST(test_chord_hash_range);
    CHORD_RUN_TEST(test_chord_hash_distribution);
    CHORD_RUN_TEST(test_chord_hash_empty_string);
    CHORD_RUN_TEST(test_chord_hash_special_chars);
    CHORD_RUN_TEST(test_chord_hash_collision_resistance);
    
    CHORD_TEST_FINI();
}

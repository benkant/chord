#include <stdio.h>
#include <stdint.h>
#include "../chord_test.h"
#include "../../src/core/key.h"
#include "../../src/core/chord_types.h"

/*
 * Unit tests for key.c - key range checking functions
 * 
 * Tests cover:
 * - key_in_range() with half-open intervals (a, b]
 * - key_in_range() with open intervals (a, b)
 * - Wrap-around behavior for circular keyspace
 * - Edge cases at boundaries
 */

static void test_key_in_range_half_open_normal(void) {
    CHORD_TEST("key_in_range half-open (a,b] - normal case");
    
    /* Range (10, 20] - half-open, right-inclusive */
    CHORD_TEST_ASSERT_FALSE(key_in_range(10, 10, 20, TRUE), 
                            "10 not in (10, 20]");
    CHORD_TEST_ASSERT_TRUE(key_in_range(15, 10, 20, TRUE), 
                           "15 in (10, 20]");
    CHORD_TEST_ASSERT_TRUE(key_in_range(20, 10, 20, TRUE), 
                           "20 in (10, 20]");
    CHORD_TEST_ASSERT_FALSE(key_in_range(21, 10, 20, TRUE), 
                            "21 not in (10, 20]");
    CHORD_TEST_ASSERT_FALSE(key_in_range(5, 10, 20, TRUE), 
                            "5 not in (10, 20]");
}

static void test_key_in_range_open_normal(void) {
    CHORD_TEST("key_in_range open (a,b) - normal case");
    
    /* Range (10, 20) - open, both exclusive */
    CHORD_TEST_ASSERT_FALSE(key_in_range(10, 10, 20, FALSE), 
                            "10 not in (10, 20)");
    CHORD_TEST_ASSERT_TRUE(key_in_range(15, 10, 20, FALSE), 
                           "15 in (10, 20)");
    CHORD_TEST_ASSERT_FALSE(key_in_range(20, 10, 20, FALSE), 
                            "20 not in (10, 20)");
    CHORD_TEST_ASSERT_FALSE(key_in_range(21, 10, 20, FALSE), 
                            "21 not in (10, 20)");
    CHORD_TEST_ASSERT_FALSE(key_in_range(5, 10, 20, FALSE), 
                            "5 not in (10, 20)");
}

static void test_key_in_range_half_open_wraparound(void) {
    CHORD_TEST("key_in_range half-open (a,b] - wraparound");
    
    /* Range (250, 10] wraps around in 8-bit keyspace (0-255) */
    CHORD_TEST_ASSERT_FALSE(key_in_range(250, 250, 10, TRUE), 
                            "250 not in (250, 10]");
    CHORD_TEST_ASSERT_TRUE(key_in_range(255, 250, 10, TRUE), 
                           "255 in (250, 10]");
    CHORD_TEST_ASSERT_TRUE(key_in_range(0, 250, 10, TRUE), 
                           "0 in (250, 10]");
    CHORD_TEST_ASSERT_TRUE(key_in_range(5, 250, 10, TRUE), 
                           "5 in (250, 10]");
    CHORD_TEST_ASSERT_TRUE(key_in_range(10, 250, 10, TRUE), 
                           "10 in (250, 10]");
    CHORD_TEST_ASSERT_FALSE(key_in_range(11, 250, 10, TRUE), 
                            "11 not in (250, 10]");
    CHORD_TEST_ASSERT_FALSE(key_in_range(200, 250, 10, TRUE), 
                            "200 not in (250, 10]");
}

static void test_key_in_range_open_wraparound(void) {
    CHORD_TEST("key_in_range open (a,b) - wraparound");
    
    /* Range (250, 10) wraps around in 8-bit keyspace (0-255) */
    CHORD_TEST_ASSERT_FALSE(key_in_range(250, 250, 10, FALSE), 
                            "250 not in (250, 10)");
    CHORD_TEST_ASSERT_TRUE(key_in_range(255, 250, 10, FALSE), 
                           "255 in (250, 10)");
    CHORD_TEST_ASSERT_TRUE(key_in_range(0, 250, 10, FALSE), 
                           "0 in (250, 10)");
    CHORD_TEST_ASSERT_TRUE(key_in_range(5, 250, 10, FALSE), 
                           "5 in (250, 10)");
    CHORD_TEST_ASSERT_FALSE(key_in_range(10, 250, 10, FALSE), 
                            "10 not in (250, 10)");
    CHORD_TEST_ASSERT_FALSE(key_in_range(11, 250, 10, FALSE), 
                            "11 not in (250, 10)");
}

static void test_key_in_range_edge_cases(void) {
    CHORD_TEST("key_in_range edge cases");
    
    /* Single-element range (5, 6] */
    CHORD_TEST_ASSERT_FALSE(key_in_range(5, 5, 6, TRUE), 
                            "5 not in (5, 6]");
    CHORD_TEST_ASSERT_TRUE(key_in_range(6, 5, 6, TRUE), 
                           "6 in (5, 6]");
    CHORD_TEST_ASSERT_FALSE(key_in_range(7, 5, 6, TRUE), 
                            "7 not in (5, 6]");
    
    /* Adjacent values (100, 101] */
    CHORD_TEST_ASSERT_FALSE(key_in_range(100, 100, 101, TRUE), 
                            "100 not in (100, 101]");
    CHORD_TEST_ASSERT_TRUE(key_in_range(101, 100, 101, TRUE), 
                           "101 in (100, 101]");
    
    /* Same bound (50, 50] - empty range */
    CHORD_TEST_ASSERT_FALSE(key_in_range(50, 50, 50, TRUE), 
                            "50 not in (50, 50]");
    CHORD_TEST_ASSERT_FALSE(key_in_range(49, 50, 50, TRUE), 
                            "49 not in (50, 50]");
}

static void test_key_in_range_full_circle(void) {
    CHORD_TEST("key_in_range full circle wraparound");
    
    /* Range (0, 255] in 8-bit space - almost full circle */
    CHORD_TEST_ASSERT_FALSE(key_in_range(0, 0, 255, TRUE), 
                            "0 not in (0, 255]");
    CHORD_TEST_ASSERT_TRUE(key_in_range(1, 0, 255, TRUE), 
                           "1 in (0, 255]");
    CHORD_TEST_ASSERT_TRUE(key_in_range(128, 0, 255, TRUE), 
                           "128 in (0, 255]");
    CHORD_TEST_ASSERT_TRUE(key_in_range(255, 0, 255, TRUE), 
                           "255 in (0, 255]");
    
    /* Range (255, 254] wraps around almost full circle */
    CHORD_TEST_ASSERT_FALSE(key_in_range(255, 255, 254, TRUE), 
                            "255 not in (255, 254]");
    CHORD_TEST_ASSERT_TRUE(key_in_range(0, 255, 254, TRUE), 
                           "0 in (255, 254]");
    CHORD_TEST_ASSERT_TRUE(key_in_range(100, 255, 254, TRUE), 
                           "100 in (255, 254]");
    CHORD_TEST_ASSERT_TRUE(key_in_range(254, 255, 254, TRUE), 
                           "254 in (255, 254]");
}

int main(void) {
    CHORD_TEST_INIT();
    
    /* Run all tests */
    CHORD_RUN_TEST(test_key_in_range_half_open_normal);
    CHORD_RUN_TEST(test_key_in_range_open_normal);
    CHORD_RUN_TEST(test_key_in_range_half_open_wraparound);
    CHORD_RUN_TEST(test_key_in_range_open_wraparound);
    CHORD_RUN_TEST(test_key_in_range_edge_cases);
    CHORD_RUN_TEST(test_key_in_range_full_circle);
    
    CHORD_TEST_FINI();
}

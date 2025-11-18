#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hash.h"
#include "chord_types.h"

/* Simple test framework */
static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
  printf("Running test: %s\n", name); \
  tests_run++;

#define ASSERT_EQ(actual, expected, msg) \
  if ((actual) == (expected)) { \
    tests_passed++; \
    printf("  PASS: %s\n", msg); \
  } else { \
    printf("  FAIL: %s: expected %d, got %d\n", msg, expected, actual); \
  }

#define ASSERT_TRUE(condition, msg) \
  if (condition) { \
    tests_passed++; \
    printf("  PASS: %s\n", msg); \
  } else { \
    printf("  FAIL: %s\n", msg); \
  }

/* Test chord_hash function */
void test_chord_hash_basic() {
  int hash1, hash2, hash3, max_key, hash_empty;
  
  TEST("test_chord_hash_basic");
  
  hash1 = chord_hash("node1");
  hash2 = chord_hash("node2");
  hash3 = chord_hash("node1");
  
  /* Hash should be deterministic - same input produces same output */
  ASSERT_EQ(hash1, hash3, "Same input produces same hash");
  
  /* Hash should be within valid range [0, 2^KEY_BITS) */
  max_key = 1 << KEY_BITS;  /* 2^8 = 256 */
  ASSERT_TRUE(hash1 >= 0 && hash1 < max_key, "Hash is within valid range [0, 256)");
  ASSERT_TRUE(hash2 >= 0 && hash2 < max_key, "Hash is within valid range [0, 256)");
  
  /* Different inputs should produce different hashes (in most cases) */
  ASSERT_TRUE(hash1 != hash2, "Different inputs produce different hashes");
  
  /* Test empty string */
  hash_empty = chord_hash("");
  ASSERT_TRUE(hash_empty >= 0 && hash_empty < max_key, "Empty string hash is within valid range");
  ASSERT_EQ(hash_empty, 0, "Empty string hashes to 0");
}

int main() {
  printf("=== Chord Hash Function Tests ===\n\n");
  
  /* Initialize ring singleton required by chord_hash */
  ring_get();
  
  test_chord_hash_basic();
  
  printf("\n=== Test Summary ===\n");
  printf("Tests run: %d\n", tests_run);
  printf("Assertions passed: %d\n", tests_passed);
  
  if (tests_passed == 6) {
    printf("\n✓ All tests passed!\n");
    return 0;
  } else {
    printf("\n✗ Some tests failed\n");
    return 1;
  }
}

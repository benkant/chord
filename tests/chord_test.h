#ifndef CHORD_TEST_H
#define CHORD_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
 * Simple test framework inspired by NNG's nuts.h
 * Provides basic test infrastructure for unit and integration tests
 */

/* Test statistics */
static int chord_test_total = 0;
static int chord_test_passed = 0;
static int chord_test_failed = 0;
static const char *chord_test_current = NULL;

/* Color output support */
#define CHORD_TEST_COLOR_RED     "\x1b[31m"
#define CHORD_TEST_COLOR_GREEN   "\x1b[32m"
#define CHORD_TEST_COLOR_YELLOW  "\x1b[33m"
#define CHORD_TEST_COLOR_RESET   "\x1b[0m"

/* Test initialization and cleanup */
#define CHORD_TEST_INIT() \
    do { \
        chord_test_total = 0; \
        chord_test_passed = 0; \
        chord_test_failed = 0; \
        printf("=== Starting Test Suite ===\n\n"); \
    } while (0)

#define CHORD_TEST_FINI() \
    do { \
        printf("\n=== Test Summary ===\n"); \
        printf("Total:  %d\n", chord_test_total); \
        printf("Passed: " CHORD_TEST_COLOR_GREEN "%d" CHORD_TEST_COLOR_RESET "\n", chord_test_passed); \
        printf("Failed: " CHORD_TEST_COLOR_RED "%d" CHORD_TEST_COLOR_RESET "\n", chord_test_failed); \
        if (chord_test_failed == 0) { \
            printf("\n" CHORD_TEST_COLOR_GREEN "✓ All tests passed!" CHORD_TEST_COLOR_RESET "\n"); \
            return 0; \
        } else { \
            printf("\n" CHORD_TEST_COLOR_RED "✗ Some tests failed" CHORD_TEST_COLOR_RESET "\n"); \
            return 1; \
        } \
    } while (0)

/* Test case definition */
#define CHORD_TEST(name) \
    do { \
        chord_test_current = name; \
        chord_test_total++; \
        printf("Running: %s\n", name); \
    } while (0)

/* Assertion macros */
#define CHORD_TEST_PASS() \
    do { \
        chord_test_passed++; \
        printf("  " CHORD_TEST_COLOR_GREEN "✓" CHORD_TEST_COLOR_RESET " %s\n", chord_test_current); \
    } while (0)

#define CHORD_TEST_FAIL(msg, ...) \
    do { \
        chord_test_failed++; \
        printf("  " CHORD_TEST_COLOR_RED "✗" CHORD_TEST_COLOR_RESET " %s\n", chord_test_current); \
        printf("    " CHORD_TEST_COLOR_RED "FAIL:" CHORD_TEST_COLOR_RESET " " msg "\n", ##__VA_ARGS__); \
        printf("    at %s:%d\n", __FILE__, __LINE__); \
    } while (0)

#define CHORD_TEST_ASSERT(cond, msg, ...) \
    do { \
        if (!(cond)) { \
            CHORD_TEST_FAIL(msg, ##__VA_ARGS__); \
            return; \
        } \
    } while (0)

#define CHORD_TEST_ASSERT_EQ(actual, expected, msg) \
    do { \
        if ((actual) != (expected)) { \
            CHORD_TEST_FAIL("%s: expected %d, got %d", msg, (int)(expected), (int)(actual)); \
            return; \
        } \
    } while (0)

#define CHORD_TEST_ASSERT_NE(actual, unexpected, msg) \
    do { \
        if ((actual) == (unexpected)) { \
            CHORD_TEST_FAIL("%s: got unexpected value %d", msg, (int)(actual)); \
            return; \
        } \
    } while (0)

#define CHORD_TEST_ASSERT_TRUE(cond, msg) \
    CHORD_TEST_ASSERT(cond, msg)

#define CHORD_TEST_ASSERT_FALSE(cond, msg) \
    CHORD_TEST_ASSERT(!(cond), msg)

#define CHORD_TEST_ASSERT_NULL(ptr, msg) \
    CHORD_TEST_ASSERT((ptr) == NULL, msg)

#define CHORD_TEST_ASSERT_NOT_NULL(ptr, msg) \
    CHORD_TEST_ASSERT((ptr) != NULL, msg)

#define CHORD_TEST_ASSERT_STR_EQ(actual, expected, msg) \
    do { \
        if (strcmp((actual), (expected)) != 0) { \
            CHORD_TEST_FAIL("%s: expected \"%s\", got \"%s\"", msg, expected, actual); \
            return; \
        } \
    } while (0)

/* Test runner helper */
#define CHORD_RUN_TEST(test_func) \
    do { \
        test_func(); \
        if (chord_test_failed > 0 && chord_test_passed == 0) { \
            /* Test function failed without calling PASS */ \
        } else if (chord_test_failed == 0) { \
            CHORD_TEST_PASS(); \
        } \
    } while (0)

#endif /* CHORD_TEST_H */

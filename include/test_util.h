#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdio.h>
#include <inttypes.h> // Required for PRIu64 and PRIx64 macros

/**
 * ASSERT_INT_EQ: Compares two integers (Square, Rank, File, int).
 */
#define ASSERT_INT_EQ(expected, actual, func_name) \
do { \
int _exp = (int)(expected); \
int _act = (int)(actual); \
if (_exp != _act) { \
printf("  [FAIL] %s (Line %d)\n", #func_name, __LINE__); \
printf("    Expected: %d\n", _exp); \
printf("    Actual:   %d\n", _act); \
return 1; \
} \
} while (0)

/**
 * ASSERT_UINT64_EQ: Compares two Bitboards (uint64_t).
 * Prints in Hex format with ULL suffix for easy copy-pasting.
 */
#define ASSERT_UINT64_EQ(expected, actual, func_name) \
do { \
uint64_t _exp = (uint64_t)(expected); \
uint64_t _act = (uint64_t)(actual); \
if (_exp != _act) { \
printf("  [FAIL] %s (Line %d)\n", #func_name, __LINE__); \
printf("    Expected: 0x%016" PRIX64 "ULL\n", _exp); \
printf("    Actual:   0x%016" PRIX64 "ULL\n", _act); \
return 1; \
} \
} while (0)

#endif

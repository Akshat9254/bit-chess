#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

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

/**
 * ASSERT_STR_EQ: Compares two strings.
 */
#define ASSERT_STR_EQ(expected, actual, func_name) \
do { \
const char* _exp = (const char*)(expected); \
const char* _act = (const char*)(actual); \
if (_exp == NULL || _act == NULL) { \
printf("  [FAIL] %s (Line %d): NULL pointer detected\n", #func_name, __LINE__); \
return 1; \
} \
if (strcmp(_exp, _act) != 0) { \
printf("  [FAIL] %s (Line %d)\n", #func_name, __LINE__); \
printf("    Expected: \"%s\"\n", _exp); \
printf("    Actual:   \"%s\"\n", _act); \
return 1; \
} \
} while (0)

/**
 * ASSERT_TRUE: Compares two booleans
 */
#define ASSERT_BOOL_EQ(expected, actual, func_name) \
do { \
const bool _exp = (bool) (expected); \
const bool _act = (bool) (actual); \
if (_exp != _act) { \
printf("  [FAIL] %s (Line %d)\n", #func_name, __LINE__); \
printf("    Expected: \"%s\"\n", (_exp) ? "true" : "false"); \
printf("    Actual:   \"%s\"\n", (_act) ? "true": "false"); \
return 1; \
} \
} while (0)

#endif

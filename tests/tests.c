/*
 Copyright (c) 2019 Krzysztof Gabis
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define KGFLAGS_IMPLEMENTATION
#include "../kgflags.h"

#define TEST(DESC, A) printf("%4d: %-72s-", __LINE__, DESC);\
if(A){puts(" OK");tests_passed++;}\
else{puts(" FAIL");tests_failed++;}
#define STREQ(A, B) ((A) && (B) ? strcmp((A), (B)) == 0 : 0)
#define EPSILON 0.000001
#define DBLEQ(a, b) (fabs((a) - (b)) < EPSILON)
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(*array))

static void test_suite_expected(void);
static void test_suite_uncommon(void);
static void test_suite_errors(void);
static void test_suite_int(void);
static void test_suite_double(void);

static bool test_kgflags_contains_error(_kgflags_error_kind_t kind);
static void test_kgflags_reset(void);

static int tests_passed;
static int tests_failed;

int main() {
    test_suite_expected();
    test_suite_uncommon();
    test_suite_errors();
    test_suite_int();
    test_suite_double();
    printf("Tests failed: %d\n", tests_failed);
    printf("Tests passed: %d\n", tests_passed);
    return tests_failed;
}

static void test_suite_expected() {
    test_kgflags_reset();

    char *argv[] = {
        "app",
        "non-flag-argument-0",
        "--string", "lorem ipsum",
        "--bool",
        "--int", "123",
        "non-flag-argument-1",
        "--double", "123.3",
        "non-flag-argument-2",
        "--optional-assigned", "lorem ipsum",
        "--string-array", "ala", "ma", "kota",
        "--int-array", "1", "2", "3",
        "--double-array", "1.23", "2.34", "3.45",
    };

    const char *stringval = NULL;
    kgflags_string("string", "lorem", "String flag.", true, &stringval);

    bool boolval = false;
    kgflags_bool("bool", false, "Boolean flag.", true, &boolval);

    int intval = 0;
    kgflags_int("int", 0, "Integer flag.", true, &intval);

    double doubleval = 0;
    kgflags_double("double", 0.0, "Double flag.", true, &doubleval);

    kgflags_string_array_t string_arr;
    kgflags_string_array("string-array", "String array flag.", true, &string_arr);

    kgflags_int_array_t int_arr;
    kgflags_int_array("int-array", "Int array flag.", true, &int_arr);

    kgflags_double_array_t double_arr;
    kgflags_double_array("double-array", "Double array flag.", true, &double_arr);

    const char* optionalval = NULL;
    kgflags_string("optional", "lorem", "Optional flag.", false, &optionalval);

    const char* optionalval_assigned = NULL;
    kgflags_string("optional-assigned", NULL, "Optional flag (assigned).", false, &optionalval_assigned);

    bool ok = kgflags_parse(ARRAY_SIZE(argv), argv);

    TEST("Parsing succeeded", ok);

    if (!ok) {
        return;
    }

    TEST("String argument", STREQ(stringval, "lorem ipsum"));
    TEST("Optional string argument", STREQ(optionalval, "lorem"));
    TEST("Bool argument", boolval == true);
    TEST("Int argument", intval == 123);
    TEST("Double argument", DBLEQ(doubleval, 123.3));
    TEST("Optional assigned", STREQ(optionalval_assigned, "lorem ipsum"));

    TEST("String array count", kgflags_get_string_array_count(&string_arr) == 3);
    if (kgflags_get_string_array_count(&string_arr) == 3) {
        TEST("Array argument [0]", kgflags_get_string_array_item(&string_arr, 0) == "ala");
        TEST("Array argument [1]", kgflags_get_string_array_item(&string_arr, 1) == "ma");
        TEST("Array argument [2]", kgflags_get_string_array_item(&string_arr, 2) == "kota");
    }

    TEST("Int array count", kgflags_get_int_array_count(&int_arr) == 3);
    if (kgflags_get_int_array_count(&int_arr) == 3) {
        TEST("Array argument [0]", kgflags_get_int_array_item(&int_arr, 0) == 1);
        TEST("Array argument [1]", kgflags_get_int_array_item(&int_arr, 1) == 2);
        TEST("Array argument [2]", kgflags_get_int_array_item(&int_arr, 2) == 3);
    }

    TEST("Double array count", kgflags_get_double_array_count(&double_arr) == 3);
    if (kgflags_get_double_array_count(&double_arr) == 3) {
        TEST("Array argument [0]", DBLEQ(kgflags_get_double_array_item(&double_arr, 0), 1.23));
        TEST("Array argument [1]", DBLEQ(kgflags_get_double_array_item(&double_arr, 1), 2.34));
        TEST("Array argument [2]", DBLEQ(kgflags_get_double_array_item(&double_arr, 2), 3.45));
    }

    int non_flag_count = kgflags_get_non_flag_args_count();
    TEST("Non-flag args count", non_flag_count == 3);
    if (non_flag_count == 3) {
        TEST("Non-flag [0]", STREQ(kgflags_get_non_flag_arg(0), "non-flag-argument-0"));
        TEST("Non-flag [1]", STREQ(kgflags_get_non_flag_arg(1), "non-flag-argument-1"));
        TEST("Non-flag [2]", STREQ(kgflags_get_non_flag_arg(2), "non-flag-argument-2"));
    }
}

static void test_suite_uncommon() {
    {
        test_kgflags_reset();
        char *argv[] = { "", };
        TEST("No values", kgflags_parse(ARRAY_SIZE(argv), argv));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", };
        const char* optionalval = NULL;
        kgflags_string("optional", NULL, NULL, false, &optionalval);
        TEST("No values, only optional", kgflags_parse(ARRAY_SIZE(argv), argv));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--string", "--val"};
        const char *str = NULL;
        kgflags_string("string", NULL, NULL, true, &str);
        TEST("Value with prefix", kgflags_parse(ARRAY_SIZE(argv), argv));
        TEST("Value is --val", STREQ(str, "--val"));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--", "val"};
        const char *str = NULL;
        kgflags_string("", NULL, NULL, true, &str);
        TEST("Empty flag name", kgflags_parse(ARRAY_SIZE(argv), argv));
        TEST("Value is val", STREQ(str, "val"));
    }
}

static void test_suite_errors() {
    {
        test_kgflags_reset();
        char *argv[] = { "", "--string"};
        const char *str = NULL;
        kgflags_string("string", NULL, NULL, true, &str);
        TEST("Missing value (string)", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_MISSING_VALUE set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_MISSING_VALUE));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--intval"};
        int intval = 0;
        kgflags_int("intval", 0, NULL, true, &intval);
        TEST("Missing value (int)", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_MISSING_VALUE set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_MISSING_VALUE));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--double"};
        double dblval = 0.0;
        kgflags_double("double", 0.0, NULL, true, &dblval);
        TEST("Missing value (double)", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_MISSING_VALUE set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_MISSING_VALUE));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--unknown", "val"};
        TEST("Unknown flag", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_UNKNOWN_FLAG set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_UNKNOWN_FLAG));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "" };
        double dblval = 0.0;
        kgflags_double("non-flag", 0.0, NULL, true, &dblval);
        TEST("Non-flag flag", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_UNASSIGNED_FLAG set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_UNASSIGNED_FLAG));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--invalid-int", "abc" };
        int intval = 0;
        kgflags_int("invalid-int", 0.0, NULL, true, &intval);
        TEST("Invalid int", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_INVALID_INT set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_INVALID_INT));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--invalid-int", "123.3" };
        int intval = 0;
        kgflags_int("invalid-int", 0.0, NULL, true, &intval);
        TEST("Invalid int (double)", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_INVALID_INT set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_INVALID_INT));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--invalid-int", "1", "2", "abc" };
        kgflags_int_array_t arr;
        kgflags_int_array("invalid-int", NULL, true, &arr);
        TEST("Invalid int in array", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_INVALID_INT set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_INVALID_INT));
        TEST("Array count == 0", kgflags_get_int_array_count(&arr) == 0);
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--invalid-double", "abc" };
        double dblval = 0.0;
        kgflags_double("invalid-double", 0.0, NULL, true, &dblval);
        TEST("Invalid double", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_INVALID_DOUBLE set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_INVALID_DOUBLE));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--invalid-double", "1.23", "2.34", "abc" };
        kgflags_double_array_t arr;
        kgflags_double_array("invalid-double", NULL, true, &arr);
        TEST("Invalid double in array", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_INVALID_DOUBLE set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_INVALID_DOUBLE));
        TEST("Array count == 0", kgflags_get_double_array_count(&arr) == 0);
    }

    {
        test_kgflags_reset();
        char *argv[] = { "" };
        for (int i = 0; i < (KGFLAGS_MAX_FLAGS + 1); i++) {
            char *buf = (char*)malloc(256);
            int *intval = (int*)malloc(sizeof(int));
            sprintf(buf, "flag-%d", i);
            kgflags_int(buf, 0.0, NULL, true, intval);
        }
        TEST("Too many flags", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_TOO_MANY_FLAGS set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_TOO_MANY_FLAGS));
    }

    {
        test_kgflags_reset();
        const char *str = NULL;
        char *argv[] = { "", "--string", "val1", "--string", "val2"};
        kgflags_string("string", NULL, NULL, true, &str);
        TEST("Multiple assignment", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_MULTIPLE_ASSIGNMENT set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_MULTIPLE_ASSIGNMENT));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "",};
        const char *strval1 = NULL, *strval2 = NULL;
        kgflags_string("string", NULL, NULL, true, &strval1);
        kgflags_string("string", NULL, NULL, true, &strval2);
        TEST("Duplicate flag", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_DUPLICATE_FLAG set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_DUPLICATE_FLAG));
    }
}

void test_suite_int() {
    {
        test_kgflags_reset();
        char *argv[] = { "", "--intval", "-123"};
        int intval = 0;
        kgflags_int("intval", 0, NULL, true, &intval);
        TEST("Negative value (int)", kgflags_parse(ARRAY_SIZE(argv), argv));
        TEST("intval == -123", intval == -123);
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--intval", "+123"};
        int intval = 0;
        kgflags_int("intval", 0, NULL, true, &intval);
        TEST("Positive value (int)", kgflags_parse(ARRAY_SIZE(argv), argv));
        TEST("intval == +123", intval == 123);
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--intval", "123a"};
        int intval = 0;
        kgflags_int("intval", 0, NULL, true, &intval);
        TEST("Invalid int value - extra character", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_INVALID_INT set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_INVALID_INT));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--intval", "abc"};
        int intval = 0;
        kgflags_int("intval", 0, NULL, true, &intval);
        TEST("Invalid int value - string", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_INVALID_INT set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_INVALID_INT));
    }

    {
        test_kgflags_reset();
        int intmax = INT_MAX ;
        char buf[256];
        sprintf(buf, "%d", intmax);
        char *argv[] = { "", "--intval", buf};
        int intval = 0;
        kgflags_int("intval", 0, NULL, true, &intval);
        TEST("INT_MAX", kgflags_parse(ARRAY_SIZE(argv), argv));
        TEST("intval == INT_MAX", intval == INT_MAX);
    }

    {
        test_kgflags_reset();
        unsigned int overflow = (unsigned int)INT_MAX + 1;
        char buf[256];
        sprintf(buf, "%ud", overflow);
        char *argv[] = { "", "--intval", buf};
        int intval = 0;
        kgflags_int("intval", 0, NULL, true, &intval);
        TEST("INT_MAX + 1", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_INVALID_INT set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_INVALID_INT));
    }

    {
        test_kgflags_reset();
        int intmin = INT_MIN ;
        char buf[256];
        sprintf(buf, "%d", intmin);
        char *argv[] = { "", "--intval", buf};
        int intval = 0;
        kgflags_int("intval", 0, NULL, true, &intval);
        TEST("INT_MIN", kgflags_parse(ARRAY_SIZE(argv), argv));
        TEST("intval == INT_MIN", intval == INT_MIN);
    }

    {
        test_kgflags_reset();
        long long intmin = (long long)INT_MIN - 1;
        char buf[256];
        sprintf(buf, "%lld", intmin);
        char *argv[] = { "", "--intval", buf};
        int intval = 0;
        kgflags_int("intval", 0, NULL, true, &intval);
        TEST("INT_MIN - 1", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_INVALID_INT set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_INVALID_INT));
    }
}

static void test_suite_double() {
    {
        test_kgflags_reset();
        char *argv[] = { "", "--dblval", "+123.4"};
        double dblval = 0.0;
        kgflags_double("dblval", 0.0, NULL, true, &dblval);
        TEST("Positive value (double)", kgflags_parse(ARRAY_SIZE(argv), argv));
        TEST("dblval == +123.4", DBLEQ(dblval, +123.4));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--dblval", "-123.4"};
        double dblval = 0.0;
        kgflags_double("dblval", 0.0, NULL, true, &dblval);
        TEST("Negative value (double)", kgflags_parse(ARRAY_SIZE(argv), argv));
        TEST("dblval == -123.4", DBLEQ(dblval, -123.4));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--dblval", "123"};
        double dblval = 0.0;
        kgflags_double("dblval", 0.0, NULL, true, &dblval);
        TEST("Value without dot (double)", kgflags_parse(ARRAY_SIZE(argv), argv));
        TEST("dblval == 123", DBLEQ(dblval, 123));
    }

    {
        test_kgflags_reset();
        double dblmax = DBL_MAX;
        char buf[512];
        sprintf(buf, "%f", dblmax);
        char *argv[] = { "", "--dblval", buf};
        double dblval = 0.0;
        kgflags_double("dblval", 0.0, NULL, true, &dblval);
        TEST("DBL_MAX (double)", kgflags_parse(ARRAY_SIZE(argv), argv));
        TEST("dblval == DBL_MAX", DBLEQ(dblval, DBL_MAX));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--dblval", "NaN"};
        double dblval = 0.0;
        kgflags_double("dblval", 0.0, NULL, true, &dblval);
        TEST("NaN", kgflags_parse(ARRAY_SIZE(argv), argv));
        TEST("isnan", isnan(dblval));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--dblval", "Inf"};
        double dblval = 0.0;
        kgflags_double("dblval", 0.0, NULL, true, &dblval);
        TEST("Inf", kgflags_parse(ARRAY_SIZE(argv), argv));
        TEST("isinf", isinf(dblval));
    }

    {
        test_kgflags_reset();
        char *argv[] = { "", "--dblval", "-Inf"};
        double dblval = 0.0;
        kgflags_double("dblval", 0.0, NULL, true, &dblval);
        TEST("-Inf", kgflags_parse(ARRAY_SIZE(argv), argv));
        TEST("isinf", isinf(dblval));
    }

    {
        test_kgflags_reset();
        char buf[256];
        sprintf(buf, "%f", NAN);
        char *argv[] = { "", "--dblval", "123.4a"};
        double dblval = 0.0;
        kgflags_double("dblval", 0.0, NULL, true, &dblval);
        TEST("Invalid double", kgflags_parse(ARRAY_SIZE(argv), argv) == false);
        TEST("Errors count == 1", _kgflags_g.errors_count == 1);
        TEST("KGFLAGS_ERROR_KIND_INVALID_DOUBLE set", test_kgflags_contains_error(KGFLAGS_ERROR_KIND_INVALID_DOUBLE));
    }
}

static bool test_kgflags_contains_error(_kgflags_error_kind_t kind) {
    for (int i = 0; i < _kgflags_g.errors_count; i++) {
        _kgflags_error_t *err = &_kgflags_g.errors[i];
        if (err->kind == kind) {
            return true;
        }
    }
    return false;
}

static void test_kgflags_reset() {
    memset(&_kgflags_g, 0, sizeof(_kgflags_g));
}

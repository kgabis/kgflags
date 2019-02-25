#define KGFLAGS_IMPLEMENTATION
#include "../kgflags.h"

int main(int argc, char **argv) {
    const char *string_val = NULL;
    kgflags_string("string", "lorem", "String flag.", true, &string_val);

    bool bool_val = false;
    kgflags_bool("bool", false, "Boolean flag.", true, &bool_val);

    int int_val = 0;
    kgflags_int("int", 0, "Integer flag.", true, &int_val);

    double double_val = 0;
    kgflags_double("double", 0.0, "Double flag.", true, &double_val);

    kgflags_string_array_t string_arr;
    kgflags_string_array("string-array", "String array flag.", true, &string_arr);

    kgflags_int_array_t int_arr;
    kgflags_int_array("int-array", "Int array flag.", true, &int_arr);

    kgflags_double_array_t double_arr;
    kgflags_double_array("double-array", "Double array flag.", true, &double_arr);

    kgflags_set_custom_description("Usage of fullapi [-FLAGS]:");
    kgflags_set_prefix("-");
    if (!kgflags_parse(argc, argv)) {
        kgflags_print_errors();
        kgflags_print_usage();
        return 1;
    }

    printf("string_val: %s\n", string_val);
    printf("bool_val: %s\n", bool_val ? "true" : "false");
    printf("int_val: %d\n", int_val);
    printf("double_val: %f\n", double_val);

    printf("string-arr count: %d\n", kgflags_string_array_get_count(&string_arr));
    for (int i = 0; i < kgflags_string_array_get_count(&string_arr); i++) {
        printf("string-arr [%d]\n: %s", i, kgflags_string_array_get_item(&string_arr, i));
    }

    printf("int-arr count: %d\n", kgflags_int_array_get_count(&int_arr));
    for (int i = 0; i < kgflags_int_array_get_count(&int_arr); i++) {
        printf("int-arr [%d]: %d\n", i, kgflags_int_array_get_item(&int_arr, i));
    }

    printf("double-arr count: %d\n", kgflags_double_array_get_count(&double_arr));
    for (int i = 0; i < kgflags_double_array_get_count(&double_arr); i++) {
        printf("double-arr [%d]: %1.4g\n", i, kgflags_double_array_get_item(&double_arr, i));
    }

    int non_flag_count = kgflags_get_non_flag_args_count();
    for (int i = 0; i < non_flag_count; i++) {
        printf("Non-flag arguments [%d] = %s\n", i, kgflags_get_non_flag_arg(i));
    }

    return 0;
}

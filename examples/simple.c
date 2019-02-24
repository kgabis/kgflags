#define KGFLAGS_IMPLEMENTATION
#include "../kgflags.h"

int main(int argc, char **argv) {
    const char *to_print = NULL;  // guaranteed to be assigned only if kgflags_parse succeeds
    kgflags_string("to-print", NULL, "String to print.", true, &to_print);
    if (!kgflags_parse(argc, argv)) {
        kgflags_print_errors();
        kgflags_print_usage();
        return 1;
    }
    puts(to_print);
    return 0;
}

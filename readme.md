## About
kgflags is an easy to use command-line flag parsing library.

## Features
* Header-only (just copy kgflags.h).
* Easy to use.
* MIT licensed.
* No dynamic allocations.
* It supports strings, booleans, integers, double precision floats, string arrays, integer arrays, and double arrays.
* Not relying on macros and non-standard features which makes it easy to debug and modify.
* Test suites and examples.

## Example
```c
// simple.c
#define KGFLAGS_IMPLEMENTATION
#include "kgflags.h"

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
```

```
$ gcc simple.c -o simple
$ ./simple --to-print "HELLO WORLD"
HELLO WORLD
```

More examples can be found in [examples directory](http://github.com/kgabis/kgflags/tree/master/examples).

## Installation
Run:
```
git clone https://github.com/kgabis/kgflags.git
```
and copy kgflags.h to you source code tree.

It behaves like most single header libraries - you have to declare KGFLAGS_IMPLEMENTATION in *one* C or C++ file *before* including it.

```c
#define KGFLAGS_IMPLEMENTATION
#include "kgflags.h"
```

You can also customize max number of supported arguments/flags/errors by redefining KGFLAGS_MAX_NON_FLAG_ARGS, KGFLAGS_MAX_FLAGS and KGFLAGS_MAX_ERRORS (*before* including kgflags.h).

## Testing
Run ```pushd tests; ./run_tests.sh; popd``` to compile and run tests.

## Limitations
* It relies on global variables which means it's not thread safe. This shouldn't be an issue since argument parsing is done only once during startup of the application.
* kgflags dosn't do any dynamic memory allocations. All string values returned are pointers to values given in argv array passed to ```kgflags_parse```. Same goes for default values, description strings and a prefix.

## Contributing

I will always merge *working* bug fixes. However, if you want to add something new to the API, please create an "issue" on github for this first so we can discuss if it should end up in the library before you start implementing it.  
Remember to follow code's style and write appropriate tests.

## Acknowledgements
Many thanks to [Mateusz Belicki](https://github.com/mbelicki/) for all his suggestions and help.  
kgflags is heavily inspired by Go's [flag package](https://golang.org/pkg/flag/).

## License
[The MIT License (MIT)](http://opensource.org/licenses/mit-license.php)
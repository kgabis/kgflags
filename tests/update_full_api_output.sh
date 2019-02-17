#!/bin/bash

# set -xe

CC="gcc"
CFLAGS="-O0 -g -Wall -Wextra -std=c99 -pedantic-errors"

echo "Compiling, running and saving output of ../examples/full_api.c with ${CC} ${CFLAGS}"
${CC} ${CFLAGS} ../examples/full_api.c -o full_api
"./full_api" -extra-flag 2> "full_api_output"
rm full_api
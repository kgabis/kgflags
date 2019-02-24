#!/bin/bash

# set -xe

TESTS_OK=true

OUTDIR="output"

function compile_and_run {
	COMPILER=$1
	FLAGS=$2
	CFILE="tests.c"
	OUTPUT="tests"

	echo "Compiling and running ${CFILE} with ${COMPILER} ${FLAGS}:"

	touch "${OUTDIR}/${OUTPUT}"
	${COMPILER} ${FLAGS} "${CFILE}" -o "${OUTDIR}/${OUTPUT}"
	"./${OUTDIR}/${OUTPUT}" > "${OUTDIR}/output_${COMPILER}"
	RES=$?

	if [ ${RES} != "0" ]; then
		echo " FAIL"
		cat "${OUTDIR}/output_${COMPILER}"
		TESTS_OK=false
	else
		echo "	OK (output in ${OUTDIR}/output_${COMPILER})"
	fi
}

if [ -d "${OUTDIR}" ]; then
	rm -r "${OUTDIR}"
fi

mkdir "${OUTDIR}"

CC="gcc"
CFLAGS="-O0 -g -Wall -Wextra -std=c99 -pedantic-errors"

CPPC="g++"
CPPFLAGS="-O0 -g -Wall -Wextra -x c++ -Wno-c++11-compat-deprecated-writable-strings"

compile_and_run ${CC} "${CFLAGS}"
compile_and_run ${CPPC} "${CPPFLAGS}"

for f in `ls ../examples/*.c`
do
	echo "Compiling ${f} with ${CC} ${CFLAGS}:"
	fname=`basename ${f}`
	${CC} ${CFLAGS} ${f} -o "${OUTDIR}/${fname}.out"
	RES=$?

	if [ ${RES} != "0" ]; then
		echo " FAIL"
		TESTS_OK=false
	else
		echo "	OK"
	fi
done

echo "Compiling, running and comparing output of ../examples/full_api.c with ${CC} ${CFLAGS}:"
${CC} ${CFLAGS} ../examples/full_api.c -o "${OUTDIR}/full_api"
"./${OUTDIR}/full_api" -extra-flag 2> "${OUTDIR}/full_api_output"
diff "${OUTDIR}/full_api_output" full_api_output
RES=$?

if [ ${RES} != "0" ]; then
	echo " FAIL"
	TESTS_OK=false
else
	echo "	OK"
fi

if [ "${TESTS_OK}" == true ]; then
	echo "ALL TESTS SUCCEEDED"
else
	echo "TESTS FAILED"
fi


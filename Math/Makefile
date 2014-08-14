CC = g++
CFLAGS = -fno-strict-aliasing -Wall -fabi-version=6 -D VUL_TEST
C98 = -std=c++98
C11 = -std=c++11 -D VUL_CPLUSPLUS11
AVX = -mavx -D VUL_AOSOA_AVX
SSE = -msse4 -D VUL_AOSOA_SSE
INC = -I.:./Tests
FILE = tests.cpp

setup:
	mkdir -p ./bin/
full:
	${CC} ${C11} ${AVX} ${SSE} ${INC} ${FILE} -o ./bin/test_full
avx:
	${CC} ${C11} ${AVX} ${INC} ${FILE} -o ./bin/test_avx
sse:
	${CC} ${C11} ${SSE} ${INC} ${FILE} -o ./bin/test_sse
pure:
	${CC} ${C11} ${INC} ${FILE} -o ./bin/test_pure
leg-full:
	${CC} ${C98} ${AVX} ${SSE} ${INC} ${FILE} -o ./bin/test_legacy_full
leg-avx:
	${CC} ${C98} ${AVX} ${INC} ${FILE} -o ./bin/test_legacy_avx
leg-sse:
	${CC} ${C98} ${SSE} ${INC} ${FILE} -o ./bin/test_legacy_sse
leg-pure:
	${CC} ${C98} ${INC} ${FILE} -o ./bin/test_legacy_pure

all: setup full sse avx pure leg-full leg-avx leg-sse leg-pure
	@echo "Done!"

test_pure: pure leg-pure
	./bin/test_pure;
	./bin/test_legacy_pure;

test_sse: sse leg-sse
	./bin/test_sse;
	./bin/test_legacy_sse;

test_avx: avx leg-avx
	./bin/test_avx;
	./bin/test_legacy_avx;
	
test_full: full leg-full
	./bin/test_full;
	./bin/test_legacy_full;

test_all: all test_pure test_sse test_avx test_full
	

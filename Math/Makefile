CC = clang++
CFLAGS= -stdlib=libstdc++ -fno-strict-aliasing -Wall -D VUL_TEST -Werror -Wno-error=shift-count-negative -Wno-unknown-pragmas -lm -g
C98 = -std=c++98
C11 = -std=c++11 -D VUL_CPLUSPLUS11
AVX = -mavx -D VUL_AOSOA_AVX
SSE = -msse4 -D VUL_AOSOA_SSE
NEON = -mfpu=neon -D VUL_AOSOA_NEON -mcpu=cortex-a15
INC = -I.:./Tests
FILE = tests.cpp

setup:
	mkdir -p ./bin/
full:
	${CC} ${CFLAGS} ${C11} ${NEON} ${AVX} ${SSE} ${INC} ${FILE} -o ./bin/test_full
avx:
	${CC} ${CFLAGS} ${C11} ${AVX} ${INC} ${FILE} -o ./bin/test_avx
sse:
	${CC} ${CFLAGS} ${C11} ${SSE} ${INC} ${FILE} -o ./bin/test_sse
neon:
	${CC} ${CFLAGS} ${C11} ${NEON} ${INC} ${FILE} -o ./bin/test_neon
pure:
	${CC} ${CFLAGS} ${C11} ${INC} ${FILE} -o ./bin/test_pure
leg-full:
	${CC} ${CFLAGS} ${C98} ${AVX} ${SSE} ${INC} ${FILE} -o ./bin/test_legacy_full
leg-avx:
	${CC} ${CFLAGS} ${C98} ${AVX} ${INC} ${FILE} -o ./bin/test_legacy_avx
leg-sse:
	${CC} ${CFLAGS} ${C98} ${SSE} ${INC} ${FILE} -o ./bin/test_legacy_sse
leg-neon:
	${CC} ${CFLAGS} ${C98} ${NEON} ${INC} ${FILE} -o ./bin/test_legacy_neon
leg-pure:
	${CC} ${CFLAGS} ${C98} ${INC} ${FILE} -o ./bin/test_legacy_pure

all: setup full neon sse avx pure leg-full leg-avx leg-sse leg-neon leg-pure
	@echo "Done!"

test_pure: pure leg-pure
	./bin/test_pure;
	./bin/test_legacy_pure;

test_neon: neon leg-neon
	./bin/test_neon;
	./bin/test_legacy_neon;

test_sse: sse leg-sse
	./bin/test_sse;
	./bin/test_legacy_sse;

test_avx: avx leg-avx
	./bin/test_avx;
	./bin/test_legacy_avx;
	
test_full: full leg-full
	./bin/test_full;
	./bin/test_legacy_full;

test_all: all test_pure test_neon test_sse test_avx test_full 
	

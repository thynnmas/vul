setup:
	mkdir ./bin/
full:
	g++ -std=c++11 -mavx -msse4 -Wall -I.:./Tests tests.cpp -o ./bin/test_full -D VUL_TEST -D VUL_CPLUSPLUS11 -D VUL_AOSOA_SSE -D VUL_AOSOA_AVX -fabi-version=6
avx:
	g++ -std=c++11 -mavx -Wall -I.:./Tests tests.cpp -o ./bin/test_avx -D VUL_TEST -D VUL_CPLUSPLUS11 -D VUL_AOSOA_AVX -fabi-version=6
sse:
	g++ -std=c++11 -msse4 -Wall -I.:./Tests tests.cpp -o ./bin/test_sse -D VUL_TEST -D VUL_CPLUSPLUS11 -D VUL_AOSOA_SSE -fabi-version=6
pure:
	g++ -std=c++11 -Wall -I.:./Tests tests.cpp -o ./bin/test_pure -D VUL_TEST -D VUL_CPLUSPLUS11 -fabi-version=6
leg-full:
	g++ -std=c++98 -mavx -msse4 -Wall -I.:./Tests tests.cpp -o ./bin/test_legacy_full -D VUL_TEST -D VUL_AOSOA_SSE -D VUL_AOSOA_AVX -fabi-version=6
leg-avx:
	g++ -std=c++98 -mavx -Wall -I.:./Tests tests.cpp -o ./bin/test_legacy_avx -D VUL_TEST -D VUL_AOSOA_AVX -fabi-version=6
leg-sse:
	g++ -std=c++98 -msse4 -Wall -I.:./Tests tests.cpp -o ./bin/test_legacy_sse -D VUL_TEST -D VUL_AOSOA_SSE -fabi-version=6
leg-pure:
	g++ -std=c++98 -Wall -I.:./Tests tests.cpp -o ./bin/test_legacy_pure -D VUL_TEST -fabi-version=6

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
	

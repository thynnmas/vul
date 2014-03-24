#ifdef VUL_TEST_C

#include "Tests/test_gl.h"


#include <stdio.h>

#define VUL_DEFINE
#include "vul_resizable_array.h"
#include "vul_sort.h"
#include "vul_timer.h"

int comp( const void *a, const void *b ) {
	int ia, ib;
	memcpy( &ia, a, sizeof( int ) );
	memcpy( &ib, b, sizeof( int ) );
	return ia - ib;
}

#define SIZE_1 10
#define SIZE_2 100
#define SIZE_3 1000
#define SIZE_4 10000
#define SIZE_5 100000
#define SIZE_6 1000000

void bench_sorts( )
{
	vul_timer_t clock;
	unsigned long long itime, stime, qtime, ttime;
	int *inta, *intb, *intc, *intd, *inte, *intf;

	srand( time( NULL ) );
	
	inta = new int[ SIZE_1 ];
	for( int i = 0; i < SIZE_1; ++i ) {
		inta[ i ] = ( rand( ) % RAND_MAX );
	}
	intb = new int[ SIZE_2 ];
	for( int i = 0; i < SIZE_2; ++i ) {
		intb[ i ] = rand( ) % RAND_MAX;
	}
	intc = new int[ SIZE_3 ];
	for( int i = 0; i < SIZE_3; ++i ) {
		intc[ i ] = rand( ) % RAND_MAX;
	}
	intd = new int[ SIZE_4 ];
	for( int i = 0; i < SIZE_4; ++i ) {
		intd[ i ] = rand( ) % RAND_MAX;
	}
	inte = new int[ SIZE_5 ];
	for( int i = 0; i < SIZE_5; ++i ) {
		inte[ i ] = rand( ) % RAND_MAX;
	}
	intf = new int[ SIZE_6 ];
	for( int i = 0; i < SIZE_6; ++i ) {
		intf[ i ] = rand( ) % RAND_MAX;
	}
	
	vul_vector_t *insertiona = vul_vector_create( sizeof( int ), SIZE_1 );
	vul_vector_t *insertionb = vul_vector_create( sizeof( int ), SIZE_2 );
	vul_vector_t *insertionc = vul_vector_create( sizeof( int ), SIZE_3 );
	vul_vector_t *insertiond = vul_vector_create( sizeof( int ), SIZE_4 );
	vul_vector_t *shella = vul_vector_create( sizeof( int ), SIZE_1 );
	vul_vector_t *shellb = vul_vector_create( sizeof( int ), SIZE_2 );
	vul_vector_t *shellc = vul_vector_create( sizeof( int ), SIZE_3 );
	vul_vector_t *shelld = vul_vector_create( sizeof( int ), SIZE_4 );
	vul_vector_t *shelle = vul_vector_create( sizeof( int ), SIZE_5 );
	vul_vector_t *shellf = vul_vector_create( sizeof( int ), SIZE_6 );
	vul_vector_t *quicka = vul_vector_create( sizeof( int ), SIZE_1 );
	vul_vector_t *quickb = vul_vector_create( sizeof( int ), SIZE_2 );
	vul_vector_t *quickc = vul_vector_create( sizeof( int ), SIZE_3 );
	vul_vector_t *quickd = vul_vector_create( sizeof( int ), SIZE_4 );
	vul_vector_t *quicke = vul_vector_create( sizeof( int ), SIZE_5 );
	vul_vector_t *quickf = vul_vector_create( sizeof( int ), SIZE_6 );
	vul_vector_t *thynna = vul_vector_create( sizeof( int ), SIZE_1 );
	vul_vector_t *thynnb = vul_vector_create( sizeof( int ), SIZE_2 );
	vul_vector_t *thynnc = vul_vector_create( sizeof( int ), SIZE_3 );
	vul_vector_t *thynnd = vul_vector_create( sizeof( int ), SIZE_4 );
	vul_vector_t *thynne = vul_vector_create( sizeof( int ), SIZE_5 );
	vul_vector_t *thynnf = vul_vector_create( sizeof( int ), SIZE_6 );
	
	vul_vector_copy( shella, 0, inta, SIZE_1 );
	vul_vector_copy( shellb, 0, intb, SIZE_2 );
	vul_vector_copy( shellc, 0, intc, SIZE_3 );
	vul_vector_copy( shelld, 0, intd, SIZE_4 );
	vul_vector_copy( shelle, 0, inte, SIZE_5 );
	vul_vector_copy( shellf, 0, intf, SIZE_6 );
	vul_vector_copy( insertiona, 0, inta, SIZE_1 );
	vul_vector_copy( insertionb, 0, intb, SIZE_2 );
	vul_vector_copy( insertionc, 0, intc, SIZE_3 );
	vul_vector_copy( insertiond, 0, intd, SIZE_4 );
	vul_vector_copy( quicka, 0, inta, SIZE_1 );
	vul_vector_copy( quickb, 0, intb, SIZE_2 );
	vul_vector_copy( quickc, 0, intc, SIZE_3 );
	vul_vector_copy( quickd, 0, intd, SIZE_4 );
	vul_vector_copy( quicke, 0, inte, SIZE_5 );
	vul_vector_copy( quickf, 0, intf, SIZE_6 );
	vul_vector_copy( thynna, 0, inta, SIZE_1 );
	vul_vector_copy( thynnb, 0, intb, SIZE_2 );
	vul_vector_copy( thynnc, 0, intc, SIZE_3 );
	vul_vector_copy( thynnd, 0, intd, SIZE_4 );
	vul_vector_copy( thynne, 0, inte, SIZE_5 );
	vul_vector_copy( thynnf, 0, intf, SIZE_6 );
	

	printf( "Sort\t|\tInsert\t|\tShell\t|\tQuick\t|\tThynn\n" );

	vul_timer_reset( &clock );
	vul_sort_vector_insertion( insertiona, &comp );
	itime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_shell( shella, &comp );
	stime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_quick( quicka, &comp );
	qtime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_thynn( thynna, &comp );
	ttime = vul_timer_get_millis_cpu( &clock );
	printf( "%d\t|\t%llu.%04llus\t|\t%llu.%04llus\t|\t%llu.%04llus\t|\t%llu.%04llus\n",
					SIZE_1,
					itime / 1000LL, itime % 1000LL,
					stime / 1000LL, stime % 1000LL,
					qtime / 1000LL, qtime % 1000LL,
					ttime / 1000LL, ttime % 1000LL );
	
	vul_timer_reset( &clock );
	vul_sort_vector_insertion( insertionb, &comp );
	itime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_shell( shellb, &comp );
	stime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_quick( quickb, &comp );
	qtime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_thynn( thynnb, &comp );
	ttime = vul_timer_get_millis_cpu( &clock );
	printf( "%d\t|\t%llu.%04llus\t|\t%llu.%04llus\t|\t%llu.%04llus\t|\t%llu.%04llus\n",
					SIZE_2,
					itime / 1000LL, itime % 1000LL,
					stime / 1000LL, stime % 1000LL,
					qtime / 1000LL, qtime % 1000LL,
					ttime / 1000LL, ttime % 1000LL );

	vul_timer_reset( &clock );
	vul_sort_vector_insertion( insertionc, &comp );
	itime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_shell( shellc, &comp );
	stime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_quick( quickc, &comp );
	qtime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_thynn( thynnc, &comp );
	ttime = vul_timer_get_millis_cpu( &clock );
	printf( "%d\t|\t%llu.%04llus\t|\t%llu.%04llus\t|\t%llu.%04llus\t|\t%llu.%04llus\n",
					SIZE_3,
					itime / 1000LL, itime % 1000LL,
					stime / 1000LL, stime % 1000LL,
					qtime / 1000LL, qtime % 1000LL,
					ttime / 1000LL, ttime % 1000LL );
	
	vul_timer_reset( &clock );
	vul_sort_vector_insertion( insertiond, &comp );
	itime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_shell( shelld, &comp );
	stime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_quick( quickd, &comp );
	qtime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_thynn( thynnd, &comp );
	ttime = vul_timer_get_millis_cpu( &clock );
	printf( "%d\t|\t%llu.%04llus\t|\t%llu.%04llus\t|\t%llu.%04llus\t|\t%llu.%04llus\n",
					SIZE_4,
					itime / 1000LL, itime % 1000LL,
					stime / 1000LL, stime % 1000LL,
					qtime / 1000LL, qtime % 1000LL,
					ttime / 1000LL, ttime % 1000LL );
	
	vul_timer_reset( &clock );
	vul_sort_vector_shell( shelle, &comp );
	stime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_quick( quicke, &comp );
	qtime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_thynn( thynne, &comp );
	ttime = vul_timer_get_millis_cpu( &clock );
	printf( "%d\t|\t--\t|\t%llu.%04llus\t|\t%llu.%04llus\t|\t%llu.%04llus\n",
					SIZE_5,
					stime / 1000LL, stime % 1000LL,
					qtime / 1000LL, qtime % 1000LL,
					ttime / 1000LL, ttime % 1000LL );
	
	vul_timer_reset( &clock );
	vul_sort_vector_shell( shellf, &comp );
	stime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_quick( quickf, &comp );
	qtime = vul_timer_get_millis_cpu( &clock );
	vul_timer_reset( &clock );
	vul_sort_vector_thynn( thynnf, &comp );
	ttime = vul_timer_get_millis_cpu( &clock );
	printf( "%d\t|\t--\t|\t%llu.%04llus\t|\t%llu.%04llus\t|\t%llu.%04llus\n",
					SIZE_6,
					stime / 1000LL, stime % 1000LL,
					qtime / 1000LL, qtime % 1000LL,
					ttime / 1000LL, ttime % 1000LL );

	delete [] inta;
	delete [] intb;
	delete [] intc;
	delete [] intd;
	delete [] inte;
	delete [] intf;

	printf( "Done" );
}

#define SIZE_MULT_INTS 16
#define SIZE_MULT_ITERATIONS 100000
void bench_sorts_multiple( )
{
	vul_timer_t clock;
	unsigned long long itime, stime;
	int **ints;

	srand( time( NULL ) );
	
	ints = new int*[ SIZE_MULT_ITERATIONS ];
	for( int i = 0; i < SIZE_MULT_ITERATIONS; ++i ) {
		ints[ i ] = new int[ SIZE_MULT_INTS ];
		for( int j = 0; j < SIZE_MULT_INTS; ++j ) {
			ints[ i ][ j ] = ( rand( ) % RAND_MAX );
		}
	}
	
	vul_vector_t **insertion = new vul_vector_t*[ SIZE_MULT_ITERATIONS ];
	vul_vector_t **shell= new vul_vector_t*[ SIZE_MULT_ITERATIONS ];
	for( int i = 0; i < SIZE_MULT_ITERATIONS; ++i ) {
		insertion[ i ] = vul_vector_create( sizeof( int ), SIZE_MULT_INTS );
		vul_vector_copy( insertion[ i ], 0, ints[ i ], SIZE_MULT_INTS );
		shell[ i ] = vul_vector_create( sizeof( int ), SIZE_MULT_INTS );
		vul_vector_copy( shell[ i ], 0, ints[ i ], SIZE_MULT_INTS );
	}
	
	
	vul_timer_reset( &clock );
	for( int i = 0; i < SIZE_MULT_ITERATIONS; ++i ) {
		vul_sort_vector_insertion( insertion[ i ], &comp );
	}
	itime = vul_timer_get_millis( &clock );
	vul_timer_reset( &clock );
	for( int i = 0; i < SIZE_MULT_ITERATIONS; ++i ) {
		vul_sort_vector_shell( shell[ i ], &comp );
	}
	stime = vul_timer_get_millis_cpu( &clock );
	printf( "Insertion\t|\t%llu.%04llus\t|\nShell\t|\t%llu.%04llus\n",
					itime / 1000LL, itime % 1000LL,
					stime / 1000LL, stime % 1000LL);
	
	for( int i = 0; i < SIZE_MULT_ITERATIONS; ++i ) {
		vul_vector_destroy( insertion[ i ] );
		vul_vector_destroy( shell[ i ] );

		delete [ ] ints[ i ];
	}
	delete [ ] insertion;
	delete [ ] shell;
	delete [ ] ints;

	printf( "Done" );
}

void test_sorts( )
{
	srand( time( NULL ) );
	
	int *inta = new int[ SIZE_3 ];
	for( int i = 0; i < SIZE_3; ++i ) {
		inta[ i ] = ( rand( ) % SIZE_3 );
	}
	vul_vector_t *shella = vul_vector_create( sizeof( int ), SIZE_3 );
	vul_vector_t *insertiona = vul_vector_create( sizeof( int ), SIZE_3 );
	vul_vector_t *quicka = vul_vector_create( sizeof( int ), SIZE_3 );
	vul_vector_t *thynna = vul_vector_create( sizeof( int ), SIZE_3 );

	vul_vector_copy( shella, 0, inta, SIZE_3 );
	vul_vector_copy( insertiona, 0, inta, SIZE_3 );
	vul_vector_copy( quicka, 0, inta, SIZE_3 );
	vul_vector_copy( thynna, 0, inta, SIZE_3 );
	
	vul_sort_vector_insertion( insertiona, &comp );
	vul_sort_vector_shell( shella, &comp );
	vul_sort_vector_quick( quicka, &comp );
	vul_sort_vector_thynn( thynna, &comp );
	

	int c = 1;
	printf( "index|\tshell\t|\tinsertion\t|\tquick\t|\tthynn\n" );
	for( int i = 0; i < SIZE_3; ++i ) {
		int ti, ts, tq, tt;
		memcpy( &ti, vul_vector_get( insertiona, i ), sizeof( int ) );
		memcpy( &ts, vul_vector_get( shella, i ), sizeof( int ) );
		memcpy( &tq, vul_vector_get( quicka, i ), sizeof( int ) );
		memcpy( &tt, vul_vector_get( thynna, i ), sizeof( int ) );
		if ( comp( vul_vector_get( shella, i ), vul_vector_get( insertiona, i ) ) != 0 
			 || comp( vul_vector_get( shella, i ), vul_vector_get( quicka, i ) ) != 0 
			 || comp( vul_vector_get( shella, i ), vul_vector_get( thynna, i ) ) != 0  ) {
			c = 0;
			printf( "%d\t|\t%d\t|\t%d\t|\t%d\t|\t%d\t|%s\n", i, ti, ts, tq, tt, "NO" );
		} else {
			printf( "%d\t|\t%d\t|\t%d\t|\t%d\t|\t%d\t|%s\n", i, ti, ts, tq, tt, "YES" );
		}
	}
	if ( c ) {
		printf( "WOHOO" );
	} else {
		printf( "NO" );
	}
}

#include "vul_compress.h"
void test_compression_rle( )
{
	const ui8_t *comp, *out;
	ui32_t len_in, len_comp, len_out;
	
	const char *strin = "WWWWWWWWWWWWBWWWWWWWWWWWWBBBWWWWWWWWWWWWWWWWWWWWWWWWBWWWWWWWWWWWWWW";
	len_in = strlen( strin );
	comp = vul_compress_rle( ( ui8_t* )strin, len_in, &len_comp );
	
	out = vul_decompress_rle( comp, len_comp, &len_out );
	printf( "Lengths, in (%d), comp(%d), ratio(%f), out(%d)\n", len_in, len_comp, (f32_t)len_comp / (f32_t)len_in, len_out );
	printf( "In:  %s\n", strin );
	printf( "Out: %s\n", (char*)out );
	printf( "compare: %d\n", strcmp(strin, ( char* )out ) );
}

int main( int argv, char **argc )
{
	//bench_sorts( );
	//bench_sorts_multiple( );
	
	//test_sorts( );

	//test_compression_rle( );


	vul_test_gl( );
		
	printf( "Done, no errors.\n" );

	return 0;
}

#endif
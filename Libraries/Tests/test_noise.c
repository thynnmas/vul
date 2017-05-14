#define VUL_DEFINE
#include "vul_noise.h"

int vul__test_noise_gaussian( )
{
	float *a = ( float* )malloc( sizeof( float ) * 256 * 256 );
	vul_noise_gaussian( a, 256 * 256 );

	FILE *f = fopen("noise_gaussian.ppm", "w");
	fprintf( f, "P3\n%d %d\n%d\n", 256, 256, 255 );
	for( int i = 0; i < 256 * 256; ++i ) {
		fprintf( f, "%d %d %d ", 
					( unsigned char )( a[ i ] * 255.f ),
					( unsigned char )( a[ i ] * 255.f ),
					( unsigned char )( a[ i ] * 255.f ) );
	}
	fclose( f );

   free( a );
	
	return 1;
}

int vul__test_noise_worley_2d( )
{
	float *a = ( float* )malloc( sizeof( float ) * 256 * 256 );
	int bands = 13;
	float weights[ 13 ] = {
		0, 0.000003, 0.000229, 0.005977,
		0.060598, 0.24173, 0.382925, 0.24173, 0.060598,
		0.005977, 0.000229, 0.000003, 0
	};
	for( int y = 0; y < 256; ++y ) {
		for( int x = 0; x < 256; ++x ) {
			a[ y * 256 + x ] = vul_noise_worley_2d( x / 32.f, y / 32.f, 13, weights );
		}
	}

	FILE *f = fopen("noise_worley_2d.ppm", "w");
	fprintf( f, "P3\n%d %d\n%d\n", 256, 256, 255 );
	for( int i = 0; i < 256 * 256; ++i ) {
		fprintf( f, "%d %d %d ", 
					( unsigned char )( a[ i ] * 255.f ),
					( unsigned char )( a[ i ] * 255.f ),
					( unsigned char )( a[ i ] * 255.f ) );
	}
	fclose( f );

   free( a );

	return 1;
}

int vul_test_noise( )
{
	return 1;
}

int main( int argc, char **argv )
{
   // @TODO(thynn): Use seeded rng to create the noise and check that we get
   // correct results!
	vul__test_noise_gaussian( );
	vul__test_noise_worley_2d( );
	
	return 0;
}

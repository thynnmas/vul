#include <stdio.h>
#include <malloc.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define VUL_DEFINE
#include "vul_timer.h"

#include <Eigen/SVD>

using namespace Eigen;

int main( int argc, char **argv )
{
   if( argc < 3 ) {
      printf( "Usage: %s [path to image] [rank to reconstruct from] [iterations (optional)]\n", argv[ 0 ] );
      exit(1);
   }

   int w, h, n;
   unsigned char *data = stbi_load( argv[ 1 ], &w, &h, &n, 0 );
   
   // @TODO(thynn): Don't average into grayscale, do something useful (or each channel separately)
	MatrixXf A = MatrixXf( h, w );
   for( int y = 0; y < h; ++y ) {
      for( int x = 0; x < w; ++x ) {
         float v = ( float )data[ ( y * w + x ) * n ] / 255.f;
         for( int c = 1; c < n; ++c ) {
            v += ( float )data[ ( y * w + x ) * n + c ] / 255.f;
         }
			A( y, x ) = v / ( float )n;
      }
   }

   char *eptr;
   int rank = strtol( argv[ 2 ], &eptr, 10 );
   int wrank = rank;

   printf( "Computing SVD of %s (%dx%d)\n", argv[ 1 ], w, h );
   vul_timer *t = vul_timer_create( );
	JacobiSVD< MatrixXf > svd( A, ComputeThinU | ComputeThinV );
   uint64_t mms = vul_timer_get_micros( t );
   printf( "Completed in %lu.%lus\n", mms / 1000000, mms % 1000000 );

	VectorXf sigma = svd.singularValues();
   for( int i = 0; i < svd.rank(); ++i ) {
      printf( "S[%d]: %f\n", i, sigma[i] );
   }

   printf( "Rank of decomposition %d, wanted at most %d\n", svd.rank(), wrank );

	MatrixXf B = svd.matrixU() * svd.singularValues().asDiagonal() * svd.matrixV().transpose();

   unsigned char *odata = ( unsigned char* )malloc( sizeof( unsigned char ) * w * h * 3 );
   for( int y = 0; y < h; ++y ) {
      for( int x = 0; x < w; ++x ) {
         float v = B( y, x ) * 255.f;
         v = v < 0.f   ? 0.f 
           : v > 255.f ? 255.f 
           : v;
         odata[ ( y * w + x ) * 3     ] = v;
         odata[ ( y * w + x ) * 3 + 1 ] = v;
         odata[ ( y * w + x ) * 3 + 2 ] = v;
      }
   }
   stbi_write_bmp( "out.bmp", w, h, 3, odata );
   printf( "Wrote output to out.bmp.\n");
   for( int y = 0; y < h; ++y ) {
      for( int x = 0; x < w; ++x ) {
         float v = A( y, x ) * 255.f;
         v = v < 0.f   ? 0.f 
           : v > 255.f ? 255.f 
           : v;
         odata[ ( y * w + x ) * 3     ] = v;
         odata[ ( y * w + x ) * 3 + 1 ] = v;
         odata[ ( y * w + x ) * 3 + 2 ] = v;
      }
   }
   stbi_write_bmp( "source.bmp", w, h, 3, odata );
   printf( "Wrote input to source.bmp.\n");
   
   free( odata );
   free( data );
}

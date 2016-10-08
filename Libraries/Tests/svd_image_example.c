#include <stdio.h>
#include <malloc.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define VUL_DEFINE
#define VUL_LINALG_ERROR_ASSERT
#include "vul_linalg.h"
#include "vul_timer.h"

int main( int argc, char **argv )
{
   if( argc < 3 ) {
      printf( "Usage: %s [path to image] [rank to reconstruct from] [iterations (optional)]\n", argv[ 0 ] );
      exit(1);
   }

   int w, h, n;
   unsigned char *data = stbi_load( argv[ 1 ], &w, &h, &n, 0 );
   
   float *A = ( float* )malloc( sizeof( float ) * w * h );
   // @TODO(thynn): Don't average into grayscale, do something useful (or each channel separately)
   for( int y = 0; y < h; ++y ) {
      for( int x = 0; x < w; ++x ) {
         float v = ( float )data[ ( y * w + x ) * n ] / 255.f;
         for( int c = 1; c < n; ++c ) {
            v += ( float )data[ ( y * w + x ) * n + c ] / 255.f;
         }
         A[ y * w + x ] = v / ( float )n;
      }
   }

   char *eptr;
   int rank = strtol( argv[ 2 ], &eptr, 10 );
   int wrank = rank;
   vul_linalg_svd_basis *res = ( vul_linalg_svd_basis* )malloc( sizeof( vul_linalg_svd_basis ) * ( w < h ? w : h ) );

   int iters = 32;
   if( argc > 3 ) {
      iters = strtol( argv[ 3 ], &eptr, 10 );
   }
   
   printf( "Computing SVD of %s (%d iterations)\n", argv[ 1 ], iters );
   vul_timer *t = vul_timer_create( );
   vul_linalg_svd_dense( res, &rank, A, w, h, iters, 1e-4 ); // @TODO(thynn): eps and iter should be parameters
   uint64_t mms = vul_timer_get_micros( t );
   printf( "Completed in %lu.%lus\n", mms / 1000000, mms % 1000000 );

   for( int i = 0; i < rank; ++i ) {
      printf( "S[%d]: %f, axis %d\n", i, res[ i ].sigma, res[ i ].axis );
   }

   printf( "Rank of decomposition %d, wanted at most %d\n", rank, wrank );

   float *B = ( float* )malloc( sizeof( float ) * w * h );
   vul_linalg_svd_basis_reconstruct_matrix( B, res, rank );

   unsigned char *odata = ( unsigned char* )malloc( sizeof( unsigned char ) * w * h * 3 );
   for( int y = 0; y < h; ++y ) {
      for( int x = 0; x < w; ++x ) {
         float v = B[ y * w + x ] * 255.f;
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
         float v = A[ y * w + x ] * 255.f;
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
   
   vul_linalg_svd_basis_destroy( res, rank );
   free( res );
   free( A );
   free( B );
   free( odata );
   free( data );
}

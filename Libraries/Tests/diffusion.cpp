#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#define VUL_DEFINE
#define VUL_LINALG_ERROR_ASSERT
#include "../vul_linalg.h"
#include "../vul_timer.h"

#ifdef VUL_LINALG_DOUBLE
#define vul_linalg_real double
#else
#define vul_linalg_real float
#endif

/*
 * Not all platforms have PI defined.
 */
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795028841971693993751058
#endif

#ifdef TFY_PLOT
#include <malloc.h>
#include <stb_truetype.h>
#ifdef NANOVG_GLEW
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <nanovg.h>
#include <nanovg_gl.h>
#include "../vul_gl.h"

#define PLOTTING_WINDOW_WIDTH 800
#define PLOTTING_WINDOW_HEIGHT 480

typedef struct plotting_ctx {
   GLFWwindow *window;
   NVGcontext *vg;
   vul_linalg_real plotting_height;
} plotting_ctx;

void plot_error_cb( int error, const char *desc )
{
   printf( "GLFW error %d: %s\n", error, desc );
}

plotting_ctx *plotting_init( )
{
   plotting_ctx *context;
   GLenum err;

   context = ( plotting_ctx* )malloc( sizeof( plotting_ctx ) );

   glfwSetErrorCallback( plot_error_cb );

   if( !glfwInit( ) ) {
      printf( "Failed to initialize plotting (GLFW)." );
      return NULL;
   }

#ifdef NANOVG_GLES2_IMPLEMENTATION
   glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
#else
   glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_API );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
   glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
   glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
   glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, 1 );
#endif
   glfwWindowHint( GLFW_SAMPLES, 4 );

   context->window = glfwCreateWindow( PLOTTING_WINDOW_WIDTH, PLOTTING_WINDOW_HEIGHT, "Diffusion", NULL, NULL );
   if( !context->window ) {
      glfwTerminate( );
      printf( "Failed to initialize plotting (Window).\n" );
      return NULL;
   }
   glfwMakeContextCurrent( context->window );
   glfwSwapInterval( 0 );

#ifdef NANOVG_GLEW
   glewExperimental = GL_TRUE;
   if( ( err = glewInit( ) ) != GLEW_OK ){
      printf( "Failed to initialize plotting (GLEW: %s).\n", glewGetErrorString( err ) );
      return NULL;
   }
   while( vul_gl_check_error_print( NULL ) )
      ; /* Glew likes to generate errors in experimental mode, just ignore them */
#endif

#ifdef NANOVG_GLES2_IMPLEMENTATION
   context->vg = nvgCreateGLES2( NVG_ANTIALIAS | NVG_STENCIL_STROKES );
#else
   context->vg = nvgCreateGL3( NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG );
#endif
   if( !context->vg ) {
      printf( "Failed to initialize plotting (NanoVG).\n" );
      return NULL;
   }
   
   if( nvgCreateFont( context->vg, "sans", "Roboto-Regular.ttf" ) == -1 ) {
      printf( "Failed to load font, will not label axes.\n" );
   }
   context->plotting_height = 1.0;

   return context;
}

void plotting_clean( plotting_ctx *ctx )
{
#ifdef NANOVG_GLES2_IMPLEMENTATION
   nvgDeleteGLES2( ctx->vg );
#else
   nvgDeleteGL3( ctx->vg );
#endif
   glfwTerminate( );
}

void plotting_vector( plotting_ctx *ctx, vul_linalg_vector *u, int n, vul_linalg_real dp, 
                      vul_linalg_real time, vul_linalg_vector *analytic )
{
   int i, j, idx, vx, vy;
   vul_linalg_real pos, px_ratio, base, range, fvx, max_diff, max_val;
   char gl_error_str[ 256 ], label[ 128 ];

   glViewport( 0, 0, PLOTTING_WINDOW_WIDTH, PLOTTING_WINDOW_HEIGHT );
   glClearColor( 0.f, 0.f, 0.f, 0.f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

   glfwGetFramebufferSize( ctx->window, &vx, &vy );
   px_ratio = ( vul_linalg_real )vx / ( vul_linalg_real )vy;
   nvgBeginFrame( ctx->vg, PLOTTING_WINDOW_WIDTH, PLOTTING_WINDOW_HEIGHT, px_ratio );

   /* Find maximum value */
   for( i = 0; i < n; ++i ) {
      max_val = fmax( vul_linalg_vector_get( u, i ), max_val );
   }

   /* Render background */
   nvgBeginPath( ctx->vg );
   nvgRect( ctx->vg, 16, 16, PLOTTING_WINDOW_WIDTH - 32, PLOTTING_WINDOW_HEIGHT - 32 );
   nvgFillColor( ctx->vg, nvgRGBA( 128, 128, 128, 255 ) );
   nvgFill( ctx->vg );

   /* Render the particle density */
   nvgBeginPath( ctx->vg );
   nvgMoveTo( ctx->vg, 16, PLOTTING_WINDOW_HEIGHT - 16 );
   vx = 16; vy = PLOTTING_WINDOW_HEIGHT - 16; fvx = vx;
   base = ( vul_linalg_real )( PLOTTING_WINDOW_HEIGHT - 16 );
   range = ( vul_linalg_real )( PLOTTING_WINDOW_HEIGHT - 32 );
   for( i = 0; i < n; ++i ) {
      vy = ( int )( base - range * ( vul_linalg_vector_get( u, i ) / fmax( max_val, ctx->plotting_height ) ) );
      nvgLineTo( ctx->vg, ( int )fvx, vy );
      fvx += ( vul_linalg_real )( PLOTTING_WINDOW_WIDTH - 32 ) / ( vul_linalg_real )( n - 1 );
   }
   nvgLineTo( ctx->vg, PLOTTING_WINDOW_WIDTH - 16, ( int )base );
   nvgFillColor( ctx->vg, nvgRGBA( 128, 0, 0, 128 ) );
   nvgFill( ctx->vg );

   /* Render the analytic solution if there is one */
   max_diff = 0.0;
   if( analytic ) {
      nvgBeginPath( ctx->vg );
      nvgMoveTo( ctx->vg, 16, PLOTTING_WINDOW_HEIGHT - 16 );
      vx = 16; vy = PLOTTING_WINDOW_HEIGHT - 16; fvx = vx;
      base = ( vul_linalg_real )( PLOTTING_WINDOW_HEIGHT - 16 );
      range = ( vul_linalg_real )( PLOTTING_WINDOW_HEIGHT - 32 );
      for( i = 0; i < n; ++i ) {
         vy = ( int )( base - range * ( vul_linalg_vector_get( analytic,i ) / fmax( max_val, ctx->plotting_height ) ) );
         nvgLineTo( ctx->vg, ( int )fvx, vy );
         fvx += ( vul_linalg_real )( PLOTTING_WINDOW_WIDTH - 32 ) / ( vul_linalg_real )( n - 1 );
      }
      nvgLineTo( ctx->vg, PLOTTING_WINDOW_WIDTH - 16, ( int )base );
      nvgFillColor( ctx->vg, nvgRGBA( 0, 128, 0, 128 ) );
      nvgFill( ctx->vg );

      /* Also render the difference between the solutions */
      nvgBeginPath( ctx->vg );
      nvgMoveTo( ctx->vg, 16, PLOTTING_WINDOW_HEIGHT - 16 );
      vx = 16; vy = PLOTTING_WINDOW_HEIGHT - 16; fvx = vx;
      base = ( vul_linalg_real )( PLOTTING_WINDOW_HEIGHT - 16 );
      range = ( vul_linalg_real )( PLOTTING_WINDOW_HEIGHT - 32 );
      for( i = 0; i < n; ++i ) {
         vul_linalg_real diff = abs( vul_linalg_vector_get( analytic, i ) - vul_linalg_vector_get( u, i ) );
         max_diff = fmax( diff, max_diff );
         vy = ( int )( base - range * ( diff * ctx->plotting_height ) );
         nvgLineTo( ctx->vg, ( int )fvx, vy );
         fvx += ( vul_linalg_real )( PLOTTING_WINDOW_WIDTH - 32 ) / ( vul_linalg_real )( n - 1 );
      }
      nvgLineTo( ctx->vg, PLOTTING_WINDOW_WIDTH - 16, ( int )base );
      nvgFillColor( ctx->vg, nvgRGBA( 0, 0, 128, 64 ) );
      nvgFill( ctx->vg );
   }

   nvgFontFace( ctx->vg, "sans" );
   nvgFontSize( ctx->vg, 16 );

   /* Draw lines at certain particle heights and label them */
   for( i = 1; i < 5; ++i ) {
      nvgBeginPath( ctx->vg );
      vy = i * ( ( vul_linalg_real )( PLOTTING_WINDOW_HEIGHT - 32 ) / 5 );
      nvgMoveTo( ctx->vg, 16, ( PLOTTING_WINDOW_HEIGHT - 16 ) - vy );
      nvgLineTo( ctx->vg, PLOTTING_WINDOW_WIDTH - 16, ( PLOTTING_WINDOW_HEIGHT - 16 ) - vy );
      nvgFillColor( ctx->vg, nvgRGBA( 0, 0, 128, 255 ) );
      nvgFill( ctx->vg );

      if( i > 0 ) {
         nvgTextAlign( ctx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP );
         nvgFillColor( ctx->vg, nvgRGBA( 128, 0, 0, 255 ) );
         sprintf( label, "%3.6f", ( vul_linalg_real )i * ( fmax( max_val, ctx->plotting_height ) / 5.0 ) );
         nvgText( ctx->vg, 20, ( PLOTTING_WINDOW_HEIGHT - 12 ) - vy, label, NULL );
         nvgTextAlign( ctx->vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP );
         nvgFillColor( ctx->vg, nvgRGBA( 0, 0, 128, 255 ) );
         sprintf( label, "%1.2f", ( vul_linalg_real )i * ( ctx->plotting_height / 5.0 ) );
         nvgText( ctx->vg, PLOTTING_WINDOW_WIDTH - 20, ( PLOTTING_WINDOW_HEIGHT - 12 ) - vy, label, NULL );
      }
   }

   /* And finally, some stats */
   nvgTextAlign( ctx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM );
   nvgFillColor( ctx->vg, nvgRGBA( 192, 192, 192, 255 ) );
   sprintf( label, "After %3.6f seconds", time );
   nvgText( ctx->vg, 0, PLOTTING_WINDOW_HEIGHT, label, NULL );
   vul_linalg_real sum = 0;
   for( int j = 0; j < n; ++j ) {
      sum += vul_linalg_vector_get( u, j );
   }
   nvgTextAlign( ctx->vg, NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM );
   nvgFillColor( ctx->vg, nvgRGBA( 192, 192, 192, 255 ) );
   vul_linalg_real err_pct = ( max_diff / max_val ) * 100.0;
   sprintf( label, "Total mass in scene: %3.6f, max error: %3.6f%%", sum, err_pct );
   nvgText( ctx->vg, PLOTTING_WINDOW_WIDTH, PLOTTING_WINDOW_HEIGHT, label, NULL );

   /* End frame and render */
   nvgEndFrame( ctx->vg );
   glfwSwapBuffers( ctx->window );
   glfwPollEvents( );
}
#include <nanovg.c> // To avoid linking...
#endif

typedef enum {
   BOUNDARY_UNBOUNDED,
   BOUNDARY_ABSORBING,
   BOUNDARY_REFLECTIVE,
   BOUNDARY_TYPE_Count
} boundary_type;

typedef enum {
   DIFFUSION_CONSTANT,
   DIFFUSION_LINEAR,
   DIFFUSION_STEP,
   DIFFUSION_NON_DIFFERENTIAL,
   DIFFUSION_Count
} diffusion_function;

/*
 * The Dirac Delta function. Every interval is 0 except the interval
 * containing x0, the value of which is 1 / dx due to the relation
 *    int_{x0-delta_x/2}^{x0+delta_x/2} dx delta(x - x0 ) = 1,
 *      ~= nabla_x delta_{nabla_x}(x0) = 1 
 *      -> delta_{nabla_x}(x0) = 1 / nabla_x
 * which is basically midpoint method to find the height.
 * If x0 is exactly on the border between two intervals, the upper
 * interval is chosen.
 */
void dirac_delta( vul_linalg_vector *u, vul_linalg_real x0, vul_linalg_real xmin, vul_linalg_real dx )
{
   vulb__sparse_vclear( u );
   
   int cell = ( int )( ( x0 - xmin ) / dx );
   vul_linalg_vector_insert( u, cell, 1.0 / dx );
}


static vul_linalg_matrix *A = 0, *B, *D, *D2;
static int *lu_indices = 0;
/*
 * The crank-nicolson scheme implementation.
 * u is output
 * u_last is input, data at previous step
 * diffusivity is a function that returns D given a position.
 * p_min, dp are minimum positions and position discretization
 * dt is the time discretization step
 */
void crank_nicolson( vul_linalg_vector *u, vul_linalg_vector *u_last, int n,
                     vul_linalg_real( *diffusivity )( vul_linalg_real pos ),
                     vul_linalg_real p_min, vul_linalg_real dp,
                     vul_linalg_real dt, boundary_type boundary )
{
   vul_linalg_vector *y, *x;
   int *indices;

   vul_linalg_real alpha = dt / ( 2.0 * pow( dp, 2 ) );

   // Construct A and B
   if( A == 0 ) {
      A = vul_linalg_matrix_create( 0, 0, 0, 0 );
      B = vul_linalg_matrix_create( 0, 0, 0, 0 );

      vul_linalg_real Dp, Dm;
      Dp = diffusivity( p_min + ( dp * 0.5 ) );
      Dm = diffusivity( p_min - ( dp * 0.5 ) );
      vul_linalg_matrix_insert( A, 0, 0, 1.0 + alpha * ( Dp + Dm ) );
      vul_linalg_matrix_insert( B, 0, 0, 1.0 - alpha * ( Dp + Dm ) );
      switch( boundary ) {
      case BOUNDARY_REFLECTIVE:
         vul_linalg_matrix_insert( A, 0, 1, -alpha * ( Dp + Dm ) );
         vul_linalg_matrix_insert( B, 0, 1,  alpha * ( Dp + Dm ) );
         break;
      case BOUNDARY_ABSORBING:
      case BOUNDARY_UNBOUNDED: // This could be the above as well, it doesn't matter
         vul_linalg_matrix_insert( A, 0, 1, -alpha * Dp );
         vul_linalg_matrix_insert( B, 0, 1,  alpha * Dp );
         break;
      default:
         assert( 0 ); // Invalid boundary type
         break;
      }
      for( int i = 1; i < n - 1; ++i ) {
         Dp = diffusivity( p_min + ( dp * ( vul_linalg_real )i ) + ( dp * 0.5 ) );
         Dm = diffusivity( p_min + ( dp * ( vul_linalg_real )i ) - ( dp * 0.5 ) );
         vul_linalg_matrix_insert( A, i, i - 1, -alpha * Dm );
         vul_linalg_matrix_insert( B, i, i - 1,  alpha * Dm );
         vul_linalg_matrix_insert( A, i, i, 1.0 + alpha * ( Dp + Dm ) );
         vul_linalg_matrix_insert( B, i, i, 1.0 - alpha * ( Dp + Dm ) );
         vul_linalg_matrix_insert( A, i, i + 1, -alpha * Dp );
         vul_linalg_matrix_insert( B, i, i + 1,  alpha * Dp );
      }
      Dp = diffusivity( p_min + ( dp * ( vul_linalg_real )n - 1 ) + ( dp * 0.5 ) );
      Dm = diffusivity( p_min + ( dp * ( vul_linalg_real )n - 1 ) - ( dp * 0.5 ) );
      switch( boundary ) {
      case BOUNDARY_REFLECTIVE:
         vul_linalg_matrix_insert( A, n - 1, n - 2, -alpha * ( Dp + Dm ) );
         vul_linalg_matrix_insert( B, n - 1, n - 2,  alpha * ( Dp + Dm ) );
         break;
      case BOUNDARY_ABSORBING:
      case BOUNDARY_UNBOUNDED: // This could be the above as well, it doesn't matter
         vul_linalg_matrix_insert( A, n - 1, n - 2, -alpha * Dm );
         vul_linalg_matrix_insert( B, n - 1, n - 2,  alpha * Dm );
         break;
      default:
         assert( 0 ); // Invalid boundary type
         break;
      }
      vul_linalg_matrix_insert( A, n - 1, n - 1, 1.0 + alpha * ( Dp + Dm ) );
      vul_linalg_matrix_insert( B, n - 1, n - 1, 1.0 - alpha * ( Dp + Dm ) );
      
      /* Decomposition */
      lu_indices = ( int* )malloc( sizeof( int ) * n );
      vul_timer *t = vul_timer_create( );
      vul_linalg_cholesky_decomposition_sparse( &D, &D2, A, n, n );
      uint64_t te = vul_timer_get_micros( t );
      vul_timer_destroy( t );
      printf( "Decomposition took %llu.%llums\n", te / 1000, te % 1000 );
   }

   /* Solving */
   y = vul_linalg_vector_create( 0, 0, 0 );
   vulb__sparse_mmul( y, B, u_last );
   x = vul_linalg_cholesky_solve_sparse( D, D2, A, u_last, y, n, n, 1024, 1e-8 );
   vulb__sparse_vcopy( u, x );

   vul_linalg_vector_destroy( y );
   vul_linalg_vector_destroy( x );
}

/*
 * A constant diffusion function (i.e. a diffusion costant).
 */
vul_linalg_real difu_const( vul_linalg_real pos )
{
   return ( vul_linalg_real )1.0;
}

/*
 * The lower part of the 2-step diffusion function 
 */
vul_linalg_real difu_step_negative( vul_linalg_real pos )
{
   return ( vul_linalg_real )0.2;
}

/*
 * The upper part of the 2-step diffusion function 
*/
vul_linalg_real difu_step_positive( vul_linalg_real pos )
{
   return ( vul_linalg_real )1.0;
}

/*
 * The generic 2-step diffusion function
 */
vul_linalg_real difu_step( vul_linalg_real pos )
{
   return pos >= ( vul_linalg_real )0.0
      ? difu_step_positive( pos )
      : difu_step_negative( pos );
}

/*
 * These are the [A, B] edges of the spatial domain,
 * and are only used with a linear diffusion function
 * to avoid extra parameters in the function.
 */
static vul_linalg_real difu_linear_p_min, difu_linear_p_max;
/*
 * An example of a continous, differentiable diffustion function
 */
vul_linalg_real difu_linear( vul_linalg_real pos )
{
   vul_linalg_real ret = 0.2 + ( pos - difu_linear_p_min ) * ( difu_linear_p_max - difu_linear_p_min );
   return ret;
}

/*
 * An example of a continous, non-differential diffusion function.
 */
vul_linalg_real difu_non_diff( vul_linalg_real pos )
{
   return 2.0 - abs( pos );
}

/*
 * Helper function for compute_analytic_step that calculates
 * the A term.
 */
vul_linalg_real analytic_step( vul_linalg_real pos, vul_linalg_real t, vul_linalg_real x0 )
{
   vul_linalg_real a_plus;
   vul_linalg_real dp, dm, rtd, pwr, epwr,
      one = ( vul_linalg_real )1.0,
      two = ( vul_linalg_real )2.0,
      four = ( vul_linalg_real )4.0;

   dp = difu_step_positive( pos );
   dm = difu_step_negative( pos );
   rtd = sqrt( dm / dp );
   pwr = ( ( dp - dm ) * pow( x0, 2 ) ) / ( four * dp * dm * t );
   epwr = rtd * exp( pwr );

   /* Calculate A+ */
   a_plus = two
         / ( one + erf( x0 / sqrt( four * dp * t ) ) 
          + epwr * ( 1 - erf( x0 / sqrt( four * dm * t ) ) ) );

   if( pos >= ( vul_linalg_real )0.0 ) {
      return a_plus;
   }

   /* Calculate A- */
   return a_plus * epwr;
}

/*
* Compute the analytical solution for a step diffusion function
* in an bounded system.
*/
void compute_analytic_step( vul_linalg_vector *u, int n, vul_linalg_real mass,
                        vul_linalg_real x0, vul_linalg_real dx, vul_linalg_real xmin,
                        vul_linalg_real t )
{
   if( t != 0 ) {
      for( int i = 0; i < n; ++i ){
         vul_linalg_real x = xmin + ( i * dx );
         vul_linalg_real d = difu_step( x );
         vul_linalg_real fdt = ( vul_linalg_real )4.0 * d * t;
         vul_linalg_real a = analytic_step( x, t, x0 );
         vul_linalg_real left = a / sqrt( fdt * M_PI );
         vul_linalg_real right = -pow( x - x0, 2 ) / fdt;
         vul_linalg_vector_insert( u, i, mass * left * exp( right ) );
      }
   } else {
      dirac_delta( u, x0, 0.0, dx );
   }
}
/*
* Compute the analytical solution for a diffusion constant
* in an bounded system.
*/
void compute_analytic_constant( vul_linalg_vector *u, int n, vul_linalg_real mass,
                        vul_linalg_real x0, vul_linalg_real dx, vul_linalg_real xmin,
                        vul_linalg_real t, boundary_type boundary )
{
   switch( boundary ) {
   case BOUNDARY_REFLECTIVE:
      {
         vul_linalg_real L = dx * n;
         for( int i = 0; i < n; ++i ){
            vul_linalg_real x = xmin + ( i * dx );
            vul_linalg_real n0 = ( vul_linalg_real )1.0 / L;
            vul_linalg_real n1toinf = 0.0;
            int n = 1;
            while( 1 ) {
               vul_linalg_real npi = ( vul_linalg_real )n * M_PI;
               vul_linalg_real old = n1toinf;
               n1toinf += exp( -pow( ( npi / L ), 2 ) * difu_const( x ) * t )
                  * ( ( vul_linalg_real )2.0 / L )
                  * cos( npi * ( ( x0 - xmin ) / L ) ) * cos( npi * ( ( x - xmin ) / L ) );
               if( old == n1toinf || t == 0 ) {
                  break;
               }
               ++n;
            }
            vul_linalg_vector_insert( u, i, n0 + n1toinf );
         }
      }
      break;
   case BOUNDARY_ABSORBING:
      {
         vul_linalg_real L = dx * n;
         for( int i = 0; i < n; ++i ){
            vul_linalg_real x = xmin + ( i * dx );
            vul_linalg_real n1toinf = 0.0;
            int n = 1;
            while( 1 ) {
               vul_linalg_real npi = ( vul_linalg_real )n * M_PI;
               vul_linalg_real old = n1toinf;
               n1toinf += exp( -pow( ( npi / L ), 2 ) * difu_const( x ) * t )
                  * ( ( vul_linalg_real )2.0 / L )
                  * sin( npi * ( ( x0 - xmin ) / L ) ) * sin( npi * ( ( x - xmin ) / L ) );
               if( old == n1toinf || t == 0 ) {
                  break;
               }
               ++n;
            }
            vul_linalg_vector_insert( u, i, n1toinf );
         }
      }
      break;
   case BOUNDARY_UNBOUNDED:
      for( int i = 0; i < n; ++i ){
         vul_linalg_real x = xmin + ( i * dx );
         vul_linalg_real fdt = 4.0 * difu_const( x ) * t;
         vul_linalg_vector_insert( u, i, ( mass / sqrt( fdt * M_PI ) ) * exp( -pow( x - x0, 2 ) / fdt ) );
      }
      break;
   default:
      assert( 0 ); // Not a valid boundary type
   }
}

/*
 * The number of cells to discretize the spatial domain into
 */
#define N 201
/* 
 * The number of iterations to discretize the time domain into
 */
#define ITER 5000
/*
 * The total duration to simulate.
 */
#define DURATION ((vul_linalg_real) 5.0)

/*
 * The lower end of the spatial domain.
 */
#define P_MIN ((vul_linalg_real) -2.0)

/*
 * The size of the spatial domain
 */
#define P_SIZE ((vul_linalg_real) 4.0)

/*
 * The initial point of the Dirac delta.
 */
#define P_0 ((vul_linalg_real)0.0)

/*
 * The boundary type
 */
#define BOUNDARY_TYPE BOUNDARY_REFLECTIVE

/*
 * The diffusion function type
 */
#define DIFFUSION_FUNCTION_TYPE DIFFUSION_CONSTANT

int main( int argc, char **argv )
{
   vul_linalg_vector *u, *u2, *a;
   vul_timer *t;
   uint64_t pre, elapsed;

   t = vul_timer_create( );

   u = vul_linalg_vector_create( 0, 0, 0 );
   u2 = vul_linalg_vector_create( 0, 0, 0 );
   a = vul_linalg_vector_create( 0, 0, 0 );

   vul_linalg_real dp = P_SIZE / ( vul_linalg_real )( N - 1 );
   vul_linalg_real dt = DURATION / ( vul_linalg_real )ITER;
   vul_linalg_real( *diffusion_func )( vul_linalg_real ) = difu_const; // Default to const
#ifdef TFY_PLOT
   plotting_ctx *ctx = plotting_init( );
#endif
   boundary_type boundary = BOUNDARY_TYPE;
   diffusion_function diff_type = DIFFUSION_FUNCTION_TYPE;
   bool calc_analytic = true;

   switch( diff_type ) {
   case DIFFUSION_LINEAR:
      diffusion_func = difu_linear;
      difu_linear_p_min = P_MIN;
      difu_linear_p_max = P_MIN + dp * ( N - 1 );
      calc_analytic = false;
      break;
   case DIFFUSION_STEP:
      diffusion_func = difu_step;
      break;
   case DIFFUSION_NON_DIFFERENTIAL:
      diffusion_func = difu_non_diff;
      calc_analytic = false;
      break;
   case DIFFUSION_CONSTANT:
   default:
      diffusion_func = difu_const;
      break;
   }

#ifdef TFY_PLOT
   while( !glfwWindowShouldClose( ctx->window ) ) { 
      ctx->plotting_height = 1.0;
#endif
      dirac_delta( u, P_0, P_MIN, dp );
#ifdef TFY_PLOT
      for( int i = 0; i < ITER && !glfwWindowShouldClose( ctx->window ); ++i ){
#else
      for( int i = 0; i < ITER; ++i ){
#endif
         vul_linalg_real time = ( vul_linalg_real )i * dt;
         switch( diff_type ) {
         case DIFFUSION_STEP:
            compute_analytic_step( a, N, 1.0, P_0, dp, P_MIN, time );
            break;
         case DIFFUSION_CONSTANT:
            compute_analytic_constant( a, N, 1.0, P_0, dp, P_MIN, time, boundary );
            break;
         case DIFFUSION_NON_DIFFERENTIAL:
         case DIFFUSION_LINEAR:
         default:
            // No analytical solution by default
            break;
         }
         /* Alternate between using u and u2 as "front buffers" and "back buffers" */
         if( ( i & 1 ) == 0 ) {
            pre = vul_timer_get_micros( t );
            crank_nicolson( u2, u, N, diffusion_func, P_MIN, dp, dt, boundary );
            elapsed = vul_timer_get_micros( t ) - pre;
#ifdef TFY_PLOT
            plotting_vector( ctx, u2, N, dp, time, calc_analytic ? a : NULL );
#endif
         } else {
            pre = vul_timer_get_micros( t );
            crank_nicolson( u, u2, N, diffusion_func, P_MIN, dp, dt, boundary );
            elapsed = vul_timer_get_micros( t ) - pre;
#ifdef TFY_PLOT
            plotting_vector( ctx, u, N, dp, time, calc_analytic ? a : NULL );
#endif
         }
         printf( "Iteration %d calculated in %llu.%llums\n", i, elapsed / 1000, elapsed % 1000 );
      }
      if( ( ITER & 1 ) != 0 ) {
         u = u2; // Make sure result is in u at the end
      }
#ifdef TFY_PLOT
      plotting_vector( ctx, u, N, dp, 1.0, NULL );
   }
#endif

#ifdef TFY_PLOT
   plotting_clean( ctx );
#endif
}

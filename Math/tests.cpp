/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * @TODO: Proper tests:
 * We try to test all categories for all cases, and all edge cases 
 * against static, precalculated results. In  addition, we use the
 * current system time as a seed and test a not insignificant number
 * of random cases against a reference implementation (glm as of now).
 * The hope is that everyone runs the tests a few runs before trusting 
 * our library and that we get good coverage this way.
 * If the fuzzing fails, the seed is provided in output; please supply
 * this with the bug report so we can reproduce.
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
 *   the MIT licence applies (see the LICENCE file)
 */

#ifdef VUL_TEST

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define VUL_DEFINE
#include "vul_types.hpp"
#include "vul_affine.hpp"
#include "vul_vector.hpp"
#include "vul_aabb.hpp"
#include "vul_point.hpp"
#include "vul_matrix.hpp"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace vul;

void test_half( )
{
	half f1a( 0.5f ), f1b( 1.7f ), f1c( -7.1f );
	float fa = 0.5f, fb = 1.7f, fc = -7.1f;

	printf( "Float: %f & %f (+: %f) (-: %f) (*: %f) (/: %f)\n", fa, fb, fa+fb, fa-fb, fa*fb, fa/fb );
	printf( "half:  %f & %f (+: %f) (-: %f) (*: %f) (/: %f)\n", (float)f1a, (float)f1b, (float)(f1a+f1b), (float)(f1a-f1b), (float)(f1a*f1b), (float)(f1a/f1b) );
	printf( "Float: (++: %f) (--: %f)  (u-: %f)\n", ++fa, --fb, -fc );
	printf( "half:  (++: %f) (--: %f)  (u-: %f)\n", (float)++f1a, (float)--f1b, (float)-f1c );
	printf( "Float: %f & %f (+: %f) (-: %f) (*: %f) (/: %f)\n", fb, fc, fb+fc, fb-fc, fb*fc, fb/fc );
	printf( "half:  %f & %f (+: %f) (-: %f) (*: %f) (/: %f)\n", (float)f1b, (float)f1c, (float)(f1b+f1c), (float)(f1b-f1c), (float)(f1b*f1c), (float)(f1b/f1c) );
	printf( "max (65504): %f, max+1 (65505): %f\n", (float)half(65504.f), (float)half(65505.f) );
	
	printf( "AOK\n\n" );
}

void test_fixed( )
{
	fixed_32< 8 > f1a( 1.5f ), f1b( 2.2f ), f1c( -7.1f );
	fixed_32< 10 > f2a( 1.5f ), f2b( 2.2f ), f2c( -7.1f );
	fixed_32< 16 > f3a( 1.5f ), f3b( 2.2f ), f3c( -7.1f );
	fixed_32< 24 > f4a( 1.5f ), f4b( 2.2f ), f4c( -7.1f );
	float fa = 1.5f, fb = 2.2f, fc = -7.1f;

	printf( "Float:      %f & %f (+: %f) (-: %f) (*: %f) (/: %f)\n", fa, fb, fa+fb, fa-fb, fa*fb, fa/fb );
	printf( "Fixed32_8:  %f & %f (+: %f) (-: %f) (*: %f) (/: %f)\n", (float)f1a, (float)f1b, (float)(f1a+f1b), (float)(f1a-f1b), (float)(f1a*f1b), (float)(f1a/f1b) );
	printf( "Fixed32_10: %f & %f (+: %f) (-: %f) (*: %f) (/: %f)\n", (float)f2a, (float)f2b, (float)(f2a+f2b), (float)(f2a-f2b), (float)(f2a*f2b), (float)(f2a/f2b) );
	printf( "Fixed32_16: %f & %f (+: %f) (-: %f) (*: %f) (/: %f)\n", (float)f3a, (float)f3b, (float)(f3a+f3b), (float)(f3a-f3b), (float)(f3a*f3b), (float)(f3a/f3b) );
	printf( "Fixed32_24: %f & %f (+: %f) (-: %f) (*: %f) (/: %f)\n", (float)f4a, (float)f4b, (float)(f4a+f4b), (float)(f4a-f4b), (float)(f4a*f4b), (float)(f4a/f4b) );
	printf( "Float:      (++: %f) (--: %f)  (u-: %f)\n", ++fa, --fb, -fc );
	printf( "Fixed32_8:  (++: %f) (--: %f)  (u-: %f)\n", (float)(++f1a), (float)(--f1b), (float)(-f1c) );
	printf( "Fixed32_10: (++: %f) (--: %f)  (u-: %f)\n", (float)(++f2a), (float)(--f2b), (float)(-f2c) );
	printf( "Fixed32_16: (++: %f) (--: %f)  (u-: %f)\n", (float)(++f3a), (float)(--f3b), (float)(-f3c) );
	printf( "Fixed32_24: (++: %f) (--: %f)  (u-: %f)\n", (float)(++f4a), (float)(--f4b), (float)(-f4c) );
	
	printf( "AOK\n\n" );
}

void test_vector( )
{
	Vector< f32_t, 1 > v1 = makeVector< f32_t, 1 >( 2.0f ), v1r = makeVector< f32_t, 1 >( );
	Vector< f32_t, 2 > v2 = makeVector< f32_t, 2 >( 2.0f ), v2r = makeVector< f32_t, 2 >( );
	Vector< f32_t, 3 > v3 = makeVector< f32_t, 3 >( 2.0f ), v3r = makeVector< f32_t, 3 >( );
	Vector< f32_t, 4 > v4 = makeVector< f32_t, 4 >( 2.0f ), v4r = makeVector< f32_t, 4 >( );
	Vector< f32_t, 5 > v5 = makeVector< f32_t, 5 >( 2.0f ), v5r = makeVector< f32_t, 5 >( );

	printf( "Vec1: %f\n", v1[ 0 ] );
	printf( "Vec2: %f %f\n", v2[ 0 ], v2[ 1 ] );
	printf( "Vec3: %f %f %f\n", v3[ 0 ], v3[ 1 ], v3[ 2 ] );
	printf( "Vec4: %f %f %f %f\n", v4[ 0 ], v4[ 1 ], v4[ 2 ], v4[ 3 ] );
	printf( "Vec5: %f %f %f %f %f\n", v5[ 0 ], v5[ 1 ], v5[ 2 ], v5[ 3 ], v5[ 4 ] );
	v1r = v1 + v1;
	v2r = v2 + v2;
	v3r = v3 + v3;
	v4r = v4 + v4;
	v5r = v5 + v5;
	printf( "Vec1+: %f\n", v1r[ 0 ] );
	printf( "Vec2+: %f %f\n", v2r[ 0 ], v2r[ 1 ] );
	printf( "Vec3+: %f %f %f\n", v3r[ 0 ], v3r[ 1 ], v3r[ 2 ] );
	printf( "Vec4+: %f %f %f %f\n", v4r[ 0 ], v4r[ 1 ], v4r[ 2 ], v4r[ 3 ] );
	printf( "Vec5+: %f %f %f %f %f\n", v5r[ 0 ], v5r[ 1 ], v5r[ 2 ], v5r[ 3 ], v5r[ 4 ] );
	v1r = v1 - v1;
	v2r = v2 - v2;
	v3r = v3 - v3;
	v4r = v4 - v4;
	v5r = v5 - v5;
	printf( "Vec1-: %f\n", v1r[ 0 ] );
	printf( "Vec2-: %f %f\n", v2r[ 0 ], v2r[ 1 ] );
	printf( "Vec3-: %f %f %f\n", v3r[ 0 ], v3r[ 1 ], v3r[ 2 ] );
	printf( "Vec4-: %f %f %f %f\n", v4r[ 0 ], v4r[ 1 ], v4r[ 2 ], v4r[ 3 ] );
	printf( "Vec5-: %f %f %f %f %f\n", v5r[ 0 ], v5r[ 1 ], v5r[ 2 ], v5r[ 3 ], v5r[ 4 ] );
	v1r = v1 * v1;
	v2r = v2 * v2;
	v3r = v3 * v3;
	v4r = v4 * v4;
	v5r = v5 * v5;
	printf( "Vec1*: %f\n", v1r[ 0 ] );
	printf( "Vec2*: %f %f\n", v2r[ 0 ], v2r[ 1 ] );
	printf( "Vec3*: %f %f %f\n", v3r[ 0 ], v3r[ 1 ], v3r[ 2 ] );
	printf( "Vec4*: %f %f %f %f\n", v4r[ 0 ], v4r[ 1 ], v4r[ 2 ], v4r[ 3 ] );
	printf( "Vec5*: %f %f %f %f %f\n", v5r[ 0 ], v5r[ 1 ], v5r[ 2 ], v5r[ 3 ], v5r[ 4 ] );
	v1r = v1 / v1;
	v2r = v2 / v2;
	v3r = v3 / v3;
	v4r = v4 / v4;
	v5r = v5 / v5;
	printf( "Vec1/: %f\n", v1r[ 0 ] );
	printf( "Vec2/: %f %f\n", v2r[ 0 ], v2r[ 1 ] );
	printf( "Vec3/: %f %f %f\n", v3r[ 0 ], v3r[ 1 ], v3r[ 2 ] );
	printf( "Vec4/: %f %f %f %f\n", v4r[ 0 ], v4r[ 1 ], v4r[ 2 ], v4r[ 3 ] );
	printf( "Vec5/: %f %f %f %f %f\n", v5r[ 0 ], v5r[ 1 ], v5r[ 2 ], v5r[ 3 ], v5r[ 4 ] );
	
	f32_t a[ 3 ] = { 2.f, 0.f, 0.f }, b[ 3 ] = { 0.f, 2.f, 0.f };
	Vector< f32_t, 3 > va = makeVector< f32_t, 3 >( a );
	Vector< f32_t, 3 > vb = makeVector< f32_t, 3 >( b );
	Vector< f32_t, 3 > vc = makeVector< f32_t, 3 >( 0.f );
	Vector< f32_t, 3 > vr = makeVector< f32_t, 3 >( 0.f );
	vr = cross( va, vb );
	printf( "cross: <%f, \t%f, \t%f>\n"
			"      x<%f, \t%f, \t%f>\n"
			"      =<%f, \t%f, \t%f>\n", va[ 0 ], va[ 1 ], va[ 2 ], vb[ 0 ], vb[ 1 ], vb[ 2 ], vr[ 0 ], vr[ 1 ], vr[ 2 ] );
	printf( "dotself: %f, norm: %f\n", dot( vr, vr ), norm( vr ) );
	vr = normalize( vr );
	printf( "normalized: <%f, %f, %f>\n", vr[ 0 ], vr[ 1 ], vr[ 2 ] );
	va[ 0 ] = 1.f;
	va[ 1 ] = 2.f;
	va[ 2 ] = 3.f;
	vr = min( va, 1.8f );
	printf( "min: <1, 2, 3>,1.8: <%f, %f, %f>\n", vr[ 0 ], vr[ 1 ], vr[ 2 ] );
	vr = max( va, 1.8f );
	printf( "max: <1, 2, 3>,1.8: <%f, %f, %f>\n", vr[ 0 ], vr[ 1 ], vr[ 2 ] );
	va[ 0 ] = 1.f;
	va[ 1 ] = -2.f;
	va[ 2 ] = -3.f;
	vr = abs( va );
	printf( "abs: <1, -2, -3>: <%f, %f, %f>\n", vr[ 0 ], vr[ 1 ], vr[ 2 ] );
	vr = clamp( va, 0.f, 1.f );
	printf( "clamp: <1, -2, -3>, [0,1]: <%f, %f, %f>\n", vr[ 0 ], vr[ 1 ], vr[ 2 ] );
	vr = saturate( va );
	printf( "saturate: <1, -2, -3>: <%f, %f, %f>\n", vr[ 0 ], vr[ 1 ], vr[ 2 ] );
	vb[ 0 ] = vb[ 1 ] = vb[ 2 ] = 1.f;
	vr = lerp( vc, vb, 0.5f );
	printf( "lerp (0.5): <%f, %f, %f>\n", vr[ 0 ], vr[ 1 ], vr[ 2 ] );
	printf( "mincomp <1, -2, -3>: %f\nmaxcomp <1, -2, -3>: %f\n", minComponent( va ), maxComponent( va ) );
}

void test_aabb( )
{
#ifdef VUL_CPLUSPLUS11
	Point< f32_t, 3 > p1( -1.f ), p2( 1.f );
	AABB< f32_t, 3 > aabb( p1, p2 );
	Vector< f32_t, 3 > v( 1.f );
#else
	Vector< f32_t, 3 > p1 = makeVector< f32_t, 3 >( -1.f ), p2 = makeVector< f32_t, 3 >( 1.f );
	AABB< f32_t, 3 > aabb = makeAABB< f32_t, 3 >( p1, p2 );
	Vector< f32_t, 3 > v = makeVector< f32_t, 3 >( 1.f );

	Matrix< f32_t, 3, 3 > m = makeMatrix< f32_t, 3, 3 >( );
	Affine< f32_t, 3 > a = makeAffine< f32_t, 3 >( m, -v );
#endif

	printf( "(%f, %f, %f) x (%f, %f, %f )\n", aabb._min[ 0 ], aabb._min[ 1 ], aabb._min[ 2 ],
											  aabb._max[ 0 ], aabb._max[ 1 ], aabb._max[ 2 ] );
	aabb = scale( aabb, 2.f );
	printf( "(%f, %f, %f) x (%f, %f, %f )\n", aabb._min[ 0 ], aabb._min[ 1 ], aabb._min[ 2 ],
											  aabb._max[ 0 ], aabb._max[ 1 ], aabb._max[ 2 ] );
	aabb = translate( aabb, v );
	printf( "(%f, %f, %f) x (%f, %f, %f )\n", aabb._min[ 0 ], aabb._min[ 1 ], aabb._min[ 2 ],
											  aabb._max[ 0 ], aabb._max[ 1 ], aabb._max[ 2 ] );
	aabb = transform( aabb, a );
	printf( "(%f, %f, %f) x (%f, %f, %f )\n", aabb._min[ 0 ], aabb._min[ 1 ], aabb._min[ 2 ],
											  aabb._max[ 0 ], aabb._max[ 1 ], aabb._max[ 2 ] );
	printf( "AOK\n" );
}

void test_matrix( )
{
	f32_t f[ 9 ], f2[ 4 ];
	Vector< f32_t, 3 > c1, c2, c3, v, vr;
	Point< f32_t, 3 > p, pr;
	Matrix< f32_t, 3, 3 > mv, ma, mr, mf7;
	Matrix< f32_t, 2, 2 > m2;
	Matrix< bool, 3, 3 > mrb;

	for( int i = 0; i < 9; ++i ) {
		f[ i ] = 1.f - ( ( f32_t )rand() / ( f32_t )( RAND_MAX / 2 ) );
	}
	f2[ 0 ] = f2[ 3 ] = 1.f;
	f2[ 1 ] = f2[ 2 ] = 2.f;

#ifdef VUL_CPLUSPLUS11
	c1 = Vector< f32_t >( f[ 0 ], f[ 1 ], f[ 2 ] );
	c2 = Vector< f32_t >( f[ 3 ], f[ 4 ], f[ 5 ] );
	c3 = Vector< f32_t >( f[ 6 ], f[ 7 ], f[ 8 ] );
	v = Vector< f32_t >( 1.f, 2.f, 3.f );
	p = Point< f32_t >( 1.f, 2.f, 3.f );
	pr = Point< f32_t, 3 >(  );
	vr = Vector< f32_t, 3 >( );
#else
	c1 = makeVector< f32_t >( f[ 0 ], f[ 1 ], f[ 2 ] );
	c2 = makeVector< f32_t >( f[ 3 ], f[ 4 ], f[ 5 ] );
	c3 = makeVector< f32_t >( f[ 6 ], f[ 7 ], f[ 8 ] );
	v = makeVector< f32_t >( 1.f, 2.f, 3.f );
	p = makePoint< f32_t >( 1.f, 2.f, 3.f );
	pr = makePoint< f32_t, 3 >(  );
	vr = makeVector< f32_t, 3 >( );
#endif
	glm::mat3x3 gm = glm::make_mat3x3( f ), gr( 0.f );
	glm::mat2x2 g2m = glm::make_mat2x2( f2 );
	glm::vec3 gv( 1.f, 2.f, 3.f ), gvr( 0.f );

#ifdef VUL_CPLUSPLUS11
	mv = makeMatrix33FromColumns< f32_t >( c1, c2, c3 );
	ma = Matrix< f32_t, 3, 3 >( f );
	mf7 = Matrix< f32_t, 3, 3 >( );
	mrb = Matrix< bool, 3, 3 >( );
	m2 = Matrix< f32_t, 2, 2 >( f2 );
#else
	mv = makeMatrix33FromColumns< f32_t >( c1, c2, c3 );
	ma = makeMatrix< f32_t, 3, 3 >( f );
	mf7 = makeMatrix< f32_t, 3, 3 >( );
	mrb = makeMatrix< bool, 3, 3 >( );
	mr = makeMatrix< f32_t, 3, 3 >( );
	m2 = makeMatrix< f32_t, 2, 2 >( f2 );
#endif
	mf7( 2, 1 ) = f[ 7 ];

	// Print matrix
	printf( " Reference:\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n", gm[ 0 ][ 0 ], gm[ 1 ][ 0 ], gm[ 2 ][ 0 ],
							     gm[ 0 ][ 1 ], gm[ 1 ][ 1 ], gm[ 2 ][ 1 ],
							     gm[ 0 ][ 2 ], gm[ 1 ][ 2 ], gm[ 2 ][ 2 ] );
	printf( " Ours vec:\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n", mv( 0,  0 ), mv( 1,  0 ), mv( 2,  0 ),
							     mv( 0,  1 ), mv( 1,  1 ), mv( 2,  1 ),
							     mv( 0,  2 ), mv( 1,  2 ), mv( 2,  2 ) );
	printf( " Ours arr:\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n", ma( 0,  0 ), ma( 1,  0 ), ma( 2,  0 ),
							     ma( 0,  1 ), ma( 1,  1 ), ma( 2,  1 ),
							     ma( 0,  2 ), ma( 1,  2 ), ma( 2,  2 ) );

	// Test operations
	mrb = ma == mv;
	printf( " ==:\n"
			" [ %d, %d, %d ]\n"
			" [ %d, %d, %d ]\n"
			" [ %d, %d, %d ]\n", mrb( 0,  0 ), mrb( 1,  0 ), mrb( 2,  0 ),
							     mrb( 0,  1 ), mrb( 1,  1 ), mrb( 2,  1 ),
							     mrb( 0,  2 ), mrb( 1,  2 ), mrb( 2,  2 ) );
	mrb = ma != mv;
	printf( " !=:\n"
			" [ %d, %d, %d ]\n"
			" [ %d, %d, %d ]\n"
			" [ %d, %d, %d ]\n", mrb( 0,  0 ), mrb( 1,  0 ), mrb( 2,  0 ),
							     mrb( 0,  1 ), mrb( 1,  1 ), mrb( 2,  1 ),
							     mrb( 0,  2 ), mrb( 1,  2 ), mrb( 2,  2 ) );
	assert( !all( mv == mf7 ) );
	assert( any( mv == mf7 ) );
	printf( " select mf7 (%f): %f\n", f[ 7 ], select( mf7 ) );

	mr = mv + mv;
	gr = gm + gm;
	printf( " Ref + Ref		 | Ours + Ours\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n",
			gr[ 0 ][ 0 ], gr[ 1 ][ 0 ], gr[ 2 ][ 0 ], mr( 0,  0 ), mr( 1,  0 ), mr( 2,  0 ),
			gr[ 0 ][ 1 ], gr[ 1 ][ 1 ], gr[ 2 ][ 1 ], mr( 0,  1 ), mr( 1,  1 ), mr( 2,  1 ),
			gr[ 0 ][ 2 ], gr[ 1 ][ 2 ], gr[ 2 ][ 2 ], mr( 0,  2 ), mr( 1,  2 ), mr( 2,  2 ) );
	mr = mv - mv;
	gr = gm - gm;
	printf( " Ref - Ref		 | Ours - Ours\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n",
			gr[ 0 ][ 0 ], gr[ 1 ][ 0 ], gr[ 2 ][ 0 ], mr( 0,  0 ), mr( 1,  0 ), mr( 2,  0 ),
			gr[ 0 ][ 1 ], gr[ 1 ][ 1 ], gr[ 2 ][ 1 ], mr( 0,  1 ), mr( 1,  1 ), mr( 2,  1 ),
			gr[ 0 ][ 2 ], gr[ 1 ][ 2 ], gr[ 2 ][ 2 ], mr( 0,  2 ), mr( 1,  2 ), mr( 2,  2 ) );
	mr = mv * mv;
	gr = gm * gm;
	printf( " Ref * Ref		 | Ours * Ours\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n",
			gr[ 0 ][ 0 ], gr[ 1 ][ 0 ], gr[ 2 ][ 0 ], mr( 0,  0 ), mr( 1,  0 ), mr( 2,  0 ),
			gr[ 0 ][ 1 ], gr[ 1 ][ 1 ], gr[ 2 ][ 1 ], mr( 0,  1 ), mr( 1,  1 ), mr( 2,  1 ),
			gr[ 0 ][ 2 ], gr[ 1 ][ 2 ], gr[ 2 ][ 2 ], mr( 0,  2 ), mr( 1,  2 ), mr( 2,  2 ) );	
	
	ma *= 2.f;
	gr = gm * 2.f;
	printf( " Ref * 2.f		 | Ours * 2.f\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n",
			gr[ 0 ][ 0 ], gr[ 1 ][ 0 ], gr[ 2 ][ 0 ], ma( 0,  0 ), ma( 1,  0 ), ma( 2,  0 ),
			gr[ 0 ][ 1 ], gr[ 1 ][ 1 ], gr[ 2 ][ 1 ], ma( 0,  1 ), ma( 1,  1 ), ma( 2,  1 ),
			gr[ 0 ][ 2 ], gr[ 1 ][ 2 ], gr[ 2 ][ 2 ], ma( 0,  2 ), ma( 1,  2 ), ma( 2,  2 ) );	
	ma /= 2.f;
	gr = gr / 2.f;
	printf( " Ref / 2.f		 | Ours / 2.f\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n",
			gr[ 0 ][ 0 ], gr[ 1 ][ 0 ], gr[ 2 ][ 0 ], ma( 0,  0 ), ma( 1,  0 ), ma( 2,  0 ),
			gr[ 0 ][ 1 ], gr[ 1 ][ 1 ], gr[ 2 ][ 1 ], ma( 0,  1 ), ma( 1,  1 ), ma( 2,  1 ),
			gr[ 0 ][ 2 ], gr[ 1 ][ 2 ], gr[ 2 ][ 2 ], ma( 0,  2 ), ma( 1,  2 ), ma( 2,  2 ) );	
	mr = mv + 1.f;
	gr = gm + 1.f;
	printf( " Ref + 1.f		 | Ours + 1.f\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n",
			gr[ 0 ][ 0 ], gr[ 1 ][ 0 ], gr[ 2 ][ 0 ], mr( 0,  0 ), mr( 1,  0 ), mr( 2,  0 ),
			gr[ 0 ][ 1 ], gr[ 1 ][ 1 ], gr[ 2 ][ 1 ], mr( 0,  1 ), mr( 1,  1 ), mr( 2,  1 ),
			gr[ 0 ][ 2 ], gr[ 1 ][ 2 ], gr[ 2 ][ 2 ], mr( 0,  2 ), mr( 1,  2 ), mr( 2,  2 ) );	

	mr = min( mv, 0.f );
	printf( " min(x, 0.0)\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n", mr( 0,  0 ), mr( 1,  0 ), mr( 2,  0 ),
							     mr( 0,  1 ), mr( 1,  1 ), mr( 2,  1 ),
							     mr( 0,  2 ), mr( 1,  2 ), mr( 2,  2 ) );
	mr = max( mv, 0.f );
	printf( " max(x, 0.0)\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n", mr( 0,  0 ), mr( 1,  0 ), mr( 2,  0 ),
							     mr( 0,  1 ), mr( 1,  1 ), mr( 2,  1 ),
							     mr( 0,  2 ), mr( 1,  2 ), mr( 2,  2 ) );
	mr = abs( mv );
	printf( " abs\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n", mr( 0,  0 ), mr( 1,  0 ), mr( 2,  0 ),
							     mr( 0,  1 ), mr( 1,  1 ), mr( 2,  1 ),
							     mr( 0,  2 ), mr( 1,  2 ), mr( 2,  2 ) );
	mr = clamp( mv, -0.5f, 0.5f );
	printf( " clamp( x, -0.5, 0.5 )\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n", mr( 0,  0 ), mr( 1,  0 ), mr( 2,  0 ),
							     mr( 0,  1 ), mr( 1,  1 ), mr( 2,  1 ),
							     mr( 0,  2 ), mr( 1,  2 ), mr( 2,  2 ) );
	mr = saturate( mv );
	printf( " saturate\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n", mr( 0,  0 ), mr( 1,  0 ), mr( 2,  0 ),
							     mr( 0,  1 ), mr( 1,  1 ), mr( 2,  1 ),
							     mr( 0,  2 ), mr( 1,  2 ), mr( 2,  2 ) );
	mr = lerp( mv, ma * 2.f, 0.5f );
	gr = gm + ( 0.5f * gm );
	printf( " lerp(ref,2ref,0.5)| lerp(ours,2ours,0.5)\n"
			" [ %f, %f, %f ]    | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]    | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]    | [ %f, %f, %f ]\n",
			gr[ 0 ][ 0 ], gr[ 1 ][ 0 ], gr[ 2 ][ 0 ], mr( 0,  0 ), mr( 1,  0 ), mr( 2,  0 ),
			gr[ 0 ][ 1 ], gr[ 1 ][ 1 ], gr[ 2 ][ 1 ], mr( 0,  1 ), mr( 1,  1 ), mr( 2,  1 ),
			gr[ 0 ][ 2 ], gr[ 1 ][ 2 ], gr[ 2 ][ 2 ], mr( 0,  2 ), mr( 1,  2 ), mr( 2,  2 ) );	

	printf( " minComponent: %f\t maxComponent: %f\n", minComponent( mv ), maxComponent( mv ) );

	gvr = gv * glm::mat3( 1.f );
	vr = v * makeIdentity< f32_t, 3 >( );
	printf( " vec             | [ %f, %f, %f ]\n"
			" ref * identity  | [ %f, %f, %f ]\n"
			" ours * identity | [ %f, %f, %f ]\n",
			v[ 0 ], v[ 1 ], v[ 2 ], gvr[ 0 ], gvr[ 1 ], gvr[ 2 ], vr[ 0 ], vr[ 1 ], vr[ 2 ] );
	vr = v * mv;
	gvr = gv * gm;
	printf( " ref * refmat    | [ %f, %f, %f ]\n"
			" ours * ourmat   | [ %f, %f, %f ]\n",
			gvr[ 0 ], gvr[ 1 ], gvr[ 2 ], vr[ 0 ], vr[ 1 ], vr[ 2 ] );
	gvr = glm::mat3( 1.f ) * gv;
	vr = makeIdentity< f32_t, 3 >( ) * v;
	printf( " identity * ref  | [ %f, %f, %f ]\n"
			" identity * ours | [ %f, %f, %f ]\n",
			gvr[ 0 ], gvr[ 1 ], gvr[ 2 ], vr[ 0 ], vr[ 1 ], vr[ 2 ] );
	vr = mv * v;
	gvr = gm * gv;
	printf( " refmat * ref   | [ %f, %f, %f ]\n"
			" ourmat * ours  | [ %f, %f, %f ]\n",
			gvr[ 0 ], gvr[ 1 ], gvr[ 2 ], vr[ 0 ], vr[ 1 ], vr[ 2 ] );
	pr = mv * p;
	printf( " ourmat * ourp  | [ %f, %f, %f ]\n",
			pr[ 0 ], pr[ 1 ], pr[ 2 ] );
	pr = p * mv;
	printf( " ourp * ourmat  | [ %f, %f, %f ]\n",
			pr[ 0 ], pr[ 1 ], pr[ 2 ] );

	mr = transpose( mv );
	gr = glm::transpose( gm );
	printf( " transpose ref  | transpose ours \n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n",
			gr[ 0 ][ 0 ], gr[ 1 ][ 0 ], gr[ 2 ][ 0 ], mr( 0,  0 ), mr( 1,  0 ), mr( 2,  0 ),
			gr[ 0 ][ 1 ], gr[ 1 ][ 1 ], gr[ 2 ][ 1 ], mr( 0,  1 ), mr( 1,  1 ), mr( 2,  1 ),
			gr[ 0 ][ 2 ], gr[ 1 ][ 2 ], gr[ 2 ][ 2 ], mr( 0,  2 ), mr( 1,  2 ), mr( 2,  2 ) );	


	printf( " Determinant2x2 ref: %f\t ours: %f \n", glm::determinant( g2m ), determinant( m2 ) );
	printf( " Determinant3x3 ref: %f\t ours: %f \n", glm::determinant( gm ), determinant( mv ) );

	mr = inverse( mv );
	gr = glm::inverse( gm );
	printf( " inverse ref  | inverse ours \n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n"
			" [ %f, %f, %f ] | [ %f, %f, %f ]\n",
			gr[ 0 ][ 0 ], gr[ 1 ][ 0 ], gr[ 2 ][ 0 ], mr( 0,  0 ), mr( 1,  0 ), mr( 2,  0 ),
			gr[ 0 ][ 1 ], gr[ 1 ][ 1 ], gr[ 2 ][ 1 ], mr( 0,  1 ), mr( 1,  1 ), mr( 2,  1 ),
			gr[ 0 ][ 2 ], gr[ 1 ][ 2 ], gr[ 2 ][ 2 ], mr( 0,  2 ), mr( 1,  2 ), mr( 2,  2 ) );
	
}

void test_affine( )
{
#ifdef VUL_CPLUSPLUS11
	Vector< f32_t, 3 > v = Vector< f32_t, 3 >( 1.f );
	Matrix< f32_t, 3, 3 > m = Matrix< f32_t, 3, 3 >( );
	Affine< f32_t, 3 > a = Affine< f32_t, 3 >( m, -v );
#else
	Vector< f32_t, 3 > v = makeVector< f32_t, 3 >( 1.f );
	Matrix< f32_t, 3, 3 > m = makeMatrix< f32_t, 3, 3 >( );
	Affine< f32_t, 3 > a = makeAffine< f32_t, 3 >( m, -v );
#endif

	printf( " [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n"
			" [ %f, %f, %f ]\n", a.mat( 0,  0 ), a.mat( 1,  0 ), a.mat( 2,  0 ),
							     a.mat( 0,  1 ), a.mat( 1,  1 ), a.mat( 2,  1 ),
							     a.mat( 0,  2 ), a.mat( 1,  2 ), a.mat( 2,  2 ) );
	printf( "x[ %f, %f, %f ]\n", a.vec[ 0 ], a.vec[ 1 ], a.vec[ 2 ] );
	
#ifdef VUL_CPLUSPLUS11
	Vector< f32_t, 3 > v1 = Vector< f32_t, 3 >( 2.f );
	Point< f32_t, 3 > p1 = Point< f32_t, 3 >( -2.f );
#else
	Vector< f32_t, 3 > v1 = makeVector< f32_t, 3 >( 2.f );
	Point< f32_t, 3 > p1 = makePoint< f32_t, 3 >( -2.f );
#endif

	v1 = a * v1;
	p1 = a * p1;

	// Print v1 & p1

	Matrix< f32_t, 4, 4 > mat = makeHomogeneousFromAffine( a );

	// Print mat

	printf( "AOK\n" );
}

void test_point( )
{
#ifdef VUL_CPLUSPLUS11
	Point< f32_t, 4 > pa( 1.f );
	Point< f32_t, 4 > pb( 2.f );
	Point< f32_t, 4 > pr( 0.f );
	Vector< f32_t, 4 > v( -5.f );
#else
	Point< f32_t, 4 > pa = makePoint< f32_t, 4 >( 1.f );
	Point< f32_t, 4 > pb = makePoint< f32_t, 4 >( 2.f );
	Point< f32_t, 4 > pr = makePoint< f32_t, 4 >( 0.f );
	Vector< f32_t, 4 > v = makeVector< f32_t, 4 >( -5.f );
	Vector< f32_t, 4 > vr = makeVector< f32_t, 4 >( 0.f );
#endif

	pr = pa + v;
	printf( " (%f, %f, %f)\n"
			"+(%f, %f, %f)\n"
			"=(%f, %f, %f)\n", pa[ 0 ], pa[ 1 ], pa[ 2 ],
							   v[ 0 ], v[ 1 ], v[ 2 ],
							   pr[ 0 ], pr[ 1 ], pr[ 2 ] );
	vr = pa - pb;
	printf( " (%f, %f, %f)\n"
			"-(%f, %f, %f)\n"
			"=(%f, %f, %f)\n", pa[ 0 ], pa[ 1 ], pa[ 2 ],
							   pb[ 0 ], pb[ 1 ], pb[ 2 ],
							   vr[ 0 ], vr[ 1 ], vr[ 2 ] );
	
	pa[ 0 ] = 1.f;
	pa[ 1 ] = 2.f;
	pa[ 2 ] = 3.f;
	pa[ 3 ] = -1.f;
	vr = min( pa, 1.8f );
	printf( "min: <1, 2, 3, -1>,1.8: <%f, %f, %f, %f>\n", vr[ 0 ], vr[ 1 ], vr[ 2 ], vr[ 3 ] );
	vr = max( pa, 1.8f );
	printf( "max: <1, 2, 3, -1>,1.8: <%f, %f, %f, %f>\n", vr[ 0 ], vr[ 1 ], vr[ 2 ], vr[ 3 ] );
	pa[ 0 ] = 1.f;
	pa[ 1 ] = -2.f;
	pa[ 2 ] = -3.f;
	pa[ 3 ] = 0.4f;
	vr = abs( pa );
	printf( "abs: <1, -2, -3, 0.4>: <%f, %f, %f, %f>\n", vr[ 0 ], vr[ 1 ], vr[ 2 ], vr[ 3 ] );
	vr = clamp( pa, 0.f, 1.f );
	printf( "clamp: <1, -2, -3, 0.4>, [0,1]: <%f, %f, %f, %f>\n", vr[ 0 ], vr[ 1 ], vr[ 2 ], vr[ 3 ] );
	vr = saturate( pa );
	printf( "saturate: <1, -2, -3, 0.4>: <%f, %f, %f, %f>\n", vr[ 0 ], vr[ 1 ], vr[ 2 ], vr[ 3 ] );
	pb[ 0 ] = pb[ 1 ] = pb[ 2 ] = pb[ 3 ] = 1.f;
	pr[ 0 ] = pr[ 1 ] = 0.5f; pr[ 2 ] = pr[ 3 ] = 0.f;
	vr = lerp( pr, pb, 0.5f );
	printf( "lerp (0.5): <%f, %f, %f, %f>\n", vr[ 0 ], vr[ 1 ], vr[ 2 ], vr[ 3 ] );
	printf( "mincomp <1, -2, -3, 0.4>: %f\nmaxcomp <1, -2, -3, 0.4>: %f\n", minComponent( pa ), maxComponent( pa ) );

	printf( "AOK\n" );
}
// Quick simple RNG based on Xorhash
struct RNG
{
	unsigned int m_state;

	void seed(unsigned int seed)
	{
		// Thomas Wang's integer hash, as reported by Bob Jenkins
		seed = (seed ^ 61) ^ (seed >> 16);
		seed *= 9;
		seed = seed ^ (seed >> 4);
		seed *= 0x27d4eb2d;
		seed = seed ^ (seed >> 15);
		m_state = seed;
	}

	unsigned int randUint()
	{
		// Xorshift algorithm from George Marsaglia's paper
		m_state ^= (m_state << 13);
		m_state ^= (m_state >> 17);
		m_state ^= (m_state << 5);
		return m_state;
	}

	float randFloat()
	{
		return float(randUint()) * (1.0f / 4294967296.0f);
	}

	float randFloat(float min, float max)
	{
		return min + (min - max)*randFloat();
	}
};

void test_quats( )
{
	// @TODO: Test quaternions!
}

#include "vul_timer.hpp"
#include "vul_aosoa.hpp"
void bench_aabbs( )
{
#define COUNT 1024
#define ITERATIONS 100
	AABB< f32_t, 3 > *aabbs, *outs, *outs_aosoa,  *outs_aosoaw;
	Affine< f32_t, 3 > trans;
	AABB< __m128, 3> *aosoa_aabbs, *aosoa_outs;
	AABB< __m256, 3> *aosoa_aabbsw, *aosoa_outsw;
	ui32_t i, j;
	RNG rng;
	Quaternion< f32_t > quat;
	ui64_t tPack, tTrans, tUnpack;
	timer_t t;
	
	aabbs = new AABB< f32_t, 3 >[ COUNT ];
	outs = new AABB< f32_t, 3 >[ COUNT ];
	outs_aosoa = new AABB< f32_t, 3 >[ COUNT ];
	outs_aosoaw = new AABB< f32_t, 3 >[ COUNT ];

	aosoa_aabbs = static_cast< AABB< __m128, 3 >* >( _aligned_malloc( sizeof( AABB< __m128, 3 > ) * ( COUNT / 4 ), 16 ) );
	aosoa_outs = static_cast< AABB< __m128, 3 >* >( _aligned_malloc( sizeof( AABB< __m128, 3 > ) * ( COUNT / 4 ), 16 ) );

	aosoa_aabbsw = static_cast< AABB< __m256, 3 >* >( _aligned_malloc( sizeof( AABB< __m256, 3 > ) * ( COUNT / 8 ), 32 ) );
	aosoa_outsw = static_cast< AABB< __m256, 3 >* >( _aligned_malloc( sizeof( AABB< __m256, 3 > ) * ( COUNT / 8 ), 32 ) );

	rng.seed( 47 );
	// Fill with random data.
	for( i = 0; i < COUNT; ++i )
	{
		aabbs[ i ]._min[ 0 ] = rng.randFloat( -10, 10 );
		aabbs[ i ]._min[ 1 ] = rng.randFloat( -10, 10 );
		aabbs[ i ]._min[ 2 ] = rng.randFloat( -10, 10 );
		aabbs[ i ]._max[ 0 ] = aabbs[ i ]._min[ 0 ] + rng.randFloat( 1, 10 );
		aabbs[ i ]._max[ 1 ] = aabbs[ i ]._min[ 1 ] + rng.randFloat( 1, 10 );
		aabbs[ i ]._max[ 2 ] = aabbs[ i ]._min[ 2 ] + rng.randFloat( 1, 10 );
	}
	quat = makeQuatFromAxisAngle( 
					normalize( makeVector< f32_t >( rng.randFloat( -1, 1 ), 
													rng.randFloat( -1, 1 ), 
													rng.randFloat( -1, 1 ) ) ),
													rng.randFloat( 0.f, 2.f * VUL_PI ) );
	trans.mat = makeMatrix( quat );
	trans.vec[ 0 ] = rng.randFloat( -10, 10 );
	trans.vec[ 1 ] = rng.randFloat( -10, 10 );
	trans.vec[ 2 ] = rng.randFloat( -10, 10 );
	
	// Time singe-aabb operations
	t.reset( );
	for( j = 0; j < ITERATIONS / 10; ++j )
	{
		for( i = 0; i < COUNT; ++i )
		{
			outs[ i ] = transform( aabbs[ i ], trans );
		}
	}
	tTrans = t.milliseconds( ) * 10;
	printf( "Single: %llu.%04llus\n", tTrans / 1000LL, tTrans % 1000LL );
	// Time AOSOA with pack and unpack in it (but each stage coutned separately)
	t.reset( );
	for( j = 0; j < ITERATIONS; ++j )
	{
		pack< 3 >( aosoa_aabbs, aabbs, ( ui32_t )COUNT );
	}
	tPack = t.milliseconds( );
	t.reset( );
	for( j = 0; j < ITERATIONS; ++j )
	{
		transform3D( aosoa_outs, aosoa_aabbs, trans, COUNT );
	}
	tTrans = t.milliseconds( );
	t.reset( );
	for( j = 0; j < ITERATIONS; ++j )
	{
		unpack< 3 >( outs_aosoa, aosoa_outs, COUNT );
	}
	tUnpack = t.milliseconds( );
	printf( "SSE:%llu.%04llus (pack: %llu.%04llus, trans: %llu.%04llus, unpack: %llu.%04llus )\n",
				( tPack + tTrans + tUnpack ) / 1000LL, ( tPack + tTrans + tUnpack ) % 1000LL, 
				tPack / 1000LL, tPack % 1000LL,
				tTrans / 1000LL, tTrans % 1000LL, 
				tUnpack / 1000LL, tUnpack % 1000LL );
	// Time wide AOSOA with pack and unpack in it (but each stage coutned separately)
	t.reset( );
	for( j = 0; j < ITERATIONS; ++j )
	{
		pack< 3 >( aosoa_aabbsw, aabbs, ( ui32_t )COUNT );
	}
	tPack = t.milliseconds( );
	t.reset( );
	for( j = 0; j < ITERATIONS; ++j )
	{
		transform3D( aosoa_outsw, aosoa_aabbsw, trans, COUNT );
	}
	tTrans = t.milliseconds( );
	t.reset( );
	for( j = 0; j < ITERATIONS; ++j )
	{
		unpack< 3 >( outs_aosoaw, aosoa_outsw, COUNT );
	}
	tUnpack = t.milliseconds( );
	printf( "SSE:%llu.%04llus (pack: %llu.%04llus, trans: %llu.%04llus, unpack: %llu.%04llus )\n",
				( tPack + tTrans + tUnpack ) / 1000LL, ( tPack + tTrans + tUnpack ) % 1000LL, 
				tPack / 1000LL, tPack % 1000LL,
				tTrans / 1000LL, tTrans % 1000LL, 
				tUnpack / 1000LL, tUnpack % 1000LL );
	// Check that we agree!
	
	float absEpsilon = 1e-5f;
	float relEpsilon = 1e-5f;
	for( i = 0; i < COUNT; ++i )
	{
		if( std::abs( outs[ i ]._min[ 0 ] - outs_aosoa[ i ]._min[ 0 ] ) > max( absEpsilon, relEpsilon * std::abs( outs[ i ]._min[ 0 ] ) ) ||
			std::abs( outs[ i ]._min[ 1 ] - outs_aosoa[ i ]._min[ 1 ] ) > max( absEpsilon, relEpsilon * std::abs( outs[ i ]._min[ 1 ] ) ) ||
			std::abs( outs[ i ]._min[ 2 ] - outs_aosoa[ i ]._min[ 2 ] ) > max( absEpsilon, relEpsilon * std::abs( outs[ i ]._min[ 2 ] ) ) ||
			std::abs( outs[ i ]._max[ 0 ] - outs_aosoa[ i ]._max[ 0 ] ) > max( absEpsilon, relEpsilon * std::abs( outs[ i ]._max[ 0 ] ) ) ||
			std::abs( outs[ i ]._max[ 1 ] - outs_aosoa[ i ]._max[ 1 ] ) > max( absEpsilon, relEpsilon * std::abs( outs[ i ]._max[ 1 ] ) ) ||
			std::abs( outs[ i ]._max[ 2 ] - outs_aosoa[ i ]._max[ 2 ] ) > max( absEpsilon, relEpsilon * std::abs( outs[ i ]._max[ 2 ] ) ) )
		{
			printf( "Mismatch in SSE 4-wide\n" );
		}
		if( std::abs( outs[ i ]._min[ 0 ] - outs_aosoaw[ i ]._min[ 0 ] ) > max( absEpsilon, relEpsilon * std::abs( outs[ i ]._min[ 0 ] ) ) ||
			std::abs( outs[ i ]._min[ 1 ] - outs_aosoaw[ i ]._min[ 1 ] ) > max( absEpsilon, relEpsilon * std::abs( outs[ i ]._min[ 1 ] ) ) ||
			std::abs( outs[ i ]._min[ 2 ] - outs_aosoaw[ i ]._min[ 2 ] ) > max( absEpsilon, relEpsilon * std::abs( outs[ i ]._min[ 2 ] ) ) ||
			std::abs( outs[ i ]._max[ 0 ] - outs_aosoaw[ i ]._max[ 0 ] ) > max( absEpsilon, relEpsilon * std::abs( outs[ i ]._max[ 0 ] ) ) ||
			std::abs( outs[ i ]._max[ 1 ] - outs_aosoaw[ i ]._max[ 1 ] ) > max( absEpsilon, relEpsilon * std::abs( outs[ i ]._max[ 1 ] ) ) ||
			std::abs( outs[ i ]._max[ 2 ] - outs_aosoaw[ i ]._max[ 2 ] ) > max( absEpsilon, relEpsilon * std::abs( outs[ i ]._max[ 2 ] ) ) )
		{
			printf( "Mismatch in SSE 8-wide\n" );
		}
	}

	delete [] aabbs;
	delete [] outs;
	delete [] outs_aosoa;
	delete [] outs_aosoaw;
	_aligned_free( aosoa_aabbs );
	_aligned_free( aosoa_outs );
	_aligned_free( aosoa_aabbsw );
	_aligned_free( aosoa_outsw );
}

int main( int argc, char **argv )
{
	
	printf( "\nTesting half precision format\n" );
	test_half( );
	printf( "\nTesting fixed point formats\n" );
	test_fixed( );
	printf( "\nTesting vectors\n" );
	test_vector( );
	printf( "\nTesting points\n" );
	test_point( );
	printf( "\nTesting matrices\n" );
	test_matrix( );
	printf( "\nTesting affine transformations\n" );
	test_affine( );
	printf( "\nTesting AABBs\n" );
	test_aabb( );
	// @TODO: Test quaternions
	printf( "\nTesting quaternions\n" );
	test_quats( );

	printf( "\nBenchmarking AABB transforms\n" );
	bench_aabbs( );
	
	printf( "\nDone. Enter to close.\n" );
	// Wait for input
	char buffer[ 1024 ];
	gets_s( buffer, 1024 );

	return 0;
}

#endif
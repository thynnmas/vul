/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain¹
 *
 * This file contains collission tests for a number of primitives.
 * In contrast to using CSO or the likes, these are absolute tests
 * that are design to be fast. @NOTE: This is not really fast yet,
 * it is only copied from my engine that actually uses CSO etc...
 *
 * @TODO: This doens't compile, it's not actually written/done, just copied from
 * an invironment in which it did compile (but wasn't complete)...
 * 
 * ¹ If public domain is not legally valid in your legal jurisdiction
 *   the MIT licence applies (see the LICENCE file)
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef VUL_COLLISSION_H
#define VUL_COLLISSION_H

#include "Math/vul_math.hpp"
	
#ifdef VUL_COLLISSION_TYPE_FIXED
	typedef fi32_t preal_t;
#else
	typedef f32_t preal_t;
#endif
typedef Matrix< preal_t, 3, 3 > pmat_t;
typedef Vector< preal_t, 3 > pvec_t;
typedef Point< preal_t, 3 > ppt_t;
typedef AABB< preal_t, 3 > paabb_t;

enum ColliderType {
	COLLIDER_BOX,
	COLLIDER_BOUNDED_PLANE,
	COLLIDER_CONVEX_HULL,
	COLLIDER_CYLINDER,
	COLLIDER_SPHERE,
	COLLIDER_TRIANGLE
};

union ColliderPtr {
	ColliderPlane *plane;
	ColliderBox *box;
	ColliderConvexHull *hull;
	ColliderCylinder *cylinder;
	ColliderSphere *sphere;
	ColliderTriangle *triangle;
};

class Collider {

public:
	preal_t mass;
	pmat_t localInertiaTensor;
	pvec_t localCentroid;
	ColliderType type;
	ColliderPtr ptr;		
		
	/**
		* Intersection test with a ray.
		*/
	bool intersect( const Ray &ray, preal_t *t, pvec_t *normal );

	bool intersect( Collider *other );

	bool intersect( const ppt_t &pt );

	//@TODO: Geometry stuff
private:
	// This is horribly verbose, but we do have n^2 possible intersection algos...
	template< typename T >
	bool intersect( const T &primary, Collider *other );

	Vector< preal_t, 2 > project( const ColliderTriangle &tri, const pvec_t &axis );
	Vector< preal_t, 2 > project( const ColliderBox &box, const pvec_t &axis, const pvec_t *axes = NULL );

	bool intersect( const ColliderPlane &a, const ColliderPlane &b );
	bool intersect( const ColliderPlane &plane, const ColliderBox &box );
	bool intersect( const ColliderPlane &plane, const ColliderConvexHull &hull );
	bool intersect( const ColliderPlane &plane, const ColliderCylinder &cylinder );
	bool intersect( const ColliderPlane &plane, const ColliderSphere &sphere );
	bool intersect( const ColliderPlane &plane, const ColliderTriangle &triangle );
	bool intersect( const ColliderPlane &plane, const Ray &ray, preal_t *t, pvec_t *normal );
	bool intersect( const ColliderPlane &plane, const ppt_t &pt );
		
	bool intersect( const ColliderBox &a, const ColliderBox &b );
	bool intersect( const ColliderBox &box, const ColliderPlane &plane );
	bool intersect( const ColliderBox &box, const ColliderConvexHull &hull );
	bool intersect( const ColliderBox &box, const ColliderCylinder &cylinder );
	bool intersect( const ColliderBox &box, const ColliderSphere &sphere );
	bool intersect( const ColliderBox &box, const ColliderTriangle &triangle );
	bool intersect( const ColliderBox &box, const Ray &ray, preal_t *t, pvec_t *normal );
	bool intersect( const ColliderBox &box, const ppt_t &pt );
		
	bool intersect( const ColliderConvexHull &a, const ColliderConvexHull &b );
	bool intersect( const ColliderConvexHull &hull, const ColliderPlane &plane );
	bool intersect( const ColliderConvexHull &hull, const ColliderBox &box );
	bool intersect( const ColliderConvexHull &hull, const ColliderCylinder &cylinder );
	bool intersect( const ColliderConvexHull &hull, const ColliderSphere &sphere );
	bool intersect( const ColliderConvexHull &hull, const ColliderTriangle &triangle );
	bool intersect( const ColliderConvexHull &hull, const Ray &ray, preal_t *t, pvec_t *normal );
	bool intersect( const ColliderConvexHull &hull, const ppt_t &pt );
		
	bool intersect( const ColliderCylinder &a, const ColliderCylinder &b );
	bool intersect( const ColliderCylinder &cylinder, const ColliderPlane &plane );
	bool intersect( const ColliderCylinder &cylinder, const ColliderBox &box );
	bool intersect( const ColliderCylinder &cylinder, const ColliderConvexHull &hull );
	bool intersect( const ColliderCylinder &cylinder, const ColliderSphere &sphere );
	bool intersect( const ColliderCylinder &cylinder, const ColliderTriangle &triangle );
	bool intersect( const ColliderCylinder &cylinder, const Ray &ray, preal_t *t, pvec_t *normal );
	bool intersect( const ColliderCylinder &cylinder, const ppt_t &pt );
		
	bool intersect( const ColliderSphere &a, const ColliderSphere &b );
	bool intersect( const ColliderSphere &sphere, const ColliderPlane &plane );
	bool intersect( const ColliderSphere &sphere, const ColliderBox &box );
	bool intersect( const ColliderSphere &sphere, const ColliderCylinder &cylinder );
	bool intersect( const ColliderSphere &sphere, const ColliderConvexHull &hull );
	bool intersect( const ColliderSphere &sphere, const ColliderTriangle &triangle );
	bool intersect( const ColliderSphere &sphere, const Ray &ray, preal_t *t, pvec_t *normal );
	bool intersect( const ColliderSphere &sphere, const ppt_t &pt );
		
	bool intersect( const ColliderTriangle &a, const ColliderTriangle &b );
	bool intersect( const ColliderTriangle &triangle, const ColliderPlane &plane );
	bool intersect( const ColliderTriangle &triangle, const ColliderBox &box );
	bool intersect( const ColliderTriangle &triangle, const ColliderCylinder &cylinder );
	bool intersect( const ColliderTriangle &triangle, const ColliderConvexHull &hull );
	bool intersect( const ColliderTriangle &triangle, const ColliderSphere &sphere );
	bool intersect( const ColliderTriangle &triangle, const Ray &ray, preal_t *t, pvec_t *normal );
	bool intersect( const ColliderTriangle &triangle, const ppt_t &pt );
};

#ifdef VUL_DEFINE

bool Collider::intersect( const Ray &ray, preal_t *t, pvec_t *normal )
{
	switch( type )
	{
	case COLLIDER_BOX:
		return intersect( *ptr.box, ray, t, normal );
		break;
	case COLLIDER_BOUNDED_PLANE:
		return intersect( *ptr.plane, ray, t, normal );
		break;
	case COLLIDER_CONVEX_HULL:
		return intersect( *ptr.hull, ray, t, normal );
		break;
	case COLLIDER_CYLINDER:
		return intersect( *ptr.cylinder, ray, t, normal );
		break;
	case COLLIDER_SPHERE:
		return intersect( *ptr.sphere, ray, t, normal );
		break;
	case COLLIDER_TRIANGLE:
		return intersect( *ptr.triangle, ray, t, normal );
		break;
	default:
		StaticStrDecl( str, 256 );
		str = "Collider::intersect: Unknown collider type";
		Log::getSingleton( )->writeLine( LOG_NORMAL, &str );
	}
	return false;
}

bool Collider::intersect( Collider *other )
{
	switch( type )
	{
	case COLLIDER_BOX:
		return intersect( *ptr.box, other );
		break;
	case COLLIDER_BOUNDED_PLANE:
		return intersect( *ptr.plane, other );
		break;
	case COLLIDER_CONVEX_HULL:
		return intersect( *ptr.hull, other );
		break;
	case COLLIDER_CYLINDER:
		return intersect( *ptr.cylinder, other );
		break;
	case COLLIDER_SPHERE:
		return intersect( *ptr.sphere, other );
		break;
	case COLLIDER_TRIANGLE:
		return intersect( *ptr.triangle, other );
		break;
	default:
		StaticStrDecl( str, 256 );
		str = "Collider::intersect: Unknown collider type";
		Log::getSingleton( )->writeLine( LOG_NORMAL, &str );
	}
	return false;
}

bool Collider::intersect( const ppt_t &pt )
{
	switch( type )
	{
	case COLLIDER_BOX:
		return intersect( *ptr.box, pt );
		break;
	case COLLIDER_BOUNDED_PLANE:
		return intersect( *ptr.plane, pt );
		break;
	case COLLIDER_CONVEX_HULL:
		return intersect( *ptr.hull, pt );
		break;
	case COLLIDER_CYLINDER:
		return intersect( *ptr.cylinder, pt );
		break;
	case COLLIDER_SPHERE:
		return intersect( *ptr.sphere, pt );
		break;
	case COLLIDER_TRIANGLE:
		return intersect( *ptr.triangle, pt );
		break;
	default:
		StaticStrDecl( str, 256 );
		str = "Collider::intersect: Unknown collider type";
		Log::getSingleton( )->writeLine( LOG_NORMAL, &str );
	}
	return false;
}
template< typename T >
bool Collider::intersect( const T &primary, Collider *other )
{
	switch( other->type )
	{
	case COLLIDER_BOUNDED_PLANE:
		return intersect( *other->ptr.plane, primary );
		break;
	case COLLIDER_BOX:
		return intersect( *other->ptr.box, primary );
		break;
	case COLLIDER_CONVEX_HULL:
		return intersect( *other->ptr.hull, primary );
		break;
	case COLLIDER_CYLINDER:
		return intersect( *other->ptr.cylinder, primary );
		break;
	case COLLIDER_SPHERE:
		return intersect( *other->ptr.sphere, primary );
		break;
	case COLLIDER_TRIANGLE:
		return intersect( *other->ptr.triangle, primary );
		break;
	default:
		StaticStrDecl( str, 256 );
		str = "Collider::intersect: Unknown collider type";
		Log::getSingleton( )->writeLine( LOG_NORMAL, &str );
	}
	return false;
}

Vector< preal_t, 2 > Collider::project( const ColliderTriangle &tri, const pvec_t &axis )
{
	preal_t d0, d1, d2, mn, mx;

	d0 = dot( axis, tri.getVertex( 0 ).as_vec( ) );
	d1 = dot( axis, tri.getVertex( 1 ).as_vec( ) );
	d2 = dot( axis, tri.getVertex( 2 ).as_vec( ) );

	mn = std::min( d0, std::min( d1, d2 ) );
	mx = std::max( d0, std::max( d1, d2 ) );

	return makeVector< preal_t >( mn, mx );
}

Vector< preal_t, 2 > Collider::project( const ColliderBox &box, const pvec_t &axis, const pvec_t *axes )
{
	preal_t origin = dot( axis, box.getCenter( ).as_vec( ) );
	preal_t maxex;
		
	if( axes == NULL )
	{
		maxex = abs( box.getExtent( )[ 0 ] * dot( axis, extractAxis( box.getOrientation( ), 0 ) ) )
				+ abs( box.getExtent( )[ 1 ] * dot( axis, extractAxis( box.getOrientation( ), 1 ) ) )
				+ abs( box.getExtent( )[ 2 ] * dot( axis, extractAxis( box.getOrientation( ), 2 ) ) );
	} else {
		maxex = abs( box.getExtent( )[ 0 ] * dot( axis, axes[ 0 ] ) )
				+ abs( box.getExtent( )[ 1 ] * dot( axis, axes[ 1 ] ) )
				+ abs( box.getExtent( )[ 2 ] * dot( axis, axes[ 2 ] ) );
	}

	return makeVector< preal_t >( origin - maxex, origin + maxex );
}
//-------------------------------------------------------------------------------
// Intersection algorithms.
// WARNING: This is (n^2)/2 algos and not for the faint of heart...
//

bool Collider::intersect( const ColliderPlane &a, const ColliderPlane &b )
{
	preal_t ndotn = dot( a.getNormal( ), b.getNormal( ) );
	if( abs( ndotn ) < std::numeric_limits< preal_t >::epsilon( ) ) 
	{
		// Planes are parallel, check if the same
		preal_t d;
		if( ndotn >= preal_t( 0 ) ) {
			d = a.getConstant( ) - b.getConstant( );
		} else {
			d = a.getConstant( ) + b.getConstant( );
		}
		if( d >= std::numeric_limits< preal_t >::epsilon( ) ) {
			return false;
		}
		return true;
	}
	return true;
}
bool Collider::intersect( const ColliderPlane &plane, const ColliderBox &box )
{
	preal_t tmp[ 3 ] = 
	{
		box.getExtent( )[ 0 ] * dot( plane.getNormal( ), extractAxis( box.getOrientation( ), 0 ) ),
		box.getExtent( )[ 1 ] * dot( plane.getNormal( ), extractAxis( box.getOrientation( ), 1 ) ),
		box.getExtent( )[ 2 ] * dot( plane.getNormal( ), extractAxis( box.getOrientation( ), 2 ) )
	};

	preal_t rad = std::abs( tmp[ 0 ] ) + std::abs( tmp[ 1 ] ) + std::abs( tmp[ 2 ] );
	preal_t sd = dot( plane.getNormal( ), box.getCenter( ).as_vec( ) ) - plane.getConstant( );

	return std::abs( sd ) <= rad;
}
bool Collider::intersect( const ColliderPlane &plane, const ColliderConvexHull &hull )
{
	// @TODO: This is not done
	assert( false && "Not implemented yet!" );
	return false;
}
bool Collider::intersect( const ColliderPlane &plane, const ColliderCylinder &cylinder )
{
	preal_t sd = dot( plane.getNormal( ), cylinder.getCenter( ).as_vec( ) ) - plane.getConstant( );
	preal_t ndotw = std::abs( dot( plane.getNormal( ), cylinder.getAxis( ) ) );
	preal_t root = std::sqrt( abs( preal_t( 1.f ) - ndotw*ndotw ) );
	preal_t term = cylinder.getRadius( ) * root + preal_t( 0.5f ) * cylinder.getHeight( ) * ndotw;

	return std::abs( sd ) <= term;
}
bool Collider::intersect( const ColliderPlane &plane, const ColliderSphere &sphere )
{
	// Project center distance to plane on normal
	preal_t ndotc = dot( plane.getNormal( ), sphere.getCenter( ).as_vec( ) );
	return std::abs( ndotc - plane.getConstant( ) ) <= sphere.getRadius( );
}
bool Collider::intersect( const ColliderPlane &plane, const ColliderTriangle &triangle )
{
	int signs[ 3 ] =
	{
		sign( dot( plane.getNormal( ), triangle.getVertex( 0 ).as_vec( ) ) - plane.getConstant( ) ),
		sign( dot( plane.getNormal( ), triangle.getVertex( 1 ).as_vec( ) ) - plane.getConstant( ) ),
		sign( dot( plane.getNormal( ), triangle.getVertex( 2 ).as_vec( ) ) - plane.getConstant( ) )
	};
		
	return !( signs[ 0 ] == signs[ 1 ] && signs[ 1 ] == signs[ 2 ] );
}
bool Collider::intersect( const ColliderPlane &plane, const Ray &ray, preal_t *t, pvec_t *normal )
{
	preal_t ddotn = dot( ray.getDirection( ), plane.getNormal( ) );
	preal_t sd = dot( plane.getNormal( ), ray.getOrigin( ).as_vec( ) ) - plane.getConstant( );

	if( std::abs( ddotn ) > std::numeric_limits< preal_t >::epsilon( ) )
	{
		*t = -sd / ddotn;
			
	} else if( std::abs( sd ) < std::numeric_limits< preal_t >::epsilon( ) )
	{
		*t = preal_t( 0.f );
	} else {
		return false;
	}
	// We have a collission, return the normal (which is the normal of the plane)
	*normal = plane.getNormal( );
	return true;
}
bool Collider::intersect( const ColliderPlane &plane, const ppt_t &pt )
{
	return ( dot( pt.as_vec( ), plane.getNormal( ) ) - plane.getConstant( ) )
			< std::numeric_limits< preal_t >::epsilon( );
}
		
bool Collider::intersect( const ColliderBox &a, const ColliderBox &b )
{
	const preal_t cutoff = preal_t( 1.f ) - std::numeric_limits< preal_t >::epsilon( );
	bool parallellAxes = false;
		
	const pmat_t aA = makeMatrix< preal_t >( a.getOrientation( ) );
	const pmat_t aB = makeMatrix< preal_t >( b.getOrientation( ) );
	const pvec_t exA = a.getExtent( );
	const pvec_t exB = b.getExtent( );

	const pvec_t diff = b.getCenter( ) - a.getCenter( );

	pmat_t C;
	pmat_t Cabs;
	pvec_t adotd;
	preal_t t, t0, t1, t01;
		
	for( i32_t j = 0; j < 3; ++j )
	{
		for( i32_t i = 0; i < 3; ++i )
		{
			C( j, i ) = dot( column( aA, j ), column( aB, i ) );
			Cabs( j, i ) = abs( C( j, i ) );
			if( Cabs( j, i ) > cutoff )
			{
				parallellAxes = true;
			}
		}
		adotd[ j ] = dot( column( aA, j ), diff );
		t0 = abs( adotd[ j ] );
		t1 = dot( exB, column( Cabs, j ) );
		t01 = exA[ j ] + t1;
		if( t0 > t01 )
		{
			return false;
		}
	}

	for( i32_t i = 0; i < 3; ++i )
	{
		t = abs( dot( column( aB, i ), diff ) );
		t0 = dot( exA, column( Cabs, i ) );
		t01 = t0 + exB[ i ];
		if( t > t01 )
		{
			return false;
		}
	}

	if( parallellAxes )
	{
		// 2D check is good enough
		return true;
	}

	t = abs( adotd[ 2 ] * C( 1, 0 ) - adotd[ 1 ] * C( 2, 0 ) );
	t0 = exA[ 1 ] * Cabs( 2, 0 ) + exA[ 2 ] * Cabs( 1, 0 );
	t1 = exB[ 1 ] * Cabs( 0, 2 ) + exB[ 2 ] * Cabs( 0, 1 );
	t01 = t0 + t1;
	if( t > t01 )
	{
		return false;
	}
		
	t = abs( adotd[ 2 ] * C( 1, 1 ) - adotd[ 1 ] * C( 2, 1 ) );
	t0 = exA[ 1 ] * Cabs( 2, 1 ) + exA[ 2 ] * Cabs( 1, 1 );
	t1 = exB[ 0 ] * Cabs( 0, 2 ) + exB[ 2 ] * Cabs( 0, 0 );
	t01 = t0 + t1;
	if( t > t01 )
	{
		return false;
	}

	t = abs( adotd[ 2 ] * C( 1, 2 ) - adotd[ 1 ] * C( 2, 2 ) );
	t0 = exA[ 1 ] * Cabs( 2, 2 ) + exA[ 2 ] * Cabs( 1, 2 );
	t1 = exB[ 1 ] * Cabs( 0, 1 ) + exB[ 2 ] * Cabs( 0, 0 );
	t01 = t0 + t1;
	if( t > t01 )
	{
		return false;
	}
				
	t = abs( adotd[ 0 ] * C( 2, 0 ) - adotd[ 2 ] * C( 0, 0 ) );
	t0 = exA[ 0 ] * Cabs( 2, 0 ) + exA[ 2 ] * Cabs( 0, 0 );
	t1 = exB[ 1 ] * Cabs( 1, 2 ) + exB[ 2 ] * Cabs( 1, 1 );
	t01 = t0 + t1;
	if( t > t01 )
	{
		return false;
	}
				
	t = abs( adotd[ 0 ] * C( 2, 1 ) - adotd[ 2 ] * C( 0, 1 ) );
	t0 = exA[ 0 ] * Cabs( 2, 1 ) + exA[ 2 ] * Cabs( 0, 1 );
	t1 = exB[ 0 ] * Cabs( 1, 2 ) + exB[ 2 ] * Cabs( 1, 0 );
	t01 = t0 + t1;
	if( t > t01 )
	{
		return false;
	}
				
	t = abs( adotd[ 0 ] * C( 2, 2 ) - adotd[ 2 ] * C( 0, 2 ) );
	t0 = exA[ 0 ] * Cabs( 2, 2 ) + exA[ 2 ] * Cabs( 0, 2 );
	t1 = exB[ 1 ] * Cabs( 1, 1 ) + exB[ 1 ] * Cabs( 1, 0 );
	t01 = t0 + t1;
	if( t > t01 )
	{
		return false;
	}

	t = abs( adotd[ 1 ] * C( 0, 0 ) - adotd[ 0 ] * C( 1, 0 ) );
	t0 = exA[ 0 ] * Cabs( 1, 0 ) + exA[ 1 ] * Cabs( 0, 0 );
	t1 = exB[ 1 ] * Cabs( 2, 2 ) + exB[ 2 ] * Cabs( 2, 1 );
	t01 = t0 + t1;
	if( t > t01 )
	{
		return false;
	}
		
	t = abs( adotd[ 1 ] * C( 0, 1 ) - adotd[ 0 ] * C( 1, 1 ) );
	t0 = exA[ 0 ] * Cabs( 1, 1 ) + exA[ 1 ] * Cabs( 0, 1 );
	t1 = exB[ 0 ] * Cabs( 2, 2 ) + exB[ 2 ] * Cabs( 2, 0 );
	t01 = t0 + t1;
	if( t > t01 )
	{
		return false;
	}
		
	t = abs( adotd[ 1 ] * C( 0, 2 ) - adotd[ 0 ] * C( 1, 2 ) );
	t0 = exA[ 0 ] * Cabs( 1, 2 ) + exA[ 1 ] * Cabs( 0, 2 );
	t1 = exB[ 0 ] * Cabs( 2, 1 ) + exB[ 1 ] * Cabs( 2, 0 );
	t01 = t0 + t1;
	if( t > t01 )
	{
		return false;
	}

	return true;
}
bool Collider::intersect( const ColliderBox &box, const ColliderPlane &plane )
{
	return intersect( plane, box );
}
bool Collider::intersect( const ColliderBox &box, const ColliderConvexHull &hull )
{
	// @TODO: This is not done
	assert( false && "Not implemented yet!" );
	return false;
}
bool Collider::intersect( const ColliderBox &box, const ColliderCylinder &cylinder )
{
	// @TODO: This is not done
	assert( false && "Not implemented yet!" );
	return false;
}
bool Collider::intersect( const ColliderBox &box, const ColliderSphere &sphere )
{
	pvec_t diffCenter = sphere.getCenter( ) - box.getCenter( );

	pvec_t a = makeVector< preal_t >( abs( dot( diffCenter, extractAxis( box.getOrientation( ), 0 ) ) ),
										abs( dot( diffCenter, extractAxis( box.getOrientation( ), 1 ) ) ),
										abs( dot( diffCenter, extractAxis( box.getOrientation( ), 2 ) ) ) );
	pvec_t diff = a - box.getExtent( );

	if( a[ 0 ] <= box.getExtent( )[ 0 ] ) {
		if( a[ 1 ] <= box.getExtent( )[ 1 ] ) {
			if( a[ 2 ] <= box.getExtent( )[ 2 ] ) {
				return true;
			} else {
				return diff[ 2 ] <= sphere.getRadius( );
			}
		} else {
			if( a[ 2 ] <= box.getExtent( )[ 2 ] ) {
				return diff[ 1 ] <= sphere.getRadius( );
			} else {
				preal_t r2 = sphere.getRadius( ) * sphere.getRadius( );
				return diff[ 1 ] * diff[ 1 ] + diff[ 2 ] * diff[ 2 ] <= r2;
			}
		}
	} else {			
		if( a[ 1 ] <= box.getExtent( )[ 1 ] ) {
			if( a[ 2 ] <= box.getExtent( )[ 2 ] ) {
				return diff[ 0 ] <= sphere.getRadius( );
			} else {
				preal_t r2 = sphere.getRadius( ) * sphere.getRadius( );
				return diff[ 0 ] * diff[ 0 ] + diff[ 2 ] * diff[ 2 ] <= r2;
			}
		} else {
			if( a[ 2 ] <= box.getExtent( )[ 2 ] ) {
				preal_t r2 = sphere.getRadius( ) * sphere.getRadius( );
				return diff[ 0 ] * diff[ 0 ] + diff[ 1 ] * diff[ 1 ] <= r2;
			} else {					
				preal_t r2 = sphere.getRadius( ) * sphere.getRadius( );
				return dot( diff, diff ) <= r2;
			}
		}
	}
}
bool Collider::intersect( const ColliderBox &box, const ColliderTriangle &triangle )
{
	Vector< preal_t, 2 > ex0, ex1;
	pvec_t diff, edges[ 3 ], axes[ 3 ];
		
	for( i32_t i = 0; i < 3; ++i )
	{
		axes[ i ] = extractAxis( box.getOrientation( ), i );
	}
	edges[ 0 ] = triangle.getVertex( 1 ) - triangle.getVertex( 0 );
	edges[ 1 ] = triangle.getVertex( 2 ) - triangle.getVertex( 0 );
	diff = cross( edges[ 0 ], edges[ 1 ] );
	ex0[ 0 ] = dot( diff, triangle.getVertex( 0 ).as_vec( ) );
	ex0[ 1 ] = ex0[ 0 ];
	ex1 = project( box, diff, axes );
	if( ex1[ 1 ] < ex0[ 0 ] || ex0[ 1 ] < ex1[ 0 ] )
	{
		return false;
	}

	for( i32_t i = 0; i < 3; ++i )
	{
		ex0 = project( triangle, axes[ i ] );
		preal_t ddotc = dot( axes[ i ], box.getCenter( ).as_vec( ) );
		ex1[ 0 ] = ddotc - box.getExtent( )[ i ];
		ex1[ 1 ] = ddotc + box.getExtent( )[ i ];
		if( ex1[ 1 ] < ex0[ 0 ] || ex0[ 1 ] < ex1[ 0 ] )
		{
			return false;
		}
	}

	edges[ 2 ] = edges[ 1 ] - edges[ 0 ];
	for( i32_t i = 0; i < 3; ++i )
	{
		for( i32_t j = 0; j < 3; ++j )
		{
			diff = cross( edges[ i ], axes[ j ] );
			ex0 = project( triangle, diff );
			ex1 = project( box, diff, axes );
			if( ex1[ 1 ] < ex0[ 0 ] || ex0[ 1 ] < ex1[ 0 ] )
			{
				return false;
			}
		}
	}

	return true;
}
bool Collider::intersect( const ColliderBox &box, const Ray &ray, preal_t *t, pvec_t *normal )
{
	// Transform ray into box space
	pmat_t invOri = makeMatrix( inverse( box.getOrientation( ) ) );
	pvec_t o = ray.getOrigin( ).as_vec( ) * invOri;
	pvec_t d = ray.getDirection( ) * invOri;
	AABB< preal_t, 3 > aabb = makeAABB< preal_t, 3 >( box.getCenter( ).as_vec( ) - box.getExtent( ) * preal_t( 0.5f ),
														box.getCenter( ).as_vec( ) + box.getExtent( ) * preal_t( 0.5f ) );
	Ray r( o.as_point( ), d );
	RayIntersectPoint pt = r.intersects( aabb );
	if( pt.intersects )
	{
		*t = pt.distanceToIntersect;
		o = o + d * ( *t );
		preal_t minus = preal_t( -1.f ),
				plus = preal_t( 1.f );
		( *normal )[ 0 ] = o[ 0 ] == aabb._min[ 0 ] ? minus : plus;
		( *normal )[ 1 ] = o[ 1 ] == aabb._min[ 1 ] ? minus : plus;
		( *normal )[ 2 ] = o[ 2 ] == aabb._min[ 2 ] ? minus : plus;
		*normal = *normal * makeMatrix( box.getOrientation( ) );
		return true;
	} else {
		return false;
	}
}
bool Collider::intersect( const ColliderBox &box, const ppt_t &pt )
{
	Vector< preal_t, 3 > halfExtent = box.getExtent( ) * preal_t( 0.5f );
	paabb_t b = makeAABB< preal_t, 3 >( ( box.getCenter( ).as_vec( ) - halfExtent ).as_point( ),
										( box.getCenter( ).as_vec( ) + halfExtent ).as_point( ) );
	pvec_t localPt = pt - box.getCenter( );
	ppt_t rpt = ( ( inverse( box.getOrientation( ) ) * localPt ) 
					+ box.getCenter( ).as_vec( ) ).as_point( ); // Rotate the local point and back to world space
	return inside( b, rpt );
}
		
bool Collider::intersect( const ColliderConvexHull &a, const ColliderConvexHull &b )
{
	// @TODO: This is not done
	assert( false && "Not implemented yet!" );
	return false;
}
bool Collider::intersect( const ColliderConvexHull &hull, const ColliderPlane &plane )
{
	return intersect( plane, hull );
}
bool Collider::intersect( const ColliderConvexHull &hull, const ColliderBox &box )
{
	return intersect( box, hull );
}
bool Collider::intersect( const ColliderConvexHull &hull, const ColliderCylinder &cylinder )
{
	// @TODO: This is not done
	assert( false && "Not implemented yet!" );
	return false;
}
bool Collider::intersect( const ColliderConvexHull &hull, const ColliderSphere &sphere )
{
	// @TODO: This is not done
	assert( false && "Not implemented yet!" );
	return false;
}
bool Collider::intersect( const ColliderConvexHull &hull, const ColliderTriangle &triangle )
{
	// @TODO: This is not done
	assert( false && "Not implemented yet!" );
	return false;
}
bool Collider::intersect( const ColliderConvexHull &hull, const Ray &ray, preal_t *t, pvec_t *normal )
{
	// @TODO: This is not done
	assert( false && "Not implemented yet!" );
	return false;
}
bool Collider::intersect( const ColliderConvexHull &hull, const ppt_t &pt )
{
	// @TODO: This is not done
	assert( false && "Not implemented yet!" );
	return false;
}
		
bool Collider::intersect( const ColliderCylinder &a, const ColliderCylinder &b )
{
	pvec_t d = b.getCenter( ) - a.getCenter( );
	pvec_t ax = cross( a.getAxis( ), b.getAxis( ) );
	preal_t lenax = norm( ax ),
					halfHa = a.getHeight( ) * preal_t( 0.5f ),
					halfHb = b.getHeight( ) * preal_t( 0.5f ),
					rsum = a.getRadius( ) + b.getRadius( ),
					wadotwb = dot( a.getAxis( ), b.getAxis( ) ),
					zero( 0.f );

	if( lenax > zero )
	{
		if( b.getRadius( ) * lenax
			+ halfHa
			+ halfHb * abs( wadotwb )
			- abs( dot( a.getAxis( ), d ) )
			< zero )
		{
			return false;
		}
		if( a.getRadius( ) * lenax
			+ halfHa * abs( wadotwb )
			+ halfHb
			- abs( dot( b.getAxis( ), d ) )
			< zero )
		{
			return false;
		}
		if( rsum * lenax - abs( dot( ax, d ) ) < zero )
		{
			return false;
		}
		// @TODO: http://www.geometrictools.com/Documentation/IntersectionOfCylinders.pdf
		// Test for separation along cylinder perpendicular for both axes
		// Test for separation in other directions
		assert( false && "Not fully implemented yet!" );
	} else {
		if( halfHa + halfHb - abs( dot( a.getAxis( ), d ) ) < zero )
		{
			return false;
		}
		if( rsum - norm( d - ( a.getAxis( ) * dot( a.getAxis( ), d ) ) ) < zero )
		{
			return false;
		}
	}
	return true;
}
bool Collider::intersect( const ColliderCylinder &cylinder, const ColliderPlane &plane )
{
	return intersect( plane, cylinder );
}
bool Collider::intersect( const ColliderCylinder &cylinder, const ColliderBox &box )
{
	return intersect( box, cylinder );
}
bool Collider::intersect( const ColliderCylinder &cylinder, const ColliderConvexHull &hull )
{
	return intersect( hull, cylinder );
}
bool Collider::intersect( const ColliderCylinder &cylinder, const ColliderSphere &sphere )
{
	// @TODO: This is not done
	assert( false && "Not implemented yet!" );
	return false;
}
bool Collider::intersect( const ColliderCylinder &cylinder, const ColliderTriangle &triangle )
{
	// @TODO: This is not done
	assert( false && "Not implemented yet!" );
	return false;
}
bool Collider::intersect( const ColliderCylinder &cylinder, const Ray &ray, preal_t *t, pvec_t *normal )
{
	// @TODO: This is not done
	assert( false && "Not implemented yet!" );
	return false;
}
bool Collider::intersect( const ColliderCylinder &cylinder, const ppt_t &pt )
{
	preal_t half( 0.5f );
	pvec_t pt1 = cylinder.getCenter( ).as_vec( ) + cylinder.getAxis( ) * ( cylinder.getHeight( ) * half ),
			pt2 = cylinder.getCenter( ).as_vec( ) + cylinder.getAxis( ) * ( cylinder.getHeight( ) * half );
	pvec_t dpt = pt.as_vec( ) - pt1;
	preal_t d = dot( dpt, pt2 - pt1 );
	preal_t lensq = cylinder.getHeight( ) * cylinder.getHeight( );

	if( d < preal_t( 0.f ) || d > lensq )
	{
		return false;
	} else {
		preal_t dsq = dot( dpt, dpt ) - d * d / lensq;

		if( dsq > cylinder.getRadius( ) * cylinder.getRadius( ) )
		{
			return false;
		} else {
			return true;
		}
	}
}
		
bool Collider::intersect( const ColliderSphere &a, const ColliderSphere &b )
{
	return norm( b.getCenter( ) - a.getCenter( ) ) <= ( a.getRadius( ) + b.getRadius( ) );
}
bool Collider::intersect( const ColliderSphere &sphere, const ColliderPlane &plane )
{
	return intersect( plane, sphere );
}
bool Collider::intersect( const ColliderSphere &sphere, const ColliderBox &box )
{
	return intersect( box, sphere );
}
bool Collider::intersect( const ColliderSphere &sphere, const ColliderCylinder &cylinder )
{
	return intersect( cylinder, sphere );
}
bool Collider::intersect( const ColliderSphere &sphere, const ColliderConvexHull &hull )
{
	return intersect( hull, sphere );
}
bool Collider::intersect( const ColliderSphere &sphere, const ColliderTriangle &triangle )
{
	return norm( sphere.getCenter( ) - triangle.getVertex( 0 ) ) <= sphere.getRadius( )
		|| norm( sphere.getCenter( ) - triangle.getVertex( 1 ) ) <= sphere.getRadius( )
		|| norm( sphere.getCenter( ) - triangle.getVertex( 2 ) ) <= sphere.getRadius( );
}
bool Collider::intersect( const ColliderSphere &sphere, const Ray &ray, preal_t *t, pvec_t *normal )
{
	const preal_t inf = std::numeric_limits< preal_t >::max( );
	const preal_t eps = std::numeric_limits< preal_t >::epsilon( );
	const pvec_t  p = sphere.getCenter( ) - ray.getOrigin( );
	const preal_t PdotP = dot( p, p );
	const preal_t PdotD = dot( p, ray.getDirection( ) );
	const preal_t b    = PdotD,
					bsqr = b * b;
	preal_t d = bsqr - PdotP + sphere.getRadius( ) * sphere.getRadius( );
	if( d < 0 ) {
		return false;
	}
	d = sqrt( d );
	f32_t t1 = b - d,
			t2 = b + d;
	t1 = t1 < eps ? inf : t1;
	t2 = t2 < eps ? inf : t2;
	*t = t1 < t2 ? t1 : t2;

	*normal = ( ray.getOrigin( ) + ray.getDirection( ) * ( *t ) ) - sphere.getCenter( );
	return true;
}
bool Collider::intersect( const ColliderSphere &sphere, const ppt_t &pt )
{
	return norm( pt - sphere.getCenter( ) ) <= sphere.getRadius( );
}
		
bool Collider::intersect( const ColliderTriangle &a, const ColliderTriangle &b )
{
	// Get normal of A
	pvec_t ae[ 3 ];
	ae[ 0 ] = a.getVertex( 1 ) - a.getVertex( 0 );
	ae[ 1 ] = a.getVertex( 2 ) - a.getVertex( 0 );
	ae[ 2 ] = a.getVertex( 0 ) - a.getVertex( 2 );
	pvec_t an = cross( ae[ 0 ], ae[ 1 ] );
		
	// Project B onto normal of A, test for separation
	preal_t anDotA = dot( an, a.getVertex( 0 ).as_vec( ) );
	Vector< preal_t, 2 > arp = project( b, an ), brp;
	if( anDotA < arp[ 0 ] || anDotA > arp[ 1 ] ) {
		return false;
	}

	// Get normal of B
	pvec_t be[ 3 ];
	be[ 0 ] = b.getVertex( 1 ) - b.getVertex( 0 );
	be[ 1 ] = b.getVertex( 2 ) - b.getVertex( 0 );
	be[ 2 ] = b.getVertex( 0 ) - b.getVertex( 2 );
	pvec_t bn = cross( be[ 0 ], be[ 1 ] );

	pvec_t nxn = cross( an, bn );
	if( dot( nxn, nxn ) >= std::numeric_limits< preal_t >::epsilon( ) )
	{	// Not parallel
		// Project A onto normal of B, test for separation
		preal_t bnDotB = dot( bn, b.getVertex( 0 ).as_vec( ) );
		brp = project( a, bn );
		if( bnDotB < arp[ 0 ] || bnDotB > arp[ 1 ] ) {
			return false;
		}

		for( ui32_t ib = 0; ib < 3; ++ib ) {
			for( ui32_t ia = 0; ia < 3; ++ia ) {
				pvec_t d = cross( ae[ ia ], be[ ib ] );
				arp = project( a, d );
				brp = project( b, d );
				if( arp[ 1 ] < brp[ 0 ] || brp[ 1 ] < arp[ 0 ] )
				{
					return false;
				}
			}
		}
	} else {
		// Triangles are coplanar
		for( ui32_t i = 0; i < 3; ++i ) {
			pvec_t d = cross( an, ae[ i ] );
			arp = project( a, d );
			brp = project( b, d );
			if( arp[ 1 ] < brp[ 0 ] || brp[ 1 ] < arp[ 0 ] ) {
				return false;
			}
		}
		for( ui32_t i = 0; i < 3; ++i ) {
			pvec_t d = cross( bn, be[ i ] );
			arp = project( a, d );
			brp = project( b, d );
			if( arp[ 1 ] < brp[ 0 ] || brp[ 1 ] < arp[ 0 ] ) {
				return false;
			}
		}
	}
	return true;
}
bool Collider::intersect( const ColliderTriangle &triangle, const ColliderPlane &plane )
{
	return intersect( plane, triangle );
}
bool Collider::intersect( const ColliderTriangle &triangle, const ColliderBox &box )
{
	return intersect( box, triangle );
}
bool Collider::intersect( const ColliderTriangle &triangle, const ColliderCylinder &cylinder )
{
	return intersect( cylinder, triangle );
}
bool Collider::intersect( const ColliderTriangle &triangle, const ColliderConvexHull &hull )
{
	return intersect( hull, triangle );
}
bool Collider::intersect( const ColliderTriangle &triangle, const ColliderSphere &sphere )
{
	return intersect( sphere, triangle );
}
bool Collider::intersect( const ColliderTriangle &triangle, const Ray &ray, preal_t *t, pvec_t *normal )
{
	const preal_t inf = std::numeric_limits< f32_t >::max( );
	const preal_t eps = std::numeric_limits< f32_t >::epsilon( );
	pvec_t e1 = triangle.getVertex( 1 ) - triangle.getVertex( 0 );
	pvec_t e2 = triangle.getVertex( 2 ) - triangle.getVertex( 0 );
	*normal = normalize( cross( e1, e2 ) );
	preal_t b = dot( *normal, ray.getDirection( ) );
	
	pvec_t w0 = ray.getOrigin( ) - triangle.getVertex( 0 );
	preal_t a = -dot( *normal, w0 );
	preal_t tn = a / b;

	ppt_t p = ray.getOrigin( ) + ( ray.getDirection( ) * tn );

	preal_t uu, uv, vv, wu, wv, inverseD;
	uu = dot( e1, e1 );
	uv = dot( e1, e2 );
	vv = dot( e2, e2 );

	pvec_t w = p - triangle.getVertex( 0 );
	wu = dot( w, e1 );
	wv = dot( w, e2 );
	inverseD = uv *uv - uu *vv;
	inverseD = 1.f / inverseD;

	preal_t u = ( uv * wv - vv * wu ) * inverseD;
	if( u < preal_t( 0.f ) || u > preal_t( 1.f ) ) {
		return false;
	}

	preal_t v = ( uv * wu - uu * wv ) * inverseD;
	if( v < preal_t( 0.f ) || ( u + v ) > preal_t( 1.f ) ) {
		return false;
	}

	*t = tn;
	return true;
}
bool Collider::intersect( const ColliderTriangle &triangle, const ppt_t &pt )
{
	pvec_t e1 = triangle.getVertex( 1 ) - triangle.getVertex( 0 );
	pvec_t e2 = triangle.getVertex( 2 ) - triangle.getVertex( 0 );
	pvec_t n = normalize( cross( e1, e2 ) );
	pvec_t d = pt - triangle.getVertex( 0 );

	preal_t dp = dot( d, n );
	pvec_t s = d - n * dp;

	return norm( s ) <= std::numeric_limits< preal_t >::epsilon( );
}


#endif

#endif
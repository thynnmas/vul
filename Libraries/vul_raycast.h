/* @TODO(thynn): This list
 * -BVH. Spheres, possibly also AABBs (octree?). Smart ways of constructing these is what we need..
 * -SIMD support, mainly for the pure soup one, but if we have an octree we might be able to move aabb
 *  intersection pre-recurse (check entire scene before calling recurse first time, go wide for all children,
 *  only recurse into those that hit)
 *     -Multithread, or do we leave that to the user??
 * -Optimize the fuck out of the dumb version (our inner loop).
 * -Proper tests!
 * -Confirm with profiling that the "smarter approach" must be slow if only called once (since constructing the BVH
 *  pretty much requires to be at least O(N), and the dumb version is also O(N)...Make it simply take a bvh
 *  that must be preconstructed with a call to vul_raycast_build_bvh?
 * -BVH construction is not properly cleaned up atm...Creating the tree will probably require multiple allocations!
 * -Rename all the types to vul__SOMETHING, then define shorthands instead. Before EOF, undefine the shorthands again...
 * -Support other vertex formats?? We could easily do this by taking a float-pointer to the first position of the first vertex
 *  and a stride. Only for indexed version, naturally.
 */

#include <float.h>
#include <stdlib.h>
#include <malloc.h> // @TODO(thynn): Not on OS X
#include <math.h>
#include <immintrin.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef float f32;
typedef size_t word;
typedef uint32_t u32;
typedef u32 idx; // @TODO(thynn): Make this definable; should be the user's index size!

#define BVH_COUNT_THRESHOLD 128 // @TODO(thynn): Make this a sensible size to ignore BVH and just do the naive thing

/* Vec3 */
typedef struct v3 {
	union {
		float A[ 3 ];
		struct {
			float x, y, z;
		};
		struct {
			float r, g, b;
		};
	};
} v3;

inline v3 v3sub( v3 a, v3 b ) { 
	v3 r; 
	r.x = a.x - b.x; 
	r.y = a.y - b.y; 
	r.z = a.z - b.z; 
	return r;
}
inline v3 v3cross( v3 a, v3 b ) { 
	v3 r; 
	r.x = a.y * b.z - b.y * a.z;
	r.y = a.z * b.x - b.z * a.x;
	r.z = a.x * b.y - b.x * a.y;
	return r;
}
inline f32 v3dot( v3 a, v3 b ) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

/* SIMD: Vec3[ 4 ] */
typedef struct v3w {
	union {
		__m128 A[ 3 ];
		struct {
			__m128 x, y, z;
		};
		struct {
			__m128 r, g, b;
		};
	};
} v3w;
inline v3w v3wsub( v3w a, v3w b ) { 
	v3w r;
	r.x = _mm_sub_ps( a.x, b.x ); 
	r.y = _mm_sub_ps( a.y, b.y ); 
	r.z = _mm_sub_ps( a.z, b.z ); 
	return r;
}
inline v3w v3wcross( v3w a, v3w b ) { 
	v3w r; 
	r.x = _mm_sub_ps( _mm_mul_ps( a.y, b.z ),
					  _mm_mul_ps( b.y, a.z ) );
	r.y = _mm_sub_ps( _mm_mul_ps( a.z, b.x ),
					  _mm_mul_ps( b.z, a.x ) );
	r.z = _mm_sub_ps( _mm_mul_ps( a.x, b.y ),
					  _mm_mul_ps( b.x, a.y ) );
	return r;
}
inline __m128 v3wdot( v3w a, v3w b ) {
	return _mm_add_ps( _mm_mul_ps( a.x, b.x ),
		   _mm_add_ps( _mm_mul_ps( a.y, b.y ),
					   _mm_mul_ps( a.z, b.z ) ) );
}

#define sse_select( mask, t, f ) _mm_or_ps( _mm_and_ps( mask, t ), _mm_andnot_ps( mask, f ) )

/* Sphere */
typedef struct sphere {
	v3 center;
	f32 radius_squared;
} sphere;

typedef struct tri {
	v3 vertex[ 3 ];
} tri;

typedef struct ray {
	v3 origin, dir;
} ray;

inline
f32 vul__raycast_triangle_intersect( v3 *v0, v3 *v1, v3 *v2, ray r )
{
	v3 e0, e1;
	v3 P, Q, T;
	f32 det, invdet, u, v, t, eps = 1e-6;

	// Calculate edges
	e0 = v3sub( *v1, *v0 );
	e1 = v3sub( *v2, *v0 );

	// Calculate determinant
	P = v3cross( r.dir, e1 );
	det = v3dot( e0, P );
	if( det > -eps && det < eps ) return FLT_MAX; // Ray in plane of triangle, no collission
	invdet = 1.f / det;

	T = v3sub( r.origin, *v0 );
	u = v3dot( T, P ) * invdet;
	if( u < 0.f || u > 1.f ) return FLT_MAX; // Intersection outside triangle

	Q = v3cross( T, e0 );
	v = v3dot( r.dir, Q ) * invdet;
	if( v < 0.f || v > 1.f ) return FLT_MAX; // Intersection outside triangle

	t = v3dot( e1, Q ) * invdet;
	if( t < eps ) return FLT_MAX; // Intersection behind/on origin, no hit

	// If we made it this far, the intersection is in the triangle, and on the right side of the origin.
	return t;
}

inline
__m128 vul__raycast_triangle_intersect_wide( v3w *v0, v3w *v1, v3w *v2, ray r )
{
	v3w e0, e1;
	v3w P, Q, T, rd, ro;
	__m128 det, invdet, u, v, t, eps, negeps, zero, one, inf, 
		   cd, cu, cv, ct, cr; // Comparisons

	inf = _mm_set1_ps( FLT_MAX );
	eps = _mm_set1_ps( 1e-6 );
	negeps = _mm_set1_ps( -1e-6 );
	zero = _mm_set1_ps( 0.f );
	one = _mm_set1_ps( 1.f );
	rd.x = _mm_set1_ps( r.dir.x );
	rd.y = _mm_set1_ps( r.dir.y );
	rd.z = _mm_set1_ps( r.dir.z );
	ro.x = _mm_set1_ps( r.origin.x );
	ro.y = _mm_set1_ps( r.origin.y );
	ro.z = _mm_set1_ps( r.origin.z );

	// Calculate edges
	e0 = v3wsub( *v1, *v0 );
	e1 = v3wsub( *v2, *v0 );

	// Calculate determinant
	P = v3wcross( rd, e1 );
	det = v3wdot( e0, P );
	cd = _mm_and_ps( _mm_cmp_ps( det, eps, _CMP_LT_OQ ), 
					 _mm_cmp_ps( det, negeps, _CMP_GT_OQ ) );
	
	invdet = _mm_div_ps( one, det );

	T = v3wsub( ro, *v0 );
	u = _mm_mul_ps( v3wdot( T, P ), invdet );
	cu = _mm_and_ps( _mm_cmp_ps( u, zero, _CMP_LT_OQ ), 
					 _mm_cmp_ps( u, one, _CMP_GT_OQ ) );
	
	Q = v3wcross( T, e0 );
	v = _mm_mul_ps( v3wdot( rd, Q ), invdet );
	cv = _mm_and_ps( _mm_cmp_ps( v, zero, _CMP_LT_OQ ),
					 _mm_cmp_ps( v, one, _CMP_GT_OQ ) );
	
	t = _mm_mul_ps( v3wdot( e1, Q ), invdet );
	ct = _mm_cmp_ps( t, eps, _CMP_LT_OQ );
	
	// If any of the comparisons above are true, we've missed
	cr = _mm_or_ps( cd, 
		 _mm_or_ps( cu, 
		 _mm_or_ps( cv, ct ) ) );

	return sse_select( cr, inf, t );
}

inline
f32 vul__raycast_sphere_intersect( sphere *s, ray r )
{
	v3 P;
	f32 t, d, t0, t1, tm, eps = 1e-6;

	P = v3sub( s->center, r.origin );
	t = v3dot( P, r.dir );
	d = v3dot( P, P ) - t * t;
	if( d > s->radius_squared ) return FLT_MAX; // Line passes by the sphere, no hit

	tm = sqrtf( s->radius_squared - d );
	t0 = t - tm;
	t1 = t + tm;

	if( t0 > t1 ) {
		t = t0;
		t0 = t1;
		t1 = t;
	}
	if( t0 < eps ) {
		if( t1 < eps ) return FLT_MAX; // Both intersections behind us
		return t1; // Intersection ahead (we're inside the sphere, but that's still a hit)
	}
	return t0; // First hit is ahead
}

/* The dump way, just cast against all of them in order. Fills in the closes match and the distance to it, or NULL if none */
int vul_raycast_triangle_soup_dumb( tri **closest, f32 *dist, ray r, tri *soup, word count )
{
	f32 dmin, d;
	word imin;

	imin = -1;
	dmin = FLT_MAX;
	for( word i = 0; i < count; ++i ) {
		d = vul__raycast_triangle_intersect( &soup[ i ].vertex[ 0 ], &soup[ i ].vertex[ 1 ], &soup[ i ].vertex[ 2 ], r );
		if( d < dmin ) {
			dmin = d;
			imin = i;
		}
	}

	if( imin == -1 ) {
		*closest = NULL;
		return 0;
	} else {
		*closest = &soup[ imin ];
		*dist = dmin;
		return 1;
	}
}

/* The dump way, just cast against all of them in order. Fills in the closes match and the distance to it, or NULL if none */
int vul_raycast_triangle_soup_dumb_simd( tri **closest, f32 *dist, ray r, tri *soup, word count )
{
	__m128 dmin, d, c;
	__m128 imin, id;
	v3w v0, v1, v2;
	f32 dm;
	word im;

	imin = _mm_set1_ps( -1.f );
	dmin = _mm_set1_ps( FLT_MAX );
	for( word i = 0; i < count; i += 4 ) {
		// @TODO(thynn): Handle the last iteration where we segfault...
		for( word j = 0; j < 3; ++j ) {
			v0.A[ j ] = _mm_setr_ps( soup[ i     ].vertex[ 0 ].A[ j ],
									 soup[ i + 1 ].vertex[ 0 ].A[ j ],
									 soup[ i + 2 ].vertex[ 0 ].A[ j ],
									 soup[ i + 3 ].vertex[ 0 ].A[ j ] );
			v1.A[ j ] = _mm_setr_ps( soup[ i     ].vertex[ 1 ].A[ j ],
									 soup[ i + 1 ].vertex[ 1 ].A[ j ],
									 soup[ i + 2 ].vertex[ 1 ].A[ j ],
									 soup[ i + 3 ].vertex[ 1 ].A[ j ] );
			v2.A[ j ] = _mm_setr_ps( soup[ i     ].vertex[ 2 ].A[ j ],
									 soup[ i + 1 ].vertex[ 2 ].A[ j ],
									 soup[ i + 2 ].vertex[ 2 ].A[ j ],
									 soup[ i + 3 ].vertex[ 2 ].A[ j ] );
		}
		d = vul__raycast_triangle_intersect_wide( &v0, &v1, &v2, r );
		
		c = _mm_cmp_ps( d, dmin, _CMP_LT_OQ );
		dmin = sse_select( c, d, dmin );
		
		id = _mm_setr_ps( i, i + 1.f, i + 2.f, i + 3.f );
		imin = sse_select( c, id, imin );
	}

	dm = FLT_MAX;
	im = -1; // @TODO(thynn): Can potentially make this slightly faster by gather over linear, but gain is minimal...
	if( dmin.m128_f32[ 0 ] < dm ) { dm = dmin.m128_f32[ 0 ]; im = ( word )imin.m128_f32[ 0 ]; }
	if( dmin.m128_f32[ 1 ] < dm ) { dm = dmin.m128_f32[ 1 ]; im = ( word )imin.m128_f32[ 1 ]; }
	if( dmin.m128_f32[ 2 ] < dm ) { dm = dmin.m128_f32[ 2 ]; im = ( word )imin.m128_f32[ 2 ]; }
	if( dmin.m128_f32[ 3 ] < dm ) { dm = dmin.m128_f32[ 3 ]; im = ( word )imin.m128_f32[ 3 ]; }

	if( im == -1 ) {
		*closest = NULL;
		return 0;
	} else {
		*closest = &soup[ im ];
		*dist = dm;
		return 1;
	}
}

typedef struct itri {
	idx indices[ 3 ];
} itri;
/* The dumb way but on indices */
int vul_raycast_triangle_soup_dumb_indexed( itri **closest, f32 *dist, ray r, v3 *vertices, idx *indices, word icount )
{
	f32 dmin, d;
	itri t;

	dmin = FLT_MAX;
	for( word i = 0; i < icount; i += 3 ) {
		d = vul__raycast_triangle_intersect( &vertices[ indices[ i ] ],
														 &vertices[ indices[ i + 1 ] ],
														 &vertices[ indices[ i + 2 ] ], r );
		if( d < dmin ) {
			dmin = d;
			t.indices[ 0 ] = indices[ i ];
			t.indices[ 1 ] = indices[ i + 1 ];
			t.indices[ 2 ] = indices[ i + 2 ];
		}
	}

	if( dmin == FLT_MAX ) {
		*closest = NULL;
		return 0;
	} else {
		( *closest )->indices[ 0 ] = t.indices[ 0 ];
		( *closest )->indices[ 1 ] = t.indices[ 1 ];
		( *closest )->indices[ 2 ] = t.indices[ 2 ];
		*dist = dmin;
		return 1;
	}
}
/* The dumb way but on indices and strided vertices */
int vul_raycast_triangle_soup_dumb_indexed_strided( itri **closest, f32 *dist, ray r, void *vertex_data, word vertex_stride, idx *indices, word icount )
{
	f32 dmin, d;
	word i0;

	dmin = FLT_MAX;
	for( word i = 0; i < icount; i += 3 ) {
		v3 *v0 = ( v3* )( ( word )vertex_data + ( word )indices[ i     ] * vertex_stride );
		v3 *v1 = ( v3* )( ( word )vertex_data + ( word )indices[ i + 1 ] * vertex_stride );
		v3 *v2 = ( v3* )( ( word )vertex_data + ( word )indices[ i + 2 ] * vertex_stride );
		d = vul__raycast_triangle_intersect( v0, v1, v2, r );
		if( d < dmin ) {
			dmin = d;
			i0 = i;
		}
	}

	if( dmin == FLT_MAX ) {
		*closest = NULL;
		return 0;
	} else {
		( *closest )->indices[ 0 ] = indices[ i0     ];
		( *closest )->indices[ 1 ] = indices[ i0 + 1 ];
		( *closest )->indices[ 2 ] = indices[ i0 + 2 ];
		*dist = dmin;
		return 1;
	}
}

//-----------------------------------------
// Accelerated versions
//

typedef enum bvh_node_type {
	BBH_NODE_TYPE_INTERNAL,
	BBH_NODE_TYPE_LEAF
} bvh_node_type;

typedef struct bvh {
	sphere bounding;
	union {
		struct {
			bvh *children;
			word child_count;
		} node;
		struct {
			tri *soup;
			word tri_count;
		} leaf_soup;
		struct {
			idx *indices;
			word index_count;
		} leaf_indexed;
	};
	bvh_node_type type;
} bvh;
/* Builds an acceleration structure of the soup */
void vul_raycast_build_bvh( bvh **out, tri *soup, word count )
{
	// @TODO(thynn): THIS
}

void vul__raycast_recurse_bvh( tri **closest, f32 *dist, bvh *root, ray r )
{
	f32 dmin, d;
	tri *t;

	if( root->type == BBH_NODE_TYPE_LEAF ) {
		vul_raycast_triangle_soup_dumb( closest, dist, r, root->leaf_soup.soup, root->leaf_soup.tri_count );
	} else {
		d = vul__raycast_sphere_intersect( &root->bounding, r );
		if( d == FLT_MAX ) {
			*closest = NULL;
			*dist = FLT_MAX;
			return; // No hit
		}

		// Recurse down the tree
		*dist = FLT_MAX;
		for( word i = 0; i < root->node.child_count; ++i ) {
			vul__raycast_recurse_bvh( &t, &d, &root->node.children[ i ], r );
			if( d < *dist ) {
				*dist = d;
				*closest = t;
			}
		}
	}
}

/* Smarter approach; build bvh if count is large enough. If accel is not null, return it (otherwise free it) for reuse.
 * if *accel is not null, reuse that, don't rebuild */
int vul_raycast_triangle_soup_bvh( tri **closest, f32 *dist, tri *soup, ray r, word count, bvh **accel )
{
	bvh *acc;
	if( count < BVH_COUNT_THRESHOLD ) {
		vul_raycast_triangle_soup_dumb( closest, dist, r, soup, count );
		return;
	}

	/* Build or reuse the bounding volume hierarchy */
	if( accel == NULL ) {
		acc = ( bvh* )malloc( sizeof( bvh ) );	
		vul_raycast_build_bvh( &acc, soup, count );
	} else {
		if( *accel == NULL ) {
			vul_raycast_build_bvh( accel, soup, count );
		}
		acc = *accel;
	}

	/* Raycast through the bounding volume hierarchy */
	vul__raycast_recurse_bvh( closest, dist, acc, r );

	/* Free the hierarchy again if wanted */
	if( accel == NULL ) {
		free( acc );
	}

	return ( *closest != NULL );
}

/* Builds an acceleration structure of the soup */
void vul_raycast_build_bvh_indexed( bvh **out, v3 *vertices, idx *indices, word icount )
{
	// @TODO(thynn): THIS
}

void vul__raycast_recurse_bvh_indexed( itri **closest, f32 *dist, bvh *root, v3 *vertices, ray r )
{
	f32 dmin, d;
	itri t, *tt = &t;

	if( root->type == BBH_NODE_TYPE_LEAF ) {
		vul_raycast_triangle_soup_dumb_indexed( closest, dist, r, vertices, root->leaf_indexed.indices, root->leaf_indexed.index_count );
	} else {
		d = vul__raycast_sphere_intersect( &root->bounding, r );
		if( d == FLT_MAX ) {
			*closest = NULL;
			*dist = FLT_MAX;
			return; // No hit
		}

		// Recurse down the tree
		*dist = FLT_MAX;
		for( word i = 0; i < root->node.child_count; ++i ) {
			vul__raycast_recurse_bvh_indexed( &tt, &d, &root->node.children[ i ], vertices, r );
			if( d < *dist ) {
				*dist = d;
				( *closest )->indices[ 0 ] = t.indices[ 0 ];
				( *closest )->indices[ 1 ] = t.indices[ 1 ];
				( *closest )->indices[ 2 ] = t.indices[ 2 ];
			}
		}
	}
}

/* Smarter approach; build bvh if count is large enough. If accel is not null, return it (otherwise free it) for reuse.
 * if *accel is not null, reuse that, don't rebuild. This is the indexed version (@TODO(thynn): 16-bit indices) */
int vul_raycast_triangle_soup_bvh_indexed( itri **closest, f32 *dist, v3 *vertices, idx *indices, word icount, ray r, bvh **accel )
{
	bvh *acc;

	if( ( icount / 3 ) < BVH_COUNT_THRESHOLD ) {
		vul_raycast_triangle_soup_dumb_indexed( closest, dist, r, vertices, indices, icount );
		return;
	}

	/* Build or reuse the bounding volume hierarchy */
	if( accel == NULL ) {
		acc = ( bvh* )malloc( sizeof( bvh ) );	
		vul_raycast_build_bvh_indexed( &acc, vertices, indices, icount );
	} else {
		if( *accel == NULL ) {
			vul_raycast_build_bvh_indexed( accel, vertices, indices, icount );
		}
		acc = *accel;
	}

	/* Raycast through the bounding volume hierarchy */
	vul__raycast_recurse_bvh_indexed( closest, dist, acc, vertices, r );

	/* Free the hierarchy again if wanted */
	if( accel == NULL ) {
		free( acc );
	}
	
	return ( *closest != NULL );
}

#ifdef __cplusplus
}
#endif

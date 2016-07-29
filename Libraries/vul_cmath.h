/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file contains 2-, 3- and 4-dimensional vector and matrix functions
 * for use in C. Matrices are column major (blame OpenGL), so the naming
 * convention is mCR (C=column, R=row); m->a10 is column 1, element 0.
 * 
 * ? If public domain is not legally valid in your legal jurisdiction
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
#ifndef VUL_CMATH_H
#define VUL_CMATH_H

#include <stddef.h>
#include <stdint.h>
#include <math.h>

#ifndef VUL_TYPES_H
#define f32 float
#define f64 double
#endif
typedef size_t word;

typedef struct v2 {
	union {
		struct {
			f32 x, y;
		};
		f32 A[ 2 ];
		struct {
			f32 u, v;
		};
	};
} v2;

typedef struct v3 {
	union {
		struct {
			f32 x, y, z;
		};
		f32 A[ 3 ];
		struct {
			f32 u, v, w;
		};
		struct {
			f32 r, g, b;
		};
	};
} v3;

typedef struct v4 {
	union {
		struct {
			f32 x, y, z, w;
		};
		f32 A[ 4 ];
		struct {
			f32 r, g, b, a;
		};
	};
} v4;

typedef struct m22 {
	union {
		f32 A[ 4 ];
		struct {
			f32 a00, a01,
				a10, a11;
		};
		v2 c[ 2 ];
	};
} m22;

typedef struct m33 {
	union {
		f32 A[ 9 ];
		struct {
			f32 a00, a01, a02,
				a10, a11, a12,
				a20, a21, a22;
		};
		v3 c[ 3 ];
	};
} m33;

typedef struct m44 {
	union {
		f32 A[ 16 ];
		struct {
			f32 a00, a01, a02, a03,
				a10, a11, a12, a13,
				a20, a21, a22, a23,
				a30, a31, a32, a33;
		};
		v4 c[ 4 ];
	};
} m44;

#ifdef _cplusplus
extern "C" {
#endif
	
#define DEFINE_V2OP( name, op ) v2 name( const v2 a, const v2 b );
#define DEFINE_V3OP( name, op ) v3 name( const v3 a, const v3 b );
#define DEFINE_V4OP( name, op ) v4 name( const v4 a, const v4 b );

#define DEFINE_S2OP( name, op ) v2 name( const v2 a, const f32 c );
#define DEFINE_S3OP( name, op ) v3 name( const v3 a, const f32 c );
#define DEFINE_S4OP( name, op ) v4 name( const v4 a, const f32 c );

	
DEFINE_V2OP( vadd2, + );
DEFINE_V2OP( vsub2, - );
DEFINE_V2OP( vmul2, * );
DEFINE_V2OP( vdiv2, / );
DEFINE_V3OP( vadd3, + );
DEFINE_V3OP( vsub3, - );
DEFINE_V3OP( vmul3, * );
DEFINE_V3OP( vdiv3, / );
DEFINE_V4OP( vadd4, + );
DEFINE_V4OP( vsub4, - );
DEFINE_V4OP( vmul4, * );
DEFINE_V4OP( vdiv4, / );

DEFINE_S2OP( vadds2, + );
DEFINE_S2OP( vsubs2, - );
DEFINE_S2OP( vmuls2, * );
DEFINE_S2OP( vdivs2, / );
DEFINE_S3OP( vadds3, + );
DEFINE_S3OP( vsubs3, - );
DEFINE_S3OP( vmuls3, * );
DEFINE_S3OP( vdivs3, / );
DEFINE_S4OP( vadds4, + );
DEFINE_S4OP( vsubs4, - );
DEFINE_S4OP( vmuls4, * );
DEFINE_S4OP( vdivs4, / );

#undef DEFINE_V2OP
#undef DEFINE_V3OP
#undef DEFINE_V4OP

#undef DEFINE_S2OP
#undef DEFINE_S3OP
#undef DEFINE_S4OP

v2 vec2( const f32 x, const f32 y );
v3 vec3( const f32 x, const f32 y, const f32 z );
v4 vec4( const f32 x, const f32 y, const f32 z, const f32 w );

v2 vcopy2( const v2 v );
v3 vcopy3( const v3 v );
v4 vcopy4( const v4 v );

f32 vdot2( const v2 a, const v2 b);
f32 vdot3( const v3 a, const v3 b);
f32 vdot4( const v4 a, const v4 b);

f32 vnorm2( const v2 v );
f32 vnorm3( const v3 v );
f32 vnorm4( const v4 v );

v2 vnormalize2( const v2 v );
v3 vnormalize3( const v3 v );
v4 vnormalize4( const v4 v );

v2 vcross2( const v2 v );
v3 vcross3( const v3 a, const v3 b );

v2 vclamp2( const v2 v, const f32 a, const f32 b );
v3 vclamp3( const v3 v, const f32 a, const f32 b );
v4 vclamp4( const v4 v, const f32 a, const f32 b );

#define vsaturate2( v ) vclamp2( v, 0.f, 1.f )
#define vsaturate3( v ) vclamp3( v, 0.f, 1.f )
#define vsaturate4( v ) vclamp4( v, 0.f, 1.f )

v2 vlerp2( const v2 a, const v2 b, const f32 t );
v3 vlerp3( const v3 a, const v3 b, const f32 t );
v4 vlerp4( const v4 a, const v4 b, const f32 t );

v2 vmin2( const v2 a, const v2 b );
v3 vmin3( const v3 a, const v3 b );
v4 vmin4( const v4 a, const v4 b );

v2 vmax2( const v2 a, const v2 b );
v3 vmax3( const v3 a, const v3 b );
v4 vmax4( const v4 a, const v4 b );

v2 vreflect2( const v2 v, const v2 n );
v3 vreflect3( const v3 v, const v3 n );
v4 vreflect4( const v4 v, const v4 n );

#define DEFINE_M22COMPWISE_OP( name, op ) m22 name( const m22 *a, const m22 *b );
#define DEFINE_M33COMPWISE_OP( name, op ) m33 name( const m33 *a, const m33 *b );
#define DEFINE_M44COMPWISE_OP( name, op ) m44 name( const m44 *a, const m44 *b );

DEFINE_M22COMPWISE_OP( madd22, + )
DEFINE_M22COMPWISE_OP( msub22, - )

DEFINE_M33COMPWISE_OP( madd33, + )
DEFINE_M33COMPWISE_OP( msub33, - )

DEFINE_M44COMPWISE_OP( madd44, + )
DEFINE_M44COMPWISE_OP( msub44, - )

#undef DEFINE_M22COMPWISE_OP
#undef DEFINE_M33COMPWISE_OP
#undef DEFINE_M44COMPWISE_OP

#define DEFINE_S22COMPWISE_OP( name, op ) m22 name( const m22 *a, const f32 c );
#define DEFINE_S33COMPWISE_OP( name, op ) m33 name( const m33 *a, const f32 c );
#define DEFINE_S44COMPWISE_OP( name, op ) m44 name( const m44 *a, const f32 c );

DEFINE_S22COMPWISE_OP( madds22, + )
DEFINE_S22COMPWISE_OP( msubs22, - )
DEFINE_S22COMPWISE_OP( mdivs22, / )
DEFINE_S22COMPWISE_OP( mmuls22, * )

DEFINE_S33COMPWISE_OP( madds33, + )
DEFINE_S33COMPWISE_OP( msubs33, - )
DEFINE_S33COMPWISE_OP( mdivs33, / )
DEFINE_S33COMPWISE_OP( mmuls33, * )

DEFINE_S44COMPWISE_OP( madds44, + )
DEFINE_S44COMPWISE_OP( msubs44, - )
DEFINE_S44COMPWISE_OP( mdivs44, / )
DEFINE_S44COMPWISE_OP( mmuls44, * )

#undef DEFINE_S22COMPWISE_OP
#undef DEFINE_S33COMPWISE_OP
#undef DEFINE_S44COMPWISE_OP

m22 mcopy2( const m22 *m );
m33 mcopy3( const m33 *m );
m44 mcopy4( const m44 *m );

m22 mmul22( const m22 *a, const m22 *b );
m33 mmul33( const m33 *a, const m33 *b );
m44 mmul44( const m44 *a, const m44 *b );

m22 mlerp22( const m22 *a, const m22 *b, const f32 t );
m33 mlerp33( const m33 *a, const m33 *b, const f32 t );
m44 mlerp44( const m44 *a, const m44 *b, const f32 t );

m22 mtranspose22( const m22 *m );
m33 mtranspose33( const m33 *m );
m44 mtranspose44( const m44 *m );

f32 mdeterminant22( const m22 *m );
f32 mdeterminant33( const m33 *m );
f32 mdeterminant44( const m44 *m );

m22 minverse22( const m22 *m );
m33 minverse33( const m33 *m );
m44 minverse44( const m44 *m );

m22 mtruncate32( const m33 *m );
m22 mtruncate42( const m44 *m );
m33 mtruncate43( const m44 *m );

/* Right side matrix-vector multiplication */
v2 vmulm2( const m22 *m, const v2 v );
/* Right side matrix-vector multiplication */
v3 vmulm3( const m33 *m, const v3 v );
/* Right side matrix-vector multiplication */
v4 vmulm4( const m44 *m, const v4 v );

#ifdef _cplusplus
}
#endif
#endif // VUL_CMATH_H

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

#define DEFINE_V2OP( name, op )\
	v2 name( const v2 a, const v2 b ) {\
		v2 r;\
		r.x = a.x op b.x;\
		r.y = a.y op b.y;\
		return r;\
	}
#define DEFINE_V3OP( name, op )\
	v3 name( const v3 a, const v3 b ) {\
		v3 r;\
		r.x = a.x op b.x;\
		r.y = a.y op b.y;\
		r.z = a.z op b.z;\
		return r;\
	}
#define DEFINE_V4OP( name, op )\
	v4 name( const v4 a, const v4 b ) {\
		v4 r;\
		r.x = a.x op b.x;\
		r.y = a.y op b.y;\
		r.z = a.z op b.z;\
		r.w = a.w op b.w;\
		return r;\
	}

#define DEFINE_S2OP( name, op )\
	v2 name( const v2 a, const f32 c ) {\
		v2 r;\
		r.x = a.x op c;\
		r.y = a.y op c;\
		return r;\
	}
#define DEFINE_S3OP( name, op )\
	v3 name( const v3 a, const f32 c ) {\
		v3 r;\
		r.x = a.x op c;\
		r.y = a.y op c;\
		r.z = a.z op c;\
		return r;\
	}
#define DEFINE_S4OP( name, op )\
	v4 name( const v4 a, const f32 c ) {\
		v4 r;\
		r.x = a.x op c;\
		r.y = a.y op c;\
		r.z = a.z op c;\
		r.w = a.w op c;\
		return r;\
	}

DEFINE_V2OP( vadd2, + );
DEFINE_V2OP( vsub2, - );
DEFINE_V2OP( vmul2, * );
DEFINE_V2OP( vdiv2, / );
DEFINE_V3OP( vadd3, + );
DEFINE_V3OP( vsub3, - );
DEFINE_V3OP( vmul3, * );
DEFINE_V3OP( vdiv3, / );
DEFINE_V4OP( vadd4, + );
DEFINE_V4OP( vsub4, - );
DEFINE_V4OP( vmul4, * );
DEFINE_V4OP( vdiv4, / );

DEFINE_S2OP( vadds2, + );
DEFINE_S2OP( vsubs2, - );
DEFINE_S2OP( vmuls2, * );
DEFINE_S2OP( vdivs2, / );
DEFINE_S3OP( vadds3, + );
DEFINE_S3OP( vsubs3, - );
DEFINE_S3OP( vmuls3, * );
DEFINE_S3OP( vdivs3, / );
DEFINE_S4OP( vadds4, + );
DEFINE_S4OP( vsubs4, - );
DEFINE_S4OP( vmuls4, * );
DEFINE_S4OP( vdivs4, / );

#undef DEFINE_V2OP
#undef DEFINE_V3OP
#undef DEFINE_V4OP

#undef DEFINE_S2OP
#undef DEFINE_S3OP
#undef DEFINE_S4OP

v2 vec2( const f32 x, const f32 y ) { v2 r; r.x = x; r.y = y; return r; }
v3 vec3( const f32 x, const f32 y, const f32 z ) { v3 r; r.x = x; r.y = y; r.z = z; return r; }
v4 vec4( const f32 x, const f32 y, const f32 z, const f32 w ) { v4 r; r.x = x; r.y = y; r.z = z; r.w = w; return r; }

v2 vcopy2( const v2 v ) { v2 r; r.x = v.x; r.y = v.y; return r; }
v3 vcopy3( const v3 v ) { v3 r; r.x = v.x; r.y = v.y; r.z = v.z; return r; }
v4 vcopy4( const v4 v ) { v4 r; r.x = v.x; r.y = v.y; r.z = v.z; r.w = v.w; return r; }

f32 vdot2( const v2 a, const v2 b) { return a.x * b.x + a.y * b.y; }
f32 vdot3( const v3 a, const v3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
f32 vdot4( const v4 a, const v4 b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

f32 vnorm2( const v2 v ) { return sqrtf( vdot2( v, v ) ); }
f32 vnorm3( const v3 v ) { return sqrtf( vdot3( v, v ) ); }
f32 vnorm4( const v4 v ) { return sqrtf( vdot4( v, v ) ); }

v2 vnormalize2( const v2 v ) { return vmuls2( v, 1.f / vnorm2( v ) ); }
v3 vnormalize3( const v3 v ) { return vmuls3( v, 1.f / vnorm3( v ) ); }
v4 vnormalize4( const v4 v ) { return vmuls4( v, 1.f / vnorm4( v ) ); }

v2 vcross2( const v2 v ) {
	v2 r;
	r.x = v.y;
	r.y = -v.x;
	return r;
}
v3 vcross3( const v3 a, const v3 b ) {
	v3 r;
	r.x = a.y * b.z - b.y * a.z;
	r.y = a.z * b.x - b.z * a.x;
	r.z = a.x * b.y - b.x * a.y;
	return r;
}

v2 vclamp2( const v2 v, const f32 a, const f32 b ) {
	v2 r;
	r.x = v.x < a ? a : v.x > b ? b : v.x;
	r.y = v.y < a ? a : v.y > b ? b : v.y;
	return r;
}
v3 vclamp3( const v3 v, const f32 a, const f32 b ) {
	v3 r;
	r.x = v.x < a ? a : v.x > b ? b : v.x;
	r.y = v.y < a ? a : v.y > b ? b : v.y;
	r.z = v.z < a ? a : v.z > b ? b : v.z;
	return r;
}
v4 vclamp4( const v4 v, const f32 a, const f32 b ) {
	v4 r;
	r.x = v.x < a ? a : v.x > b ? b : v.x;
	r.y = v.y < a ? a : v.y > b ? b : v.y;
	r.z = v.z < a ? a : v.z > b ? b : v.z;
	r.w = v.w < a ? a : v.w > b ? b : v.w;
	return r;
}

v2 vlerp2( const v2 a, const v2 b, const f32 t ) {
	v2 r;
	f32 t1 = 1.f - t;
	r.x = a.x * t + b.x * t1;
	r.y = a.y * t + b.y * t1;
	return r;
}
v3 vlerp3( const v3 a, const v3 b, const f32 t ) {
	v3 r;
	f32 t1 = 1.f - t;
	r.x = a.x * t + b.x * t1;
	r.y = a.y * t + b.y * t1;
	r.z = a.z * t + b.z * t1;
	return r;
}
v4 vlerp4( const v4 a, const v4 b, const f32 t ) {
	v4 r;
	f32 t1 = 1.f - t;
	r.x = a.x * t + b.x * t1;
	r.y = a.y * t + b.y * t1;
	r.z = a.z * t + b.z * t1;
	r.w = a.w * t + b.w * t1;
	return r;
}

v2 vmin2( const v2 a, const v2 b ) {
	v2 r;
	r.x = a.x <= b.x ? a.x : b.x;
	r.y = a.y <= b.y ? a.y : b.y;
	return r;
}
v3 vmin3( const v3 a, const v3 b ) {
	v3 r;
	r.x = a.x <= b.x ? a.x : b.x;
	r.y = a.y <= b.y ? a.y : b.y;
	r.z = a.z <= b.z ? a.z : b.z;
	return r;
}
v4 vmin4( const v4 a, const v4 b ) {
	v4 r;
	r.x = a.x <= b.x ? a.x : b.x;
	r.y = a.y <= b.y ? a.y : b.y;
	r.z = a.z <= b.z ? a.z : b.z;
	r.w = a.w <= b.w ? a.w : b.w;
	return r;
}

v2 vmax2( const v2 a, const v2 b ) {
	v2 r;
	r.x = a.x >= b.x ? a.x : b.x;
	r.y = a.y >= b.y ? a.y : b.y;
	return r;
}
v3 vmax3( const v3 a, const v3 b ) {
	v3 r;
	r.x = a.x >= b.x ? a.x : b.x;
	r.y = a.y >= b.y ? a.y : b.y;
	r.z = a.z >= b.z ? a.z : b.z;
	return r;
}
v4 vmax4( const v4 a, const v4 b ) {
	v4 r;
	r.x = a.x >= b.x ? a.x : b.x;
	r.y = a.y >= b.y ? a.y : b.y;
	r.z = a.z >= b.z ? a.z : b.z;
	r.w = a.w >= b.w ? a.w : b.w;
	return r;
}

v2 vreflect2( const v2 v, const v2 n ) {
	return vsub2( vmuls2( n, 2.f * vdot2( v, n ) ), v );
}
v3 vreflect3( const v3 v, const v3 n ) {
	return vsub3( vmuls3( n, 2.f * vdot3( v, n ) ), v );
}
v4 vreflect4( const v4 v, const v4 n ) {
	return vsub4( vmuls4( n, 2.f * vdot4( v, n ) ), v );
}

#ifndef VUL_DEFINE
#else
#define DEFINE_M22COMPWISE_OP( name, op )\
	m22 name( const m22 *a, const m22 *b ) {\
		m22 r;\
		r.a00 = a->a00 op b->a00;\
		r.a01 = a->a01 op b->a01;\
		r.a10 = a->a10 op b->a10;\
		r.a11 = a->a11 op b->a11;\
		return r;\
	}
#define DEFINE_M33COMPWISE_OP( name, op )\
	m33 name( const m33 *a, const m33 *b ) {\
		m33 r;\
		r.a00 = a->a00 op b->a00;\
		r.a01 = a->a01 op b->a01;\
		r.a02 = a->a02 op b->a02;\
		r.a10 = a->a10 op b->a10;\
		r.a11 = a->a11 op b->a11;\
		r.a12 = a->a12 op b->a12;\
		r.a20 = a->a20 op b->a20;\
		r.a21 = a->a21 op b->a21;\
		r.a22 = a->a22 op b->a22;\
		return r;\
	}
#define DEFINE_M44COMPWISE_OP( name, op )\
	m44 name( const m44 *a, const m44 *b ) {\
		m44 r;\
		r.a00 = a->a00 op b->a00;\
		r.a01 = a->a01 op b->a01;\
		r.a02 = a->a02 op b->a02;\
		r.a03 = a->a03 op b->a03;\
		r.a10 = a->a10 op b->a10;\
		r.a11 = a->a11 op b->a11;\
		r.a12 = a->a12 op b->a12;\
		r.a13 = a->a13 op b->a13;\
		r.a20 = a->a20 op b->a20;\
		r.a21 = a->a21 op b->a21;\
		r.a22 = a->a22 op b->a22;\
		r.a23 = a->a23 op b->a23;\
		r.a30 = a->a30 op b->a30;\
		r.a31 = a->a31 op b->a31;\
		r.a32 = a->a32 op b->a32;\
		r.a33 = a->a33 op b->a33;\
		return r;\
	}
#endif

#define DEFINE_S22COMPWISE_OP( name, op )\
	m22 name( const m22 *a, const f32 c ) {\
		m22 r;\
		r.a00 = a->a00 op c;\
		r.a01 = a->a01 op c;\
		r.a10 = a->a10 op c;\
		r.a11 = a->a11 op c;\
		return r;\
	}
#define DEFINE_S33COMPWISE_OP( name, op )\
	m33 name( const m33 *a, const f32 c ) {\
		m33 r;\
		r.a00 = a->a00 op c;\
		r.a01 = a->a01 op c;\
		r.a02 = a->a02 op c;\
		r.a10 = a->a10 op c;\
		r.a11 = a->a11 op c;\
		r.a12 = a->a12 op c;\
		r.a20 = a->a20 op c;\
		r.a21 = a->a21 op c;\
		r.a22 = a->a22 op c;\
		return r;\
	}
#define DEFINE_S44COMPWISE_OP( name, op )\
	m44 name( const m44 *a, const f32 c ) {\
		m44 r;\
		r.a00 = a->a00 op c;\
		r.a01 = a->a01 op c;\
		r.a02 = a->a02 op c;\
		r.a03 = a->a03 op c;\
		r.a10 = a->a10 op c;\
		r.a11 = a->a11 op c;\
		r.a12 = a->a12 op c;\
		r.a13 = a->a13 op c;\
		r.a20 = a->a20 op c;\
		r.a21 = a->a21 op c;\
		r.a22 = a->a22 op c;\
		r.a23 = a->a23 op c;\
		r.a30 = a->a30 op c;\
		r.a31 = a->a31 op c;\
		r.a32 = a->a32 op c;\
		r.a33 = a->a33 op c;\
		return r;\
	}

DEFINE_M22COMPWISE_OP( madd22, + )
DEFINE_M22COMPWISE_OP( msub22, - )

DEFINE_M33COMPWISE_OP( madd33, + )
DEFINE_M33COMPWISE_OP( msub33, - )

DEFINE_M44COMPWISE_OP( madd44, + )
DEFINE_M44COMPWISE_OP( msub44, - )

DEFINE_S22COMPWISE_OP( madds22, + )
DEFINE_S22COMPWISE_OP( msubs22, - )
DEFINE_S22COMPWISE_OP( mdivs22, / )
DEFINE_S22COMPWISE_OP( mmuls22, * )

DEFINE_S33COMPWISE_OP( madds33, + )
DEFINE_S33COMPWISE_OP( msubs33, - )
DEFINE_S33COMPWISE_OP( mdivs33, / )
DEFINE_S33COMPWISE_OP( mmuls33, * )

DEFINE_S44COMPWISE_OP( madds44, + )
DEFINE_S44COMPWISE_OP( msubs44, - )
DEFINE_S44COMPWISE_OP( mdivs44, / )
DEFINE_S44COMPWISE_OP( mmuls44, * )

#undef DEFINE_M22COMPWISE_OP
#undef DEFINE_M33COMPWISE_OP
#undef DEFINE_M44COMPWISE_OP

m22 mcopy2( const m22 *m ) {
	m22 r;
	for( u32 i = 0; i < 4; ++i ) {
		r.A[ i ] = m->A[ i ];
	}
	return r;
}
m33 mcopy3( const m33 *m ) {
	m33 r;
	for( u32 i = 0; i < 9; ++i ) {
		r.A[ i ] = m->A[ i ];
	}
	return r;
}
m44 mcopy4( const m44 *m ) {
	m44 r;
	for( u32 i = 0; i < 16; ++i ) {
		r.A[ i ] = m->A[ i ];
	}
	return r;
}

m22 mmul22( const m22 *a, const m22 *b ) {
	m22 r;
	r.a00 = a->a00 * b->a00 + a->a10 * b->a01;
	r.a01 = a->a01 * b->a00 + a->a11 * b->a01;
	r.a10 = a->a00 * b->a10 + a->a10 * b->a11;
	r.a11 = a->a01 * b->a10 + a->a11 * b->a11;
	return r;
}
m33 mmul33( const m33 *a, const m33 *b ) {
	m33 r;
	r.a00 = a->a00 * b->a00 + a->a10 * b->a01 + a->a20 * b->a02;
	r.a01 = a->a01 * b->a00 + a->a11 * b->a01 + a->a21 * b->a02;
	r.a02 = a->a02 * b->a00 + a->a12 * b->a01 + a->a22 * b->a02;
	r.a10 = a->a00 * b->a10 + a->a10 * b->a11 + a->a20 * b->a12;
	r.a11 = a->a01 * b->a10 + a->a11 * b->a11 + a->a21 * b->a12;
	r.a12 = a->a02 * b->a10 + a->a12 * b->a11 + a->a22 * b->a12;
	r.a20 = a->a00 * b->a20 + a->a10 * b->a21 + a->a20 * b->a22;
	r.a21 = a->a01 * b->a20 + a->a11 * b->a21 + a->a21 * b->a22;
	r.a22 = a->a02 * b->a20 + a->a12 * b->a21 + a->a22 * b->a22;
	return r;
}
m44 mmul44( const m44 *a, const m44 *b ) {
	m44 r;
	r.a00 = a->a00 * b->a00 + a->a10 * b->a01 + a->a20 * b->a02 + a->a30 * b->a03;
	r.a01 = a->a01 * b->a00 + a->a11 * b->a01 + a->a21 * b->a02 + a->a31 * b->a03;
	r.a02 = a->a02 * b->a00 + a->a12 * b->a01 + a->a22 * b->a02 + a->a32 * b->a03;
	r.a03 = a->a03 * b->a00 + a->a13 * b->a01 + a->a23 * b->a02 + a->a33 * b->a03;
	r.a10 = a->a00 * b->a10 + a->a10 * b->a11 + a->a20 * b->a12 + a->a30 * b->a13;
	r.a11 = a->a01 * b->a10 + a->a11 * b->a11 + a->a21 * b->a12 + a->a31 * b->a13;
	r.a12 = a->a02 * b->a10 + a->a12 * b->a11 + a->a22 * b->a12 + a->a32 * b->a13;
	r.a13 = a->a03 * b->a10 + a->a13 * b->a11 + a->a23 * b->a12 + a->a33 * b->a13;
	r.a20 = a->a00 * b->a20 + a->a10 * b->a21 + a->a20 * b->a22 + a->a30 * b->a23;
	r.a21 = a->a01 * b->a20 + a->a11 * b->a21 + a->a21 * b->a22 + a->a31 * b->a23;
	r.a22 = a->a02 * b->a20 + a->a12 * b->a21 + a->a22 * b->a22 + a->a32 * b->a23;
	r.a23 = a->a03 * b->a20 + a->a13 * b->a21 + a->a23 * b->a22 + a->a33 * b->a23;
	r.a30 = a->a00 * b->a30 + a->a10 * b->a31 + a->a20 * b->a32 + a->a30 * b->a33;
	r.a31 = a->a01 * b->a30 + a->a11 * b->a31 + a->a21 * b->a32 + a->a31 * b->a33;
	r.a32 = a->a02 * b->a30 + a->a12 * b->a31 + a->a22 * b->a32 + a->a32 * b->a33;
	r.a33 = a->a03 * b->a30 + a->a13 * b->a31 + a->a23 * b->a32 + a->a33 * b->a33;
	return r;
}

m22 mlerp22( const m22 *a, const m22 *b, const f32 t ) {
	m22 r;
	f32 t1 = 1.f - t;
	r.a00 = a->a00 * t1 + b->a00 * t;
	r.a01 = a->a01 * t1 + b->a01 * t;
	r.a10 = a->a10 * t1 + b->a10 * t;
	r.a11 = a->a11 * t1 + b->a11 * t;
	return r;
}
m33 mlerp33( const m33 *a, const m33 *b, const f32 t ) {
	m33 r;
	f32 t1 = 1.f - t;
	for( word i = 0; i < 9; ++i ) {
		r.A[ i ] = a->A[ i ] * t1 + b->A[ i ] * t;
	}
	return r;
}
m44 mlerp44( const m44 *a, const m44 *b, const f32 t ) {
	m44 r;
	f32 t1 = 1.f - t;
	for( word i = 0; i < 16; ++i ) {
		r.A[ i ] = a->A[ i ] * t1 + b->A[ i ] * t;
	}
	return r;
}

m22 mtranspose22( const m22 *m ) {
	m22 r;
	r.a00 = m->a00; r.a10 = m->a01;
	r.a01 = m->a10; r.a11 = m->a11;
	return r;
}
m33 mtranspose33( const m33 *m ) {
	m33 r;
	r.a00 = m->a00; r.a10 = m->a01; r.a20 = m->a02;
	r.a01 = m->a10; r.a11 = m->a11; r.a21 = m->a12;
	r.a02 = m->a20; r.a12 = m->a21; r.a22 = m->a22;
	return r;
}
m44 mtranspose44( const m44 *m ) {
	m44 r;
	r.a00 = m->a00; r.a10 = m->a01; r.a20 = m->a02; r.a30 = m->a03;
	r.a01 = m->a10; r.a11 = m->a11; r.a21 = m->a12; r.a31 = m->a13;
	r.a02 = m->a20; r.a12 = m->a21; r.a22 = m->a22; r.a32 = m->a23;
	r.a03 = m->a30; r.a13 = m->a31; r.a23 = m->a32; r.a33 = m->a33;
	return r;
}

f32 mdeterminant22( const m22 *m ) {
	return m->a00 * m->a11 - m->a10 * m->a01;
}
f32 mdeterminant33( const m33 *m ) {
	return ( m->a11 * m->a22 - m->a21 * m->a12 )
		 - ( m->a01 * m->a22 - m->a21 * m->a12 )
		 + ( m->a01 * m->a12 - m->a11 * m->a02 );
}
f32 mdeterminant44( const m44 *m ) {
	f32 det = 0.f;
	for( word i = 0; i < 4; ++i ) {
		m33 a;
		for( word c = 1; c < 4; ++c ) {
			for( word r = 0, k = 0; r < 4; ++r ) {
				if( r == i ) continue;
				a.A[ ( c - 1 ) * 3 + k++ ] = m->A[ c * 4 + r ];
			}
		}
		det += powf( -1.f, ( f32 )i ) * mdeterminant33( &a );
	}
	return det;
}

m22 minverse22( const m22 *m ) {
	m22 r;
	f32 f = 1.f / mdeterminant22( m );
	r.a00 = f *  m->a11;
	r.a01 = f * -m->a01;
	r.a10 = f * -m->a10;
	r.a11 = f *  m->a00;
	return r;
}
m33 minverse33( const m33 *m ) {
	m22 tmp;
	m33 cofac;
	f32 d = mdeterminant33( m );
	for( word c = 0; c < 3; ++c ) {
		for( word r = 0; r < 3; ++r ) {
			for( word j = 0, b = 0; j < 3; ++j ) {
				if( j == c ) continue;
				for( word i = 0, a = 0; i < 3; ++i ) {
					if( i == r ) continue;
					tmp.A[ b * 2 + a++ ] = m->A[ j * 3 + i ];
				}
				++b;
			}
			cofac.A[ r * 3 + c ] = mdeterminant22( &tmp );
		}
	}
	return mmuls33( &cofac, 1.f / d );
}
m44 minverse44( const m44 *m ) {
	m33 tmp;
	m44 cofac;
	f32 d = mdeterminant44( m );
	for( word c = 0; c < 4; ++c ) {
		for( word r = 0; r < 4; ++r ) {
			for( word j = 0, b = 0; j < 4; ++j ) {
				if( j == c ) continue;
				for( word i = 0, a = 0; i < 4; ++i ) {
					if( i == r ) continue;
					tmp.A[ b * 3 + a++ ] = m->A[ j * 4 + i ];
				}
				++b;
			}
			cofac.A[ r * 4 + c ] = mdeterminant33( &tmp );
		}
	}
	return mmuls44( &cofac, 1.f / d );
}

m22 mtruncate32( const m33 *m ) {
	m22 r;
	r.a00 = m->a00;
	r.a01 = m->a01;
	r.a10 = m->a10;
	r.a11 = m->a11;
	return r;
}
m22 mtruncate42( const m44 *m ) {
	m22 r;
	r.a00 = m->a00;
	r.a01 = m->a01;
	r.a10 = m->a10;
	r.a11 = m->a11;
	return r;
}
m33 mtruncate43( const m44 *m ) {
	m33 r;
	r.a00 = m->a00;
	r.a01 = m->a01;
	r.a02 = m->a02;
	r.a10 = m->a10;
	r.a11 = m->a11;
	r.a12 = m->a12;
	r.a20 = m->a20;
	r.a21 = m->a21;
	r.a22 = m->a22;
	return r;
}

v2 vmulm2( const m22 *m, const v2 v ) {
	v2 r;
	r.x = m->a00 * v.x + m->a10 * v.y;
	r.y = m->a01 * v.x + m->a11 * v.y;
	return r;
}
v3 vmulm3( const m33 *m, const v3 v ) {
	v3 r;
	r.x = m->a00 * v.x + m->a10 * v.y + m->a20 * v.z;
	r.y = m->a01 * v.x + m->a11 * v.y + m->a21 * v.z;
	r.z = m->a02 * v.x + m->a12 * v.y + m->a22 * v.z;
	return r;
}
v4 vmulm4( const m44 *m, const v4 v ) {
	v4 r;
	r.x = m->a00 * v.x + m->a10 * v.y + m->a20 * v.z + m->a30 * v.w;
	r.y = m->a01 * v.x + m->a11 * v.y + m->a21 * v.z + m->a31 * v.w;
	r.z = m->a02 * v.x + m->a12 * v.y + m->a22 * v.z + m->a32 * v.w;
	r.w = m->a03 * v.x + m->a13 * v.y + m->a23 * v.z + m->a33 * v.w;
	return r;
}

#ifdef _cplusplus
}
#endif

#endif // VUL_DEFINE

#ifndef VUL_TYPES_H
#undef f32
#undef f64
#endif // VUL_TYPES_H

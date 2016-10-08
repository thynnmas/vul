/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file contains functions to generate common 3D shapes for rendering.
 * Currently implemented:
 * - Spheres by subdivision from tetrahedron.
 * TODO(thynn): Should really also include:
 * - Box
 * - Plane
 * - Cylinder
 * - Cone
 * - Capsule
 * - Torus
 *
 * ? If public domain is not legally valid in your legal jurisdiction
 *   the MIT licence applies (see the LICENCE file)
 */
#ifndef VUL_SHAPES_H
#define VUL_SHAPES_H

#ifndef VUL_TYPES_H
#include <stdint.h>
#define f32 float
#define u32 uint32_t
#endif

#include <assert.h>

#ifndef VUL_SHAPES_INDEX_TYPE
#define VUL_SHAPES_INDEX_TYPE u32
#endif
#define idx_type VUL_SHAPES_INDEX_TYPE

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Generate a tetrahedron with the given side length. Centered at origin.
 * If verts points to a nullptr it is allocated, otherwise it is filled in place
 * and must have room for 4 vertices of 3 floats each.
 * If indices points to anullptr it is allocated, otherwise it is filled in place
 * and must have room for 4 time 3 indices (u32).
 */
void vul_shape_tetrahedron( f32 **verts, u32 **indices, f32 side_length );

/*
 * Calculates the number of vertices and faces of a sphere constructed by recursively
 * refining a tetrahedon recusion_level times, splitting each face into 6 subfaces.
 */
void vul_shape_sphere_refine_tetrahedron_faces_verts( u32 *vertices, u32 *faces, u32 recursion_level );

/*
 * Generate sphere by recursively refining a tetrahedron. Centered at origin.
 * If verts points to a nullptr it is allocated, otherwise it is filled in place.
 * If indices points to anullptr it is allocated, otherwise it is filled in place.
 * The helper function vul_shape_sphere_refine_tetrahedron_faces_verts can be used
 * to calculate the number of vertices and faces; the buffers must fit these (tightly packed)
 * meaning at least 3 * sizeof(f32) * vertices and 3 * sizeof(index) * faces.
 */
void vul_shape_sphere_refine_tetrahedron( f32 **verts, idx_type **indices, u32 recursion_level, f32 radius );

#ifdef __cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef f32
#undef u32
#endif

#endif // VUL_SHAPES_H

#ifdef VUL_DEFINE

#ifndef VUL_TYPES_H
#define f32 float
#define u32 uint32_t
#endif

#ifdef __cplusplus
extern "C" {
#endif

void vul_shape_tetrahedron( f32 **verts, u32 **indices, f32 side_length )
{
   if( !*verts ) {
      *verts = ( f32* )malloc( sizeof( f32 ) * 12 );
   }
   if( !*indices ) {
      *indices = ( u32* )malloc( sizeof( u32 ) * 12 );
   }
   ( *verts )[ 0 ] =  size_length; ( *verts )[ 1 ]  =  size_length; ( *verts )[ 2 ]  =  size_length;
   ( *verts )[ 3 ] =  size_length; ( *verts )[ 4 ]  = -size_length; ( *verts )[ 5 ]  = -size_length;
   ( *verts )[ 6 ] = -size_length; ( *verts )[ 7 ]  =  size_length; ( *verts )[ 8 ]  = -size_length;
   ( *verts )[ 9 ] = -size_length; ( *verts )[ 10 ] = -size_length; ( *verts )[ 11 ] =  size_length;
   ( *indices )[ 0 ] = 0; ( *indices )[ 1 ]  = 1; ( *indices )[ 2 ]  = 2;
   ( *indices )[ 3 ] = 0; ( *indices )[ 4 ]  = 1; ( *indices )[ 5 ]  = 3;
   ( *indices )[ 6 ] = 0; ( *indices )[ 7 ]  = 2; ( *indices )[ 8 ]  = 3;
   ( *indices )[ 9 ] = 1; ( *indices )[ 10 ] = 2; ( *indices )[ 11 ] = 3;
}

void vul_shape_sphere_refine_tetrahedron_faces_verts( u32 *vertices, u32 *faces, u32 recursion_level )
{
   u32 f = 4;
   u32 v = 4;
   for( u32 i = 1; i < recursion_level; ++i ) {
      v += f * 3;
      f *= 4;
   }
   *vertices = v;
   *faces = f;
}

void vul_shape_sphere_refine_tetrahedron( f32 **verts, idx_type **indices, u32 recursion_level, f32 radius )
{
   u32 faces, vertices;
   vul_shape_sphere_refine_tetrahedron_faces_verts( &vertices, &faces, recursion_level );

   if( !*verts ) {
      *verts = ( f32* )malloc( sizeof(f32) * vertices * 3 );
   }
   if( !*indices ) {
      *indices = ( idx_type* )malloc( sizeof( idx_type ) * faces * 3 );
   }

   f32 *tverts = &( *verts )[ vertices * 3 - 4 * 3 ];
   idx_type *tindices = &( *indices )[ faces * 3 - 4 * 3 ];
   vul_shape_tetrahedron( &tverts, &tindices, radius / sqrtf( 3.f ) );

   u32 level_vertices = 4;
   u32 level_faces = 4;
   for( u32 i = 1; i <= recursion_level; ++i ) {
      u32 last_vertices = level_vertices;
      level_vertices += level_faces * 3;
      level_faces *= 4;
      f32 *nverts = &( *verts )[ vertices * 3 - level_vertices * 3];
      idx_type *nindices = &( *indices )[ faces * 3 - level_faces * 3];
      for( u32 idx = 0, vidx = 0, iidx = 0; idx < level_faces / 2; idx += 3 ) {
         idx_type idx0 = tindices[ idx     ];
         idx_type idx1 = tindices[ idx + 1 ];
         idx_type idx2 = tindices[ idx + 2 ];

         idx_type idx3 = vidx / 3;

         f32 midx = ( tverts[ idx0 * 3     ] + tverts[ idx1 * 3     ] ) / 2.f;
         f32 midy = ( tverts[ idx0 * 3 + 1 ] + tverts[ idx1 * 3 + 1 ] ) / 2.f;
         f32 midz = ( tverts[ idx0 * 3 + 2 ] + tverts[ idx1 * 3 + 2 ] ) / 2.f;
         f32 scale = radius / sqrtf( midx * midx + midy * midy + midz * midz );
         midx *= scale;
         midy *= scale;
         midz *= scale;
         nverts[ vidx++ ] = midx;
         nverts[ vidx++ ] = midy;
         nverts[ vidx++ ] = midz;
         midx = ( tverts[ idx0 * 3     ] + tverts[ idx2 * 3     ] ) / 2.f;
         midy = ( tverts[ idx0 * 3 + 1 ] + tverts[ idx2 * 3 + 1 ] ) / 2.f;
         midz = ( tverts[ idx0 * 3 + 2 ] + tverts[ idx2 * 3 + 2 ] ) / 2.f;
         scale = radius / sqrtf( midx * midx + midy * midy + midz * midz );
         midx *= scale;
         midy *= scale;
         midz *= scale;
         nverts[ vidx++ ] = midx;
         nverts[ vidx++ ] = midy;
         nverts[ vidx++ ] = midz;
         midx = ( tverts[ idx1 * 3     ] + tverts[ idx2 * 3     ] ) / 2.f;
         midy = ( tverts[ idx1 * 3 + 1 ] + tverts[ idx2 * 3 + 1 ] ) / 2.f;
         midz = ( tverts[ idx1 * 3 + 2 ] + tverts[ idx2 * 3 + 2 ] ) / 2.f;
         scale = radius / sqrtf( midx * midx + midy * midy + midz * midz );
         midx *= scale;
         midy *= scale;
         midz *= scale;
         nverts[ vidx++ ] = midx;
         nverts[ vidx++ ] = midy;
         nverts[ vidx++ ] = midz;

         nindices[ iidx++ ] = idx0 + ( level_faces / 4 ) * 3;
         nindices[ iidx++ ] = idx3;
         nindices[ iidx++ ] = idx3 + 1;
         nindices[ iidx++ ] = idx1 + ( level_faces / 4 ) * 3;
         nindices[ iidx++ ] = idx3 + 2;
         nindices[ iidx++ ] = idx3;
         nindices[ iidx++ ] = idx2 + ( level_faces / 4 ) * 3;
         nindices[ iidx++ ] = idx3 + 1;
         nindices[ iidx++ ] = idx3 + 2;
         nindices[ iidx++ ] = idx3;
         nindices[ iidx++ ] = idx3 + 2;
         nindices[ iidx++ ] = idx3 + 1;
      }
      tverts = nverts;
      tindices = nindices;
   }
}

#undef idx_type

#ifndef VUL_TYPES_H
#undef f32
#undef u32
#endif

#endif

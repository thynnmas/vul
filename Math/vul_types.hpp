/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file defines shorter type names for the common math types in VUL.
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
 *   the MIT licence applies (see the LICENCE file)
 */

#ifndef VUL_TYPES_HPP
#define VUL_TYPES_HPP

#include "vul_half.hpp" // @TODO: Half precision float
#include "vul_fixed.hpp" // @TODO: Fixed precision for 64 and 32 bits (48.16 / 24.8)

namespace vul {

	// Floats
	typedef half f16_t;
	typedef float f32_t;
	typedef double f64_t;

	// Fixed precision
	typedef fixed_32< 8 > fi32_t;	/* Q.24.8 */

	// Signed Integers
	typedef char i8_t;
	typedef short i16_t;
	typedef int i32_t;
	typedef long long i64_t;

	// Unsigned integers
	typedef unsigned char ui8_t;
	typedef unsigned short ui16_t;
	typedef unsigned int ui32_t;
	typedef unsigned long long ui64_t;

}

#endif
/*
 * File:   operations_native.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 *
 * This file should never be included directly but only via operations.h
 */

#pragma once

#ifndef INCLUDED_FROM_NUMBERS_H
static_assert(false, "This file may only be included indirectly by numbers.h");
#endif

#include <cassert>
#include <cmath>
#include <limits>

namespace carl {

/**
 * Informational functions
 *
 * The following functions return informations about the given numbers.
 */

inline bool isNumber(const double& d) {
	return (d == d) && !std::isinf(d);
}

inline bool isInteger(const double& d) {
	double tmp;
	return std::modf(d, &tmp) == 0.0;
}

inline bool isInteger(const int&) {
	return true;
}

inline bool isNegative(const double& d) {
	return d < 0;
}

inline bool isPositive(const double& d) {
	return d > 0;
}

inline std::size_t bitsize(unsigned) {
	return sizeof(unsigned) * 8;
}

template<typename C>
inline void reserve(std::size_t) {
}

/**
 * Conversion functions
 *
 * The following function convert types to other types.
 */

inline double toDouble(const int& n) {
	return double(n);
}
inline double toDouble(const double& n) {
	return n;
}

/**
 * Basic Operators
 *
 * The following functions implement simple operations on the given numbers.
 */
inline double floor(double n) {
	return std::floor(n);
}
inline double ceil(double n) {
	return std::ceil(n);
}
inline double abs(double n) {
	return std::abs(n);
}

inline long mod(const long& n, const long& m) {
	return n % m;
}
inline unsigned long mod(const unsigned long& n, const unsigned long& m) {
	return n % m;
}
inline unsigned mod(const unsigned& n, const unsigned& m) {
	return n % m;
}
inline int mod(const int& n, const int& m) {
	return n % m;
}

inline int remainder(const int& n, const int& m) {
	return n % m;
}
inline int div(const int& n, const int& m) {
	assert(n % m == 0);
	return n / m;
}
inline int quotient(const int& n, const int& m) {
	return n / m;
}

inline double sin(double in) {
	return std::sin(in);
}

inline double cos(double in) {
	return std::cos(in);
}

inline double acos(double in) {
	return std::acos(in);
}

inline double sqrt(double in) {
	return std::sqrt(in);
}

inline double pow(double in, size_t exp) {
	return std::pow(in,exp);
}


/**
 * Returns the highest power of two below n.
 *
 * Can also be seen as the highest bit set in n.
 * @param n
 * @return
 */
template<typename Number>
inline Number highestPower(const Number& n) {
	unsigned iterations = 0;
	// Number has 2^k Bits, we do k iterations
	if (sizeof(Number) == 2) iterations = 4;
	else if (sizeof(Number) == 4) iterations = 5;
	else if (sizeof(Number) == 8) iterations = 6;
	assert(iterations > 0);

	Number res = n;
	for (unsigned i = 0; i < iterations; i++) {
		res |= res >> (1 << i);
	}
	res -= res >> 1;
	return res;
}

}

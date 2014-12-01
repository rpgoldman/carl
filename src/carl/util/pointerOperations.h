/* 
 * @file pointerOperations.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * This file contains generic operations on pointers.
 * We define our own suite of STL like operations like std::equal_to or std::less
 * on pointers (and shared pointers) with our own semantic.
 *
 * We consider two pointers equal, if the pointers themselves are equal or the
 * objects they point to are equal. Similarly, a pointer is smaller than another,
 * if the object it points to is smaller. A nullptr is considered the smallest.
 */

#pragma once

#include <functional>
#include <memory>

namespace carl {

/**
 * Alternative specialization of std::equal_to for pointer types.
 *
 * We consider two pointers equal, if they point to the same memory location or the objects they point to are equal.
 * Note that the memory location may also be zero.
 */
template<typename T, bool mayBeNull = true>
struct equal_to;

template<typename T, bool mayBeNull>
struct equal_to<T*, mayBeNull> {
	bool operator()(const T* lhs, const T* rhs) const {
		if (lhs == rhs) return true;
		if (mayBeNull) {
			if (lhs == nullptr || rhs == nullptr) return false;
		}
		return std::equal_to<T>()(*lhs, *rhs);
	}
};

template<typename T, bool mayBeNull>
struct equal_to<std::shared_ptr<T>, mayBeNull> {
	bool operator()(const std::shared_ptr<const T>& lhs, const std::shared_ptr<const T>& rhs) const {
		if (lhs == rhs) return true;
		if (mayBeNull) {
			if (lhs == nullptr || rhs == nullptr) return false;
		}
		return std::equal_to<T>()(*lhs, *rhs);
	}
};

template<typename T, bool mayBeNull = true>
struct not_equal_to;

template<typename T, bool mayBeNull>
struct not_equal_to<T*, mayBeNull> {
	bool operator()(const T* lhs, const T* rhs) const {
		return !equal_to<T, mayBeNull>()(lhs, rhs);
	}
};

template<typename T, bool mayBeNull>
struct not_equal_to<std::shared_ptr<T>, mayBeNull> {
	bool operator()(const std::shared_ptr<const T>& lhs, const std::shared_ptr<const T>& rhs) const {
		return !equal_to<T, mayBeNull>()(lhs, rhs);
	}
};

/**
 * Alternative specialization of std::equal_to for pointer types.
 *
 * We consider two pointers equal, if they point to the same memory location or the objects they point to are equal.
 * Note that the memory location may also be zero.
 */
template<typename T, bool mayBeNull = true>
struct less;

template<typename T, bool mayBeNull>
struct less<T*, mayBeNull> {
	bool operator()(const T* lhs, const T* rhs) const {
		if (lhs == rhs) return false;
		if (mayBeNull) {
			if (lhs == nullptr || rhs == nullptr) return lhs == nullptr;
		}
		return std::less<T>()(*lhs, *rhs);
	}
};

template<typename T, bool mayBeNull>
struct less<std::shared_ptr<T>, mayBeNull> {
	bool operator()(const std::shared_ptr<const T>& lhs, const std::shared_ptr<const T>& rhs) const {
		if (lhs == rhs) return false;
		if (mayBeNull) {
			if (lhs == nullptr || rhs == nullptr) return lhs == nullptr;
		}
		return std::less<T>()(*lhs, *rhs);
	}
};

template<typename T, bool mayBeNull = true>
struct greater;

template<typename T, bool mayBeNull>
struct greater<T*, mayBeNull> {
	bool operator()(const T* lhs, const T* rhs) const {
		return less<T, mayBeNull>()(rhs, lhs);
	}
};

template<typename T, bool mayBeNull>
struct greater<std::shared_ptr<T>, mayBeNull> {
	bool operator()(const std::shared_ptr<const T>& lhs, const std::shared_ptr<const T>& rhs) const {
		return less<T, mayBeNull>()(rhs, lhs);
	}
};

/**
 * Alternative specialization of std::hash for pointer types.
 *
 * In case the pointer is not a nullptr, we return the hash of the object it points to.
 */
template<typename T, bool mayBeNull = true>
struct hash;

template<typename T, bool mayBeNull>
struct hash<T*, mayBeNull> {
	std::size_t operator()(const T* t) const {
		if (mayBeNull) {
			if (t == nullptr) return 0;
		}
		return std::hash<T>()(*t);
	}
};

template<typename T, bool mayBeNull>
struct hash<std::shared_ptr<T>, mayBeNull> {
	std::size_t operator()(const std::shared_ptr<T>& t) const {
		if (mayBeNull) {
			if (t == nullptr) return 0;
		}
		return std::hash<T>()(*t);
	}
};

}
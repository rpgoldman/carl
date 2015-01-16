/**
 * @file CADPolynomials.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <algorithm>
#include <list>
#include <unordered_map>

#include "CADTypes.h"

namespace carl {
namespace cad {

template<typename Number>
class CADPolynomials : public carl::cad::PolynomialOwner<Number>  {
public:
	/// Type of univariate polynomials.
	typedef carl::cad::UPolynomial<Number> UPolynomial;
	/// Type of multivariate polynomials.
	typedef carl::cad::MPolynomial<Number> MPolynomial;
private:
	/**
	 * List of all polynomials for elimination.
	 */
	std::list<const UPolynomial*> polynomials;

	/**
	 * Maps multivariate polynomials given as input to the univariate polynomials that are used internally.
	 */
	std::unordered_map<const MPolynomial, const UPolynomial*, std::hash<MPolynomial>> map;

	/**
	 * list of polynomials scheduled for elimination
	 */
	std::vector<const UPolynomial*> scheduled;
public:
	CADPolynomials() {}
	CADPolynomials(cad::PolynomialOwner<Number>* parent): cad::PolynomialOwner<Number>(parent) {}
	
	bool hasScheduled() const {
		return !scheduled.empty();
	}
	bool isScheduled(const UPolynomial* up) const {
		return std::find(scheduled.begin(), scheduled.end(), up) != scheduled.end();
	}
	void schedule(const UPolynomial* up) {
		scheduled.push_back(this->take(up));
	}
	void schedule(const MPolynomial& p, const UPolynomial* up) {
		map[p] = up;
		scheduled.push_back(this->take(up));
	}
	void clearScheduled() {
		scheduled.clear();
	}
	auto getScheduled() const -> const decltype(scheduled)& {
		return scheduled;
	}
	
	void addPolynomial(const UPolynomial* up) {
		polynomials.push_back(up);
	}
	auto getPolynomials() const -> const decltype(polynomials)& {
		return polynomials;
	}
	
	const UPolynomial* removePolynomial(const MPolynomial& p) {
		auto it = map.find(p);
		if (it == map.end()) return nullptr;
		
		auto up = it->second;
		map.erase(it);
		
		for (auto it = scheduled.begin(); it != scheduled.end(); ++it) {
			if (*it == up) {
				scheduled.erase(it);
				return up;
			}
		}
		
		for (auto it = polynomials.begin(); it != polynomials.end(); ++it) {
			if (*it == up) {
				polynomials.erase(it);
				return up;
			}
		}
		CARL_LOG_ERROR("carl.cad", "Removing polynomial " << p << " that is neither scheduled nor in polynomials.");
		assert(false);
		return up;
	}
};

template<typename Number>
inline std::ostream& operator<<(std::ostream& os, const CADPolynomials<Number>& p) {
	os << p.getPolynomials() << " // " << p.getScheduled();
	return os;
}

}
}
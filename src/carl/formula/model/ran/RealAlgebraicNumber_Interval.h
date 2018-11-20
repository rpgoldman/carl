#pragma once

#include "../../../core/UnivariatePolynomial.h"
#include "../../../core/polynomialfunctions/RootCounting.h"
#include "../../../core/polynomialfunctions/SturmSequence.h"

#include "../../../interval/Interval.h"

#include <list>

namespace carl {
namespace ran {
  /**
   * FIX isn't this the standard representation of a real algebraic number?
   */
	template<typename Number>
	struct IntervalContent {
		using Polynomial = UnivariatePolynomial<Number>;
		
		static const Variable auxVariable;
		
		Polynomial polynomial;
		Interval<Number> interval;
		std::vector<Polynomial> sturmSequence;
		std::size_t refinementCount;
		
		Polynomial replaceVariable(const Polynomial& p) const {
			return p.replaceVariable(auxVariable);
		}
		
		IntervalContent(
			const Polynomial& p,
			const Interval<Number> i
		):
			polynomial(replaceVariable(p)),
			interval(i),
			sturmSequence(carl::sturm_sequence(p)),
			refinementCount(0)
		{}
		
		IntervalContent(
			const Polynomial& p,
			const Interval<Number> i,
			const std::vector<UnivariatePolynomial<Number>>& seq
		):
			polynomial(replaceVariable(p)),
			interval(i),
			sturmSequence(seq),
			refinementCount(0)
		{}
		bool isIntegral() {
			return interval.isPointInterval() && carl::isInteger(interval.lower());
		}
		
		void setPolynomial(const Polynomial& p) {
			polynomial = replaceVariable(p);
			sturmSequence = carl::sturm_sequence(polynomial);
		}
		
		Sign sgn(const Polynomial& p) const {
			Polynomial tmp = replaceVariable(p);
			if (polynomial == tmp) return Sign::ZERO;
			auto seq = carl::sturm_sequence(polynomial, derivative(polynomial) * tmp);
			int variations = carl::count_real_roots(seq, interval);
			assert((variations == -1) || (variations == 0) || (variations == 1));
			switch (variations) {
				case -1: return Sign::NEGATIVE;
				case 0: return Sign::ZERO;
				case 1: return Sign::POSITIVE;
				default:
					CARL_LOG_ERROR("carl.ran", "Unexpected number of variations, should be -1, 0, 1 but was " << variations);
					return Sign::ZERO;
			}
		}
		
		void refine() {
			Number pivot = carl::sample(interval);
			assert(interval.contains(pivot));
			if (polynomial.isRoot(pivot)) {
				interval = Interval<Number>(pivot, pivot);
			} else {
				if (carl::count_real_roots(polynomial, Interval<Number>(interval.lower(), BoundType::STRICT, pivot, BoundType::STRICT)) > 0) {
					interval.setUpper(pivot);
				} else {
					interval.setLower(pivot);
				}
				refinementCount++;
				assert(interval.isConsistent());
			}
		}
			
		/** Refine the interval i of this real algebraic number yielding the interval j such that !j.meets(n). If true is returned, n is the exact numeric representation of this root. Otherwise not.
		 * @param n
		 * @rcomplexity constant
		 * @scomplexity constant
		 * @return true, if n is the exact numeric representation of this root, otherwise false
		 */
		bool refineAvoiding(const Number& n) {
			if (interval.contains(n)) {
				if (polynomial.isRoot(n)) {
					interval = Interval<Number>(n, n);
					return true;
				}
				if (carl::count_real_roots(polynomial, Interval<Number>(interval.lower(), BoundType::STRICT, n, BoundType::STRICT)) > 0) {
					interval.setUpper(n);
				} else {
					interval.setLower(n);
				}
				refinementCount++;
			} else if (interval.lower() != n && interval.upper() != n) {
				return false;
			}
			
			bool isLeft = interval.lower() == n;
			
			Number newBound = carl::sample(interval);
			
			if (polynomial.isRoot(newBound)) {
				interval = Interval<Number>(newBound, newBound);
				return false;
			}
			
			if (isLeft) {
				interval.setLower(newBound);
			} else {
				interval.setUpper(newBound);
			}
			
			while (carl::count_real_roots(polynomial, interval) == 0) {
				if (isLeft) {
					Number oldBound = interval.lower();
					newBound = carl::sample(Interval<Number>(n, BoundType::STRICT, oldBound, BoundType::STRICT));
					if (polynomial.isRoot(newBound)) {
						interval = Interval<Number>(newBound, newBound);
						return false;
					}
					interval.setUpper(oldBound);
					interval.setLower(newBound);
				} else {
					Number oldBound = interval.upper();
					newBound = carl::sample(Interval<Number>(oldBound, BoundType::STRICT, n, BoundType::STRICT));
					if (polynomial.isRoot(newBound)) {
						interval = Interval<Number>(newBound, newBound);
						return false;
					}
					interval.setLower(oldBound);
					interval.setUpper(newBound);
				}
			}
			return false;
		}
		
		void refineToIntegrality() {
			while (!interval.isPointInterval() && interval.containsInteger()) {
				refine();
			}
		}
	};

	template<typename Number>
	const Variable IntervalContent<Number>::auxVariable = freshRealVariable("__r");
}
}

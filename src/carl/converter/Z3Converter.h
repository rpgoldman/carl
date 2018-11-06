/**
 * @file Z3Converter.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../numbers/numbers.h"

#if defined(USE_Z3_RANS) || defined(USE_Z3_NUMBERS)
#include <utility>

#include "../numbers/adaption_z3/z3.h"

#include "../core/Variable.h"
#include "../core/Monomial.h"
#include "../core/Term.h"
#include "../core/MultivariatePolynomial.h"

namespace carl {

/**
 * Should not be instatiated directly. A singleton instance can
 * be obtained via via carl::z3().
 * 
 * Maybe not thread-safe.
 */
class Z3Converter {
private:
	std::map<carl::Variable, polynomial::var> vars;
	Variable auxVariable = freshRealVariable("__r");

	reslimit rl;
	unsynch_mpq_manager mpq_man;
	polynomial::manager poly_man;
	algebraic_numbers::manager anum_man;

	// TODO refactor: remove operator()
public:

	static void initialize() { // must be called before creating an instance
		initialize_symbols();
        gparams::init();
        rational::initialize();
	}

	Z3Converter() :
		poly_man(rl, mpq_man), anum_man(rl, mpq_man) {}

	// z3 manager classes

	unsynch_mpq_manager& mpqMan() {
		return mpq_man;
	}

	unsynch_mpz_manager& mpzMan() {
		return mpq_man;
	}

	polynomial::manager& polyMan() {
		return poly_man;
	}

	algebraic_numbers::manager& anumMan() {
		return anum_man;
	}

	// free z3 types

	void free(mpz& val) {
		mpzMan().del(val);
	}

	void free(mpq& val) {
		mpqMan().del(val);
	}

	void free(anum& val) {
		anumMan().del(val);
	}

	// rationals and polynomial_refs are free'd automatically:
	// rationals: scoped
	// polynomial_ref: reference counting
	// var: is an unisgned

	// conversions to Z3 types

	/**
	 * Converts a polynomial pointer to a polynomial reference.
     */
	polynomial::polynomial_ref toZ3(polynomial::polynomial* p) {
		return polynomial::polynomial_ref(p, polyMan());
	}
	polynomial::polynomial_ref operator()(polynomial::polynomial* p) {
		return toZ3(p);
	}
	/**
	 * Converts a number.
     */
	rational toZ3(const rational& n) {
		return n;
	}
	rational operator()(const rational& n) {
		return toZ3(n);
	}
    #ifdef USE_CLN_NUMBERS  // TODO remove, deduplicate with carlconverter...
	rational operator()(const cln::cl_RA& n) {
		std::stringstream ss1;
		ss1 << carl::getDenom(n);
		mpz denom;
		mpzMan().set(denom, ss1.str().c_str());
		std::stringstream ss2;
		ss2 << carl::getNum(n);
		mpz num;
		mpzMan().set(num, ss2.str().c_str());
		return rational(num / denom);
	}
    #endif
	mpz toZ3MPZ(const mpz_t z) {
		mpz val;
		mpzMan().set(val, z);
		return val;
		/*
		mpz val;
		if (mpz_fits_slong_p(z)) {
			mpzMan().set(val, mpz_get_si(z));
		} else {
			std::stringstream ss;
			ss << z;
			mpzMan().set(val, ss.str().c_str());
		}
		return val;
		*/
	}
	mpz toZ3MPZ(const mpz_class& n) {
		return toZ3MPZ(n.get_mpz_t());
	}
	mpq toZ3MPQ(const mpq_class& n) {
		mpz den = toZ3MPZ(n.get_den_mpz_t());
		mpz num = toZ3MPZ(n.get_num_mpz_t());
		mpq res;
		mpq_man.set(res, num, den);
		free(den);
		free(num);
		return res;
	}
	rational toZ3Rational(const mpq_class& n) {
		return rational(toZ3MPQ(n));
	}
	rational toZ3(const mpq_class& n) {
		return toZ3Rational(n);
	}
	rational operator()(const mpq_class& n) {
		return toZ3(n);
	}
	/**
	 * Converts a variable.
     */
	polynomial::var toZ3(const carl::Variable& v) {
		auto it = vars.find(v);
		if (it == vars.end()) {
			it = vars.insert(std::make_pair(v, poly_man.mk_var())).first;
		}
		return it->second;
	}
	polynomial::var operator()(const carl::Variable& v) {
		return toZ3(v);
	}
	/**
	 * Converts a variable and an exponent.
     */
	polynomial::polynomial_ref toZ3(const std::pair<carl::Variable, carl::exponent>& p) {
		return toZ3(polyMan().mk_polynomial(toZ3(p.first), p.second));
	}
	polynomial::polynomial_ref operator()(const std::pair<carl::Variable, carl::exponent>& p) {
		return toZ3(p);
	}
	/**
	 * Converts a monomial.
     */
	polynomial::polynomial_ref toZ3(const carl::Monomial& m) {
		polynomial::polynomial_ref res(polyMan());
		res = polyMan().mk_const(rational(1));
		for (auto it: m) {
			res = res * toZ3(it);
		}
		return res;
	}
	polynomial::polynomial_ref operator()(const carl::Monomial& m) {
		return toZ3(m);
	}
	template<typename Coeff>
	polynomial::polynomial_ref toZ3(const carl::Term<Coeff>& t) {
		polynomial::polynomial_ref res(polyMan());
		assert(getDenom(t.coeff()) == 1); // z3 polynomial coefficients must be integer // TODO test
		res = toZ3(polyMan().mk_const(toZ3(t.coeff())));
		if (t.monomial()) return res * toZ3(*(t.monomial()));
		else return res;
	}
	template<typename Coeff>
	polynomial::polynomial_ref operator()(const carl::Term<Coeff>& t) {
		return toZ3(t);
	}
	template<typename Coeff>
	polynomial::polynomial_ref toZ3(const carl::MultivariatePolynomial<Coeff>& p) {
		polynomial::polynomial_ref res(polyMan());
		res = toZ3(polyMan().mk_zero());
		for (auto t: p) res = res + toZ3(t);
		polyMan().lex_sort(res);
		return res;
	}
	template<typename Coeff>
	polynomial::polynomial_ref operator()(const carl::MultivariatePolynomial<Coeff>& p) {
		return toZ3(p);
	}
	template<typename Coeff>
	polynomial::polynomial_ref toZ3(const carl::UnivariatePolynomial<Coeff>& p) {
		polynomial::polynomial_ref res(polyMan());
		res = toZ3(polyMan().mk_zero());
		unsigned exp = 0;
		polynomial::var mainvar = toZ3(p.mainVar());
		for (auto t: p.coefficients()) {
			res = res + toZ3(polyMan().mk_polynomial(mainvar, exp)) * toZ3(t);
			exp++;
		}
		polyMan().lex_sort(res);
		return res;
	}
	template<typename Coeff>
	polynomial::polynomial_ref operator()(const carl::UnivariatePolynomial<Coeff>& p) {
		return toZ3(p);
	}

	// conversions back to CArL types

	template<typename Number>
	Number toNumber(const mpz& m);

	template<typename Number>
	Number toNumber(const mpq& m);

	template<typename Coeff>
	UnivariatePolynomial<Coeff> toUnivPoly(const svector<mpz>& p, carl::Variable var) {
		std::vector<Coeff> coeff;
		for(size_t i = 0; i < p.size(); i++) {
			Coeff c = toNumber<Coeff>(p[i]);
			coeff.push_back(c);
		}
		return UnivariatePolynomial<Coeff>(var, std::move(coeff));
	}

	template<typename Coeff>
	UnivariatePolynomial<Coeff> toUnivPoly(const svector<mpz>& p) {
		return toUnivPoly<Coeff>(p, auxVariable);
	}
};

template<>
inline mpz_class Z3Converter::toNumber<mpz_class>(const mpz& m) {
	mpz_t val;
	mpz_init(val);
	mpzMan().get_mpz_t(m, val);
	mpz_class cls(val);
	mpz_clear(val);
	return cls;
	/*
	if (mpzMan().is_int64(m)) {
		int64_t val = mpzMan().get_int64(m);
		return mpz_class(val);
	} else {
		std::string s = mpzMan().to_string(m);
		return mpz_class(s);
	}
	*/
}

template<>
inline mpq_class Z3Converter::toNumber<mpq_class>(const mpz& m) {
	return mpq_class(toNumber<mpz_class>(m));
}

template<>
inline mpq_class Z3Converter::toNumber<mpq_class>(const mpq& m) {
	mpz znum;
	mpqMan().get_numerator(m, znum);
	mpz zden;
	mpqMan().get_denominator(m, zden);
	mpz_class num = toNumber<mpz_class>(znum);
	mpz_class den = toNumber<mpz_class>(zden);
	mpq_class res(num, den);
	free(znum);
	free(zden);
	return res;
}

Z3Converter& z3();

}
#endif

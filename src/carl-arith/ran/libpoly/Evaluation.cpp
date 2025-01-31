#include "Evaluation.h"

#ifdef USE_LIBPOLY

namespace carl {

std::optional<LPRealAlgebraicNumber> evaluate(
	const LPPolynomial& polynomial,
	const std::map<Variable, LPRealAlgebraicNumber>& evalMap) {

	//Turn into poly::Assignment
	poly::Assignment assignment;
	for (const auto& entry : evalMap) {
		lp_value_t val;
		//Turn into value
		lp_value_construct(&val, lp_value_type_t::LP_VALUE_ALGEBRAIC, entry.second.get_internal());
		//That copies the value into the assignment
		assignment.set(VariableMapper::getInstance().getLibpolyVariable(entry.first), poly::Value(&val));
		lp_value_destruct(&val);
	}

	poly::Value result = poly::Value(lp_polynomial_evaluate(polynomial.get_internal(), assignment.get_internal()));

	CARL_LOG_DEBUG("carl.ran.libpoly", " Result: " << result);

	if(poly::is_none(result)) {
		return std::nullopt;
	}

	return LPRealAlgebraicNumber::create_from_value(std::move(result.get_internal()));
}

boost::tribool evaluate(const BasicConstraint<LPPolynomial>& constraint, const std::map<Variable, LPRealAlgebraicNumber>& evalMap) {

	CARL_LOG_DEBUG("carl.ran.libpoly", " Evaluation constraint " << constraint << " for assignment " << evalMap);

	//Easy checks of lhs of constraint is constant
	if (is_constant(constraint.lhs())) {
		auto num = constraint.lhs().constant_part();
		switch (constraint.relation()) {
		case Relation::EQ:
			return num == 0;
		case Relation::NEQ:
			return num != 0;
		case Relation::LESS:
			return num < 0;
		case Relation::LEQ:
			return num <= 0;
		case Relation::GREATER:
			return num > 0;
		case Relation::GEQ:
			return num >= 0;
		default:
			assert(false);
			return false;
		}
	}

	//denominator can be omitted
	poly::Polynomial poly_pol = constraint.lhs().get_polynomial() ;

	//Turn into poly::Assignment
	poly::Assignment assignment;
	for (const auto& entry : evalMap) {
		lp_value_t val;
		//Turn into value
		lp_value_construct(&val, lp_value_type_t::LP_VALUE_ALGEBRAIC, entry.second.get_internal());
		//That copies the value into the assignment
		assignment.set(VariableMapper::getInstance().getLibpolyVariable(entry.first), poly::Value(&val));
		lp_value_destruct(&val);
	}

	switch (constraint.relation()) {
	case Relation::EQ:
		return evaluate_constraint(poly_pol, assignment, poly::SignCondition::EQ);
	case Relation::NEQ:
		return evaluate_constraint(poly_pol, assignment, poly::SignCondition::NE);
	case Relation::LESS:
		return evaluate_constraint(poly_pol, assignment, poly::SignCondition::LT);
	case Relation::LEQ:
		return evaluate_constraint(poly_pol, assignment, poly::SignCondition::LE);
	case Relation::GREATER:
		return evaluate_constraint(poly_pol, assignment, poly::SignCondition::GT);
	case Relation::GEQ:
		return evaluate_constraint(poly_pol, assignment, poly::SignCondition::GE);
	default:
		assert(false);
		return false;
	}
}

}


#endif
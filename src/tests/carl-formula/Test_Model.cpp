#include "gtest/gtest.h"

#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-formula/model/Model.h>

#include "../Common.h"

template<typename T>
class Model: public testing::Test {};

TYPED_TEST_CASE(Model, RationalTypes);

TYPED_TEST(Model, ModelValue)
{
	using Poly = carl::MultivariatePolynomial<TypeParam>;
	{
		carl::ModelValue<TypeParam,Poly> mv(true);
		EXPECT_TRUE(mv.isBool() && mv.asBool());
	}
	{
		carl::ModelValue<TypeParam,Poly> mv(false);
		EXPECT_TRUE(mv.isBool() && !mv.asBool());
	}
	{
		carl::ModelValue<TypeParam,Poly> mv(TypeParam(3));
		EXPECT_TRUE(mv.isRational() && mv.asRational() == TypeParam(3));
	}
}

TYPED_TEST(Model, ModelSubstitution)
{
	using Poly = carl::MultivariatePolynomial<TypeParam>;
	using ModelPolySubs = carl::ModelPolynomialSubstitution<TypeParam,Poly>;

	carl::Variable x = carl::fresh_real_variable("x");
	carl::Variable y = carl::fresh_real_variable("y");
	carl::MultivariatePolynomial<TypeParam> p(TypeParam(2) * x*x);
	carl::Model<TypeParam,Poly> m;
	m.emplace(carl::ModelVariable(x), TypeParam(3));
	m.emplace(carl::ModelVariable(y), carl::createSubstitution<TypeParam,Poly,ModelPolySubs>(p));
	
	EXPECT_TRUE(m.at(x).isRational());
	EXPECT_TRUE(m.at(x).asRational() == TypeParam(3));
	EXPECT_TRUE(m.at(y).isSubstitution());
}

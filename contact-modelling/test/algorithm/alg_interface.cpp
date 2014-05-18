#include <boost/test/unit_test.hpp>
#include <boost/any.hpp>

#include "cm/algorithm/interface.hpp"

#define PRECOMPUTED_VAL 2.0
#define ONLINE_EXPR(i, pre) i * pre

// mockup
namespace cm {

struct Grid
{
  double v;
};

} /* namespace cm */

struct GridDerived : public cm::Grid
{

};

class MyAlg : public cm::AlgInterface
{
public:
  typedef struct params_type {

  } params_type;
  typedef double precomputed_type;

private:
 boost::any impl_offline(
    const cm::Grid& input,
    const cm::Grid& output,
    const boost::any& params
  )
  {
    const params_type& p = boost::any_cast<const params_type&>(params);
    return PRECOMPUTED_VAL;
  }

  void impl_run(
    const cm::Grid& input,
          cm::Grid& output,
    const boost::any& params,
    const boost::any& precomputed
  )
  {
    const precomputed_type& pre = boost::any_cast<precomputed_type>(precomputed);
    const params_type& p = boost::any_cast<const params_type&>(params);
    output.v = ONLINE_EXPR(input.v, pre); 
  }
};

BOOST_AUTO_TEST_SUITE(algorithm_interface)

BOOST_AUTO_TEST_CASE(test_run_correctly_direct)
{
  GridDerived input;
  input.v = 1;
  GridDerived output;
  MyAlg::params_type params;
  MyAlg a;

  boost::any precomputed = a.offline(input, output, params);
  double raw_precomputed = boost::any_cast<double>(precomputed);
  BOOST_CHECK_EQUAL(
    PRECOMPUTED_VAL,
    raw_precomputed
  );

  a.run(input, output, params, precomputed);
  BOOST_CHECK_EQUAL(
    ONLINE_EXPR(input.v, raw_precomputed),
    output.v
  );
}

BOOST_AUTO_TEST_CASE(test_run_correctly_through_base)
{
  GridDerived input;
  input.v = 1;
  GridDerived output;
  MyAlg::params_type params;
  MyAlg a;
  cm::AlgInterface* a_base = &a;

  boost::any precomputed = a_base->offline(input, output, params);
  double raw_precomputed = boost::any_cast<double>(precomputed);
  BOOST_CHECK_EQUAL(
    PRECOMPUTED_VAL,
    raw_precomputed
  );

  a_base->run(input, output, params, precomputed);
  BOOST_CHECK_EQUAL(
    ONLINE_EXPR(input.v, raw_precomputed),
    output.v
  );
}

BOOST_AUTO_TEST_SUITE_END()

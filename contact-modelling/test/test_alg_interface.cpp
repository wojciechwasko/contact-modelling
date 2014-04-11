#define BOOST_TEST_MODULE alg_interface
#include <boost/test/unit_test.hpp>
#include <boost/any.hpp>

#include "AlgInterface.hpp"

#define PRECOMPUTED_VAL 2.0
#define ONLINE_EXPR(i, pre) i * pre

// mockup
struct MeshInterface
{
  double v;
};

struct MeshDerived : public MeshInterface
{

};

class MyAlg : public AlgInterface
{
public:
  typedef struct params_type {

  } params_type;
  typedef double precomputed_type;

private:
 boost::any impl_offline(
    const MeshInterface& input,
    const MeshInterface& output,
    const boost::any& params
  )
  {
    const params_type& p = boost::any_cast<const params_type&>(params);
    return PRECOMPUTED_VAL;
  }

  void impl_run(
    const MeshInterface& input,
          MeshInterface& output,
    const boost::any& params,
    const boost::any& precomputed
  )
  {
    const precomputed_type& pre = boost::any_cast<precomputed_type>(precomputed);
    const params_type& p = boost::any_cast<const params_type&>(params);
    output.v = ONLINE_EXPR(input.v, pre); 
  }
};

BOOST_AUTO_TEST_CASE(test_run_correctly_direct)
{
  MeshDerived input;
  input.v = 1;
  MeshDerived output;
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
  MeshDerived input;
  input.v = 1;
  MeshDerived output;
  MyAlg::params_type params;
  MyAlg a;
  AlgInterface* a_base = &a;

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

#define BOOST_TEST_MODULE alg_interface
#include <boost/test/unit_test.hpp>
#include <boost/any.hpp>

#include "AlgInterface.hpp"

#define PRECOMPUTED_VAL 2.0
#define ONLINE_EXPR(i, pre) i * pre

template <class T>
class MyAlg; // forward declaration of MyAlg

template <class T>
struct Alg_traits<MyAlg<T>>
{
  typedef T precomputed_type;
  typedef T input_type;
  typedef T output_type;
  typedef struct params_type {
    
  } params_type;
};

template <class T>
class MyAlg : public AlgInterface<MyAlg<T>>
{
  friend class AlgInterface<MyAlg>;
  public:
    INJECT_ALG_INTERFACE_TYPES(MyAlg)

  protected:
   static precomputed_type impl_offline(
      const input_type& input,
      const output_type& output,
      const params_type& params
    )
    {
      return PRECOMPUTED_VAL;
    }

    static void impl_run(
      const input_type& input,
            output_type& output,
      const params_type& params,
      const precomputed_type& precomputed
    )
    {
      output = ONLINE_EXPR(input, precomputed); 
    }
};

BOOST_AUTO_TEST_CASE(test_run_correctly_direct_precomputed)
{
  double input = 1;
  double output;
  MyAlg<double>::params_type params;
  double precomputed = MyAlg<double>::offline(input, output, params);
  BOOST_CHECK_EQUAL(
    PRECOMPUTED_VAL,
    precomputed
  );
  MyAlg<double>::run(input, output, params, precomputed);
  BOOST_CHECK_EQUAL(
    ONLINE_EXPR(input,precomputed),
    output
  );
}

BOOST_AUTO_TEST_CASE(test_run_correctly_boost_any_precomputed)
{
  double input = 1;
  double output;
  MyAlg<double>::params_type params;
  boost::any precomputed = MyAlg<double>::offline(input, output, params);
  double raw_precomputed = boost::any_cast<double>(precomputed);
  BOOST_CHECK_EQUAL(
    PRECOMPUTED_VAL,
    raw_precomputed
  );
  MyAlg<double>::run(input, output, params, precomputed);
  BOOST_CHECK_EQUAL(
    ONLINE_EXPR(input,raw_precomputed),
    output
  );
}

#ifndef ALGFORCESTODISPLACEMENTS_HPP
#define ALGFORCESTODISPLACEMENTS_HPP

#include <type_traits>

#include "AlgInterface.hpp"
#include "helpers/elastic_linear_model.hpp"
#include "external/armadillo.hpp"

template <class I, class O>
class AlgForcesToDisplacements;

template <class I, class O>
struct Alg_traits<AlgForcesToDisplacements<I,O>>
{
  typedef I           input_type;
  typedef O           output_type;
  typedef struct params_type {
    helpers::elastic_linear_model::skin_properties skin_props;
  } params_type;
  typedef arma::mat   precomputed_type;
};

template <class I, class O>
class AlgForcesToDisplacements : public AlgInterface<AlgForcesToDisplacements<I,O>>
{
  friend class AlgInterface<AlgForcesToDisplacements<I,O>>;
  public:
    INJECT_ALG_INTERFACE_TYPES(AlgForcesToDisplacements<I COMMA O>)

  protected:
    static precomputed_type impl_offline(
      input_cref  forces,
      output_cref disps,
      params_cref params
    )
    {
      using helpers::elastic_linear_model::forces_to_displacements_matrix;
      return forces_to_displacements_matrix(forces, disps, params.skin_props);
    }

    static void impl_run(
      output_ref  forces,
      input_cref  disps,
      params_cref params,
      precomputed_cref precomputed
    )
    {
      // perform update, possibly using non-const output.getRawValues() and Armadillo's
      // conv_to<std::vector<double>>
    }

};


#endif /* ALGFORCESTODISPLACEMENTS_HPP */


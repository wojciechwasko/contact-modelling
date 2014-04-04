#ifndef ALGDISPLACEMENTSTOFORCES_HPP
#define ALGDISPLACEMENTSTOFORCES_HPP

#include "AlgInterface.hpp"
#include "helpers/elastic_linear_model.hpp"
#include "external/armadillo.hpp"

template <class I, class O>
class AlgDisplacementsToForces;

template <class I, class O>
struct Alg_traits<AlgDisplacementsToForces<I,O>>
{
  typedef I   input_type;
  typedef O   output_type;
  typedef arma::mat  precomputed_type;
  typedef struct params_type {
    helpers::elastic_linear_model::skin_properties skin_props;
  } params_type;
};

template <class I, class O>
class AlgDisplacementsToForces : public AlgInterface<AlgDisplacementsToForces<I,O>>
{
  friend class AlgInterface<AlgDisplacementsToForces<I,O>>;
  public:
    INJECT_ALG_INTERFACE_TYPES(AlgDisplacementsToForces<I COMMA O>)

  protected:
    static precomputed_type impl_offline(
      input_cref  disps,
      output_cref forces,
      params_cref params
    )
    {
      using helpers::elastic_linear_model::displacements_to_forces_matrix;
      return displacements_to_forces_matrix(disps, forces, params.skin_props);
    }

    static void impl_run(
      input_cref  disps,
      output_ref  forces,
      params_cref params,
      precomputed_cref precomputed
    )
    {
      // perform update, possibly using non-const output.getRawValues() and Armadillo's
      // conv_to<std::vector<double>>
    }

};


#endif /* ALGDISPLACEMENTSTOFORCES_HPP */

#ifndef ALGDISPLACEMENTSTOFORCES_HPP
#define ALGDISPLACEMENTSTOFORCES_HPP

#include "external/armadillo.hpp"

#include "AlgInterface.hpp"

template <class I, class O>
class AlgDisplacementsToForces;

template <class I, class O>
struct Alg_traits<AlgDisplacementsToForces<I,O>>
{
  typedef I   input_type;
  typedef O   output_type;
  typedef arma::mat  precomputed_type;
};

template <class I, class O>
class AlgDisplacementsToForces : public AlgInterface<AlgDisplacementsToForces<I,O>>
{
  friend class AlgInterface<AlgDisplacementsToForces<I,O>>;
  INJECT_ALG_INTERFACE_TYPES(AlgDisplacementsToForces<I COMMA O>)

  protected:
    static precomputed_type impl_offline(const input_type& disps, const output_type& forces)
    {
      precomputed_type ret(forces.getRawValues().size(), disps.getRawValues().size());
      // TODO fill the matrix, maybe take it from AlgForcesToDisplacements and invert?
      return ret;
    }

    static void impl_run(const input_type& disps, output_type& forces, const precomputed_type& precomputed)
    {
      // perform update, possibly using non-const output.getRawValues() and Armadillo's
      // conv_to<std::vector<double>>
    }

};


#endif /* ALGDISPLACEMENTSTOFORCES_HPP */

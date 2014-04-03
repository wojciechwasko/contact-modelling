#ifndef ALGFORCESTODISPLACEMENTS_HPP
#define ALGFORCESTODISPLACEMENTS_HPP

#include "external/armadillo.hpp"

#include "AlgInterface.hpp"

template <class I, class O>
class AlgForcesToDisplacements;

template <class I, class O>
struct Alg_traits<AlgForcesToDisplacements<I,O>>
{
  typedef I   input_type;
  typedef O   output_type;
  typedef arma::mat  precomputed_type;
};

template <class I, class O>
class AlgForcesToDisplacements : public AlgInterface<AlgForcesToDisplacements<I,O>>
{
  friend class AlgInterface<AlgForcesToDisplacements<I,O>>;
  INJECT_ALG_INTERFACE_TYPES(AlgForcesToDisplacements<I COMMA O>)

  protected:
    static precomputed_type impl_offline(const input_type& forces, const output_type& disps)
    {
      precomputed_type ret(disps.getRawValues().size(), forces.getRawValues().size());
      // TODO fill the matrix, maybe take it from AlgForcesToDisplacements and invert?
      return ret;
    }

    static void impl_run(const input_type& forces, output_type& disps, const precomputed_type& precomputed)
    {
      // perform update, possibly using non-const output.getRawValues() and Armadillo's
      // conv_to<std::vector<double>>
    }

};


#endif /* ALGFORCESTODISPLACEMENTS_HPP */


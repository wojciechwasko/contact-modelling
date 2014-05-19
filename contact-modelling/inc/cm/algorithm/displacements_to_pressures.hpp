#ifndef ALGDISPLACEMENTSTOPRESSURES_HPP
#define ALGDISPLACEMENTSTOPRESSURES_HPP
/** 
 * \file
 * \brief   Inverse elastic problem solver (pressures over
 * rectangular area, minimum-norm solution).
 */


#include "cm/algorithm/interface.hpp"
#include "cm/skin/attributes.hpp"

namespace cm {

/**
 * \brief   Inverse Elastic problem -- reconstruct pressures from displacements.
 *
 * Assumes that there is a set of uniform normal pressures acting on the skin
 * over cells of the pressures grid and attempts to reconstruct them, i.e. find
 * a set of pressures which would cause such displacements.
 *
 * Reconstructed pressures correspond to the minimum-norm solution.
 *
 * \note  Only normal displacements are considered.
 *
 * \todo  Expand the algorithms to accept 3d displacements (there are equations
 * out there but they are cumbersome to implement)
 */
class AlgDisplacementsToPressures : public AlgInterface
{
public:
  /**
   * \brief   Parameters for the algorithm -- attributes of the skin
   */
  typedef struct params_type {
    SkinAttributes skin_props;
  } params_type;

private:
  boost::any impl_offline(
    const Grid& disps,
    const Grid& pressures,
    const boost::any& params
  );

  void impl_run(
    const Grid& disps,
          Grid& pressures,
    const boost::any& params,
    const boost::any& precomputed
  );
};

} /* namespace cm */

#endif /* ALGDISPLACEMENTSTOPRESSURES_HPP */

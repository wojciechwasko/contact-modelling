#ifndef ALGDISPLACEMENTSTOFORCES_HPP
#define ALGDISPLACEMENTSTOFORCES_HPP

/** 
 * \file
 * \brief   Inverse elastic problem solver (concentrated forces,
 * minimum-norm solution).
 */

#include "cm/algorithm/interface.hpp"
#include "cm/skin/attributes.hpp"

namespace cm {

/**
 * \brief   Inverse Elastic problem -- reconstruct forces from displacements.
 *
 * Assumes that there is a set of discrete forces acting on the skin,
 * concentrated in locations indicated by cells of the forces grid and attempts
 * to reconstruct them, i.e. find a set of forces which would cause such
 * displacements.
 *
 * Reconstructed forces correspond to the minimum-norm solution.
 */
class AlgDisplacementsToForces : public AlgInterface
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
    const Grid& forces,
    const boost::any& params
  );

  void impl_run(
    const Grid& disps,
          Grid& forces,
    const boost::any& params,
    const boost::any& precomputed
  );
};

} /* namespace cm */

#endif /* ALGDISPLACEMENTSTOFORCES_HPP */

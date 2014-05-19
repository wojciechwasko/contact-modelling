#ifndef ALGDISPLACEMENTSTONONNEGATIVENORMALFORCES_HPP
#define ALGDISPLACEMENTSTONONNEGATIVENORMALFORCES_HPP

/** 
 * \file
 * \brief   Inverse elastic problem solver (concentrated normal
 * forces, nonnegative-only solution).
 */

#include "cm/algorithm/interface.hpp"
#include "cm/skin/attributes.hpp"

namespace cm {

/**
 * \brief   Inverse Elastic problem -- reconstruct forces (only normal, nonnegative) from displacements.
 * \sa AlgDisplacementsToForces
 *
 * This algorithm serches the solutions space for nonnegative forces acting in
 * the z direction (normal to the skin) which would cause such displacements.
 */
class AlgDisplacementsToNonnegativeNormalForces : public AlgInterface
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

#endif /* ALGDISPLACEMENTSTONONNEGATIVENORMALFORCES_HPP */

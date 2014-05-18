#ifndef ALGDISPLACEMENTSTONONNEGATIVEPRESSURES_HPP
#define ALGDISPLACEMENTSTONONNEGATIVEPRESSURES_HPP

/** 
 * \file
 * \brief   Inverse elastic problem solver (pressures over
 * rectangular area, nonnegative-only solution).
 */

#include "cm/algorithm/interface.hpp"
#include "cm/skin/attributes.hpp"

namespace cm {

/**
 * \brief   Inverse Elastic problem -- reconstruct pressures (only normal, nonnegative) from displacements.
 * \sa AlgDisplacementsToPressures
 *
 * This algorithm serches the solutions space for nonnegative pressures acting in
 * the z direction (normal to the skin) which would cause such displacements.
 */
class AlgDisplacementsToNonnegativePressures : public AlgInterface
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

#endif /* ALGDISPLACEMENTSTONONNEGATIVEPRESSURES_HPP */

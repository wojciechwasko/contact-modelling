#ifndef ALGFORCESTODISPLACEMENTS_HPP
#define ALGFORCESTODISPLACEMENTS_HPP

/**
 * \file
 * \brief   Forward Elastic Problem solver (concentrated forces).
 */

#include "cm/algorithm/interface.hpp"
#include "cm/skin/attributes.hpp"

namespace cm {

/**
 * \brief   Forward Elastic Problem -- calculate displacements caused by forces.
 *
 * \sa AlgDisplacementsToForces
 */
class AlgForcesToDisplacements : public AlgInterface
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
    const Grid& forces,
    const Grid& disps,
    const boost::any& params
  );

  void impl_run(
    const Grid& forces,
          Grid& disps,
    const boost::any& params,
    const boost::any& precomputed
  );
};

} /* namespace cm */

#endif /* ALGFORCESTODISPLACEMENTS_HPP */


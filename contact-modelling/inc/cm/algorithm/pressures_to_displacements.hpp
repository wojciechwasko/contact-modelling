#ifndef ALGPRESSURESTODISPLACEMENTS_HPP
#define ALGPRESSURESTODISPLACEMENTS_HPP

/**
 * \file
 * \brief   Forward Elastic Problem solver (normal pressures over rectanglar
 * areas).
 */

#include "cm/algorithm/interface.hpp"
#include "cm/skin/attributes.hpp"

namespace cm {

/**
 * \brief   Forward Elastic Problem -- calculate displacements caused by
 * pressures.
 *
 * \sa AlgDisplacementsToPressures
 */
class AlgPressuresToDisplacements : public AlgInterface
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
    const Grid& pressures,
    const Grid& disps,
    const boost::any& params
  );

  void impl_run(
    const Grid& pressures,
          Grid& disps,
    const boost::any& params,
    const boost::any& precomputed
  );
};

} /* namespace cm */

#endif /* ALGPRESSURESTODISPLACEMENTS_HPP */


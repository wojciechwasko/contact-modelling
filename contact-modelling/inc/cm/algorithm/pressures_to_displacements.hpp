#ifndef ALGPRESSURESTODISPLACEMENTS_HPP
#define ALGPRESSURESTODISPLACEMENTS_HPP

#include "cm/algorithm/interface.hpp"
#include "cm/skin/attributes.hpp"

namespace cm {

class AlgPressuresToDisplacements : public AlgInterface
{
public:
  typedef struct params_type {
    SkinAttributes skin_props;
  } params_type;

private:
  boost::any impl_offline(
    const MeshInterface& pressures,
    const MeshInterface& disps,
    const boost::any& params
  );

  void impl_run(
    const MeshInterface& pressures,
          MeshInterface& disps,
    const boost::any& params,
    const boost::any& precomputed
  );
};

} /* namespace cm */

#endif /* ALGPRESSURESTODISPLACEMENTS_HPP */


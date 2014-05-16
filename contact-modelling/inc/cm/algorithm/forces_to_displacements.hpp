#ifndef ALGFORCESTODISPLACEMENTS_HPP
#define ALGFORCESTODISPLACEMENTS_HPP

#include "cm/algorithm/interface.hpp"
#include "cm/skin/attributes.hpp"

namespace cm {

class AlgForcesToDisplacements : public AlgInterface
{
public:
  typedef struct params_type {
    SkinAttributes skin_props;
  } params_type;

private:
  boost::any impl_offline(
    const MeshInterface& forces,
    const MeshInterface& disps,
    const boost::any& params
  );

  void impl_run(
    const MeshInterface& forces,
          MeshInterface& disps,
    const boost::any& params,
    const boost::any& precomputed
  );
};

} /* namespace cm */

#endif /* ALGFORCESTODISPLACEMENTS_HPP */


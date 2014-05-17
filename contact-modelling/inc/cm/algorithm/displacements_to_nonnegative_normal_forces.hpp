#ifndef ALGDISPLACEMENTSTONONNEGATIVENORMALFORCES_HPP
#define ALGDISPLACEMENTSTONONNEGATIVENORMALFORCES_HPP

#include "cm/algorithm/interface.hpp"
#include "cm/skin/attributes.hpp"

namespace cm {

class AlgDisplacementsToNonnegativeNormalForces : public AlgInterface
{
public:
  typedef struct params_type {
    SkinAttributes skin_props;
  } params_type;

private:
  boost::any impl_offline(
    const MeshInterface& disps,
    const MeshInterface& forces,
    const boost::any& params
  );

  void impl_run(
    const MeshInterface& disps,
          MeshInterface& forces,
    const boost::any& params,
    const boost::any& precomputed
  );
};

} /* namespace cm */

#endif /* ALGDISPLACEMENTSTONONNEGATIVENORMALFORCES_HPP */

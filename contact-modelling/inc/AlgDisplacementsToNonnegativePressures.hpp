#ifndef ALGDISPLACEMENTSTONONNEGATIVEPRESSURES_HPP
#define ALGDISPLACEMENTSTONONNEGATIVEPRESSURES_HPP

#include "SkinAttributes.hpp"
#include "AlgInterface.hpp"
#include "helpers/elastic_linear_model.hpp"

class AlgDisplacementsToNonnegativePressures : public AlgInterface
{
public:
  typedef struct params_type {
    SkinAttributes skin_props;
  } params_type;

private:
  boost::any impl_offline(
    const MeshInterface& disps,
    const MeshInterface& pressures,
    const boost::any& params
  );

  void impl_run(
    const MeshInterface& disps,
          MeshInterface& pressures,
    const boost::any& params,
    const boost::any& precomputed
  );
};



#endif /* ALGDISPLACEMENTSTONONNEGATIVEPRESSURES_HPP */
#ifndef ALGDISPLACEMENTSTOFORCES_HPP
#define ALGDISPLACEMENTSTOFORCES_HPP

#include "AlgInterface.hpp"
#include "helpers/elastic_linear_model.hpp"

class AlgDisplacementsToForces : public AlgInterface
{
public:
  typedef struct params_type {
    helpers::elastic_linear_model::skin_properties skin_props;
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


#endif /* ALGDISPLACEMENTSTOFORCES_HPP */

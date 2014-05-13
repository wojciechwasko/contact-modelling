#ifndef ALGPRESSURESTODISPLACEMENTS_HPP
#define ALGPRESSURESTODISPLACEMENTS_HPP

#include "SkinAttributes.hpp"
#include "AlgInterface.hpp"
#include "helpers/elastic_linear_model.hpp"

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

#endif /* ALGPRESSURESTODISPLACEMENTS_HPP */


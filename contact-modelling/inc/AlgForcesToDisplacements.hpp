#ifndef ALGFORCESTODISPLACEMENTS_HPP
#define ALGFORCESTODISPLACEMENTS_HPP

#include "AlgInterface.hpp"
#include "helpers/elastic_linear_model.hpp"

class AlgForcesToDisplacements : public AlgInterface
{
public:
  typedef struct params_type {
    helpers::elastic_linear_model::skin_properties skin_props;
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

#endif /* ALGFORCESTODISPLACEMENTS_HPP */


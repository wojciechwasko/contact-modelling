#include <iostream>
#include <memory>
#include <stdexcept>

#include <boost/any.hpp>


#include "main.hpp"
#include "SkinConnector.hpp"
#include "SensorValuesConverter.hpp"
#include "Forces.hpp"
#include "MeshNode.hpp"
#include "MeshNatural.hpp"
#include "MeshRegularSquare.hpp"
#include "InterpolatorLinearDelaunay.hpp"
#include "AlgDisplacementsToForces.hpp"
#include "AlgForcesToDisplacements.hpp"

#include "helpers/plot.hpp"

typedef SkinConnector<skin_object>
  skin_connector_type;
typedef MeshNatural<MeshNode<1>>
  natural_mesh_type;
typedef MeshRegularSquare<MeshNode<1>>
  interpolated_mesh_type;
typedef InterpolatorLinearDelaunay<natural_mesh_type, interpolated_mesh_type, NIPP::RemoveFromMesh>
  interpolator_type;
typedef MeshRegularSquare<MeshNode<3>>
  forces_type;
typedef MeshRegularSquare<MeshNode<3>>
  resulting_disps_type;

typedef AlgDisplacementsToForces<interpolated_mesh_type, forces_type> alg_interpolated_disps_to_forces_type;
typedef AlgForcesToDisplacements<forces_type, resulting_disps_type> alg_forces_to_disps;

int main(int argc, char** argv)
{
  std::cout << "Starting!\n";

  std::unique_ptr<skin_connector_type> skin_conn;
  try {
    skin_conn.reset(new skin_connector_type(SensorValuesConverter));
  } catch (const std::exception& e) {
    std::cerr << "Something went wrong with the inialisation:\n"
      << e.what() << "\nExiting...\n";
    return 1;
  }

  // Natural mesh, used "as-is"
  natural_mesh_type natural_mesh_1(skin_conn->sensors_begin(), skin_conn->sensors_end());

  // Natural mesh, used for creation and further interpolating to a RegularMesh
  // later on, I'll probably have the interpolation algorithms spit out the precomputed
  // metadata/cache to be kept outside of the mesh and one will be able to use the same mesh
  // wherever they wish to. Only issue is that, based on the policy, some metadata/cache would have
  // to be deleted as well... I have to wrap my head around this once the headache is over.
  natural_mesh_type natural_mesh_2(skin_conn->sensors_begin(), skin_conn->sensors_end());
  
  interpolated_mesh_type interpolated_mesh(natural_mesh_2, 0.001);

  interpolator_type interpolator(&natural_mesh_2, &interpolated_mesh);

  // TODO implement clone() method with various additions, e.g. shift the mesh to be cloned
  // in the x,y directions (useful for the elastic linear model)
  // NOTE such clone() method would not be a replacement for the copy-ctor; it'd have to support
  // cloning Nodes of different dimensionality
  forces_type forces(natural_mesh_2, 0.0011);
  resulting_disps_type resulting_disps(natural_mesh_2, 0.0012);

  alg_forces_to_disps::params_type                    params_forces_to_disps;
  alg_interpolated_disps_to_forces_type::params_type  params_disps_to_forces;

  boost::any cache_disps_to_forces = alg_interpolated_disps_to_forces_type::offline( 
    interpolated_mesh,
    forces,
    params_disps_to_forces
  );

  boost::any cache_forces_to_disps = alg_forces_to_disps::offline( 
    forces,
    resulting_disps,
    params_forces_to_disps
  );

  std::cout << "Requesting new data.\n";
  skin_conn->update(natural_mesh_1.getRawValues());
  skin_conn->update(natural_mesh_2.getRawValues());
  interpolator.interpolateBulk();
  std::cout << "New data available.\n";

  using helpers::plot::plotMesh;
  plotMesh(natural_mesh_2, "natural");
  plotMesh(interpolated_mesh, "interpolated", true);

  std::cout << "Finishing!\n";
  return 0;
}

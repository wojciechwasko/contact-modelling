#include <iostream>
#include <memory>
#include <stdexcept>

#include <boost/any.hpp>

#include "SkinWareProvider.hpp"
#include "SensorValuesConverter.hpp"
#include "MeshNatural.hpp"
#include "MeshRegularSquare.hpp"
#include "InterpolatorLinearDelaunay.hpp"
#include "AlgDisplacementsToForces.hpp"
#include "AlgForcesToDisplacements.hpp"

#include "helpers/plot.hpp"

typedef SkinWareProvider<skin_object>
  skin_connector;

int main(int argc, char** argv)
{
  std::cout << "Starting!\n";

  std::unique_ptr<skin_connector> skin_conn;
  try {
    skin_conn.reset(new skin_connector(SensorValuesConverter));
  } catch (const std::exception& e) {
    std::cerr << "Something went wrong with the inialisation:\n"
      << e.what() << "\nExiting...\n";
    return 1;
  }

  std::unique_ptr<MeshInterface> natural_mesh_1(skin_conn->createMesh());

  // Natural mesh, used for creation and further interpolating to a RegularMesh
  // later on, I'll probably have the interpolation algorithms spit out the precomputed
  // metadata/cache to be kept outside of the mesh and one will be able to use the same mesh
  // wherever they wish to. Only issue is that, based on the policy, some metadata/cache would have
  // to be deleted as well... I have to wrap my head around this once the headache is over.
  std::unique_ptr<MeshInterface> natural_mesh_2(skin_conn->createMesh());

  std::cout << "Natural mesh: no_nodes == " << natural_mesh_2->no_nodes() << std::endl;
  
  MeshRegularSquare interpolated_mesh(*natural_mesh_2, 0.0005);
  std::cout << "Interpolated mesh: no_nodes == " << interpolated_mesh.no_nodes() << std::endl;

  InterpolatorLinearDelaunay interpolator(NIPP::RemoveFromMesh);

  // TODO implement clone() method with various additions, e.g. shift the mesh to be cloned
  // in the x,y directions (useful for the elastic linear model)
  // NOTE such clone() method would not be a replacement for the copy-ctor; it'd have to support
  // cloning Nodes of different dimensionality
  MeshRegularSquare forces(interpolated_mesh, 0.0061);
  std::cout << "Forces mesh: no_nodes == " << forces.no_nodes() << std::endl;
  MeshRegularSquare resulting_disps(interpolated_mesh, 0.0062);
  std::cout << "Resulting displacements mesh: no_nodes == " << resulting_disps.no_nodes() << std::endl;

  AlgForcesToDisplacements::params_type params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;

  AlgDisplacementsToForces::params_type params_disps_to_forces;
  params_disps_to_forces.skin_props.elasticModulus = 300000;
  params_disps_to_forces.skin_props.skinThickness  = 0.002;

  // boost::any cache_disps_to_forces = alg_interpolated_disps_to_forces_type::offline( 
  //   interpolated_mesh,
  //   forces,
  //   params_disps_to_forces
  // );

  // boost::any cache_forces_to_disps = alg_forces_to_disps::offline( 
  //   forces,
  //   resulting_disps,
  //   params_forces_to_disps
  // );
  
  interpolator.offline(*natural_mesh_2, interpolated_mesh);

  std::cout << "Requesting new data.\n";
  skin_conn->update(natural_mesh_1->getRawValues());
  skin_conn->update(natural_mesh_2->getRawValues());
  interpolator.interpolate(*natural_mesh_2, interpolated_mesh);
  std::cout << "New data available.\n";

  using helpers::plot::plotMesh;
  plotMesh(*natural_mesh_2, "natural");
  plotMesh(interpolated_mesh, "interpolated", true);

  std::cout << "Finishing!\n";
  return 0;
}

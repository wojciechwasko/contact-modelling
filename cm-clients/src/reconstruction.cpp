/**
 * \file  
 * \brief   Source file for an executable which allows to perform contact reconstruction on an input
 *          file.
 */

#include "reconstruction.hpp"

#include <iostream>
#include <string>
#include <memory>
#include <boost/program_options.hpp>

#include "MeshNatural.hpp"
#include "MeshRegularSquare.hpp"
#include "InterpolatorLinearDelaunay.hpp"
#include "SensorValuesConverter.hpp"
#include "SkinYamlProvider.hpp"
#include "AlgDisplacementsToForces.hpp"
#include "AlgForcesToDisplacements.hpp"
#include "helpers/plot.hpp"

namespace po = boost::program_options;
using helpers::plot::plotMesh;

int main(int argc, char** argv)
{
  opts options;
  if (process_options(argc, argv, options))
    return 0;

  SkinYamlProvider skin_provider(SensorValuesConverter, options.input_filename);
  std::unique_ptr<MeshInterface> natural_mesh((MeshInterface*) skin_provider.createMesh());
  std::unique_ptr<MeshInterface> interp_mesh;
  std::unique_ptr<InterpolatorInterface> interpolator;
  if (options.source_pitch > 0) {
    // interpolate
    interp_mesh.reset(new MeshRegularSquare(*natural_mesh, options.source_pitch));
    interpolator.reset(new InterpolatorLinearDelaunay(NIPP::InterpolateToZero));
  }
  std::unique_ptr<MeshInterface> forces_mesh(
    new MeshRegularSquare(*natural_mesh, options.forces_pitch, options.forces_dim)
  );
  std::unique_ptr<MeshInterface> disps_mesh(
    new MeshRegularSquare(*natural_mesh, options.displacements_pitch, options.displacements_dim)
  );

  AlgDisplacementsToForces AlgDToF;
  AlgForcesToDisplacements AlgFToD;

  AlgDisplacementsToForces::params_type AlgDToF_params;
  AlgForcesToDisplacements::params_type AlgFToD_params;

  AlgDToF_params.skin_props = skin_provider.getAttributes();
  AlgFToD_params.skin_props = skin_provider.getAttributes();

  std::cout << "Constructed all the required objects.\n";
  std::cout << "Starting offline calculations.\n";

  std::unique_ptr<MeshInterface>& source_mesh = (interpolator) ? interp_mesh : natural_mesh;

  if (interpolator) {
    std::cout << "Starting offline interpolation phase.\n";
    interpolator->offline(*natural_mesh, *interp_mesh);
    std::cout << "Done with offline interpolation phase.\n";
  }

  std::cout << "Starting disps->forces offline.\n";
  boost::any AlgDToF_offline = AlgDToF.offline(*source_mesh, *forces_mesh, AlgDToF_params);
  std::cout << "Done with disps->forces offline.\n";
  std::cout << "Starting forces->disps offline.\n";
  boost::any AlgFToD_offline = AlgFToD.offline(*forces_mesh, *disps_mesh,  AlgFToD_params);
  std::cout << "Done with forces->disps offline.\n";

  std::cout << "Done with offline calculations.\n";

  std::cout << "Updating data in the mesh.\n"; 
  skin_provider.update(natural_mesh->getRawValues());
  std::cout << "Done updating data in the mesh.\n";

  if (interpolator) {
    std::cout << "Starting online interpolation phase.\n";
    interpolator->interpolate(*natural_mesh, *interp_mesh);
    std::cout << "Done with online interpolation phase.\n";
  }
  std::cout << "Running disps->forces online.\n";
  AlgDToF.run(*source_mesh, *forces_mesh, AlgDToF_params, AlgDToF_offline);
  std::cout << "Done with disps->forces online.\n";
  std::cout << "Running forces->disps online.\n";
  AlgFToD.run(*forces_mesh, *disps_mesh,  AlgFToD_params, AlgFToD_offline);
  std::cout << "Done with forces->disps online.\n";

  plotMesh(*natural_mesh, "Natural mesh");
  if (interpolator)
    plotMesh(*interp_mesh, "Interpolated source mesh", true);
  plotMesh(*forces_mesh, "Forces mesh", true);
  plotMesh(*disps_mesh,  "Reconstructed displacements mesh", true);

  return 0;
}

bool process_options(int argc, char** argv, opts& options)
{

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("input-file,i",
      po::value<std::string>(&options.input_filename)->required(),
      "YAML file to be processed")
    ("source_pitch,s",
      po::value<double>(&options.source_pitch)->default_value(-1),
      "Pitch of the mesh the sensors readings should be interpolated into. If <= 0, no interpolation "
      "will be performed (a natural mesh will be used. Pitch is the distance between two neighbouring "
      "nodes in either x or y direction, in meters. Default: 0.001 [m].")
    ("forces_pitch,f",
      po::value<double>(&options.forces_pitch)->default_value(0.001),
      "Pitch of the forces mesh, i.e. distance between two neighbouring nodes in either x or y "
      "direction, in meters. Default: 0.001 [m].")
    ("forces_dim",
      po::value<size_t>(&options.forces_dim)->default_value(1),
      "Dimensionality of the forces mesh. If 1, only normal forces will be considered. If 3, "
      "three-dimensional forces will be considered. Default: 1.")
    ("displacements_pitch,d",
      po::value<double>(&options.displacements_pitch)->default_value(0.001),
      "Pitch of the (resulting) displacements mesh, i.e. distance between two neighbourint nodes in "
      "either x or y direction, in meters. Default: 0.001 [m].")
    ("displacements_dim",
      po::value<size_t>(&options.displacements_dim)->default_value(1),
      "Dimensionality of the (resulting) displacements mesh. If 1, only normal displacements "
      "will be considered. If 3, three-dimensional displacements will be considered. Default: 1.")
  ;

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  } catch ( const boost::program_options::error& e ) {
    std::cerr << e.what() << std::endl;
    std::cerr << desc     << std::endl;
    return true;
  }

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return true;
  }

  return false;
}


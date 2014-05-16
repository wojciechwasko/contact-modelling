/**
 * \file  
 * \brief   Source file for an executable which allows to perform contact reconstruction on an input
 *          file.
 */

#include "reconstruction-forces.hpp"

#include <iostream>
#include <string>
#include <memory>
#include <cmath>

#include <boost/program_options.hpp>
#include <armadillo>

#include "cm/cm.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  opts options;
  if (process_options(argc, argv, options))
    return 0;

  LOG_SET_LEVEL(DEBUG2)

  cm::SkinProviderYaml skin_provider(options.input_filename);
  std::unique_ptr<cm::MeshInterface> natural_mesh((cm::MeshInterface*) skin_provider.createMesh());
  std::unique_ptr<cm::MeshInterface> interp_mesh;
  std::unique_ptr<cm::InterpolatorInterface> interpolator;
  if (options.source_pitch > 0) {
    // interpolate
    //interp_mesh.reset(new cm::MeshSquareBase(*natural_mesh, options.source_pitch));
    interp_mesh.reset(new cm::MeshRectangularBase(*natural_mesh, options.source_pitch, options.source_pitch));
    interpolator.reset(new cm::InterpolatorLinearDelaunay(cm::NIPP::InterpolateToZero));
  }
  std::unique_ptr<cm::MeshInterface> forces_mesh(
    new cm::MeshSquareBase(*natural_mesh, options.forces_pitch, options.forces_dim)
  );
  std::unique_ptr<cm::MeshInterface> disps_mesh(
    new cm::MeshSquareBase(*natural_mesh, options.displacements_pitch, options.displacements_dim)
  );

  cm::AlgDisplacementsToNonnegativeNormalForces AlgDToF;
  cm::AlgForcesToDisplacements AlgFToD;

  cm::AlgDisplacementsToNonnegativeNormalForces::params_type AlgDToF_params;
  cm::AlgForcesToDisplacements::params_type AlgFToD_params;

  AlgDToF_params.skin_props = skin_provider.getAttributes();
  AlgFToD_params.skin_props = skin_provider.getAttributes();

  LOG(DEBUG1) << "Constructed all the required objects.";
  LOG(DEBUG1) << "Starting offline calculations.";

  std::unique_ptr<cm::MeshInterface>& source_mesh = (interpolator) ? interp_mesh : natural_mesh;

  if (interpolator) {
    LOG(DEBUG2) << "Starting offline interpolation phase.";
    interpolator->offline(*natural_mesh, *interp_mesh);
    LOG(DEBUG2) << "Done with offline interpolation phase.";
  }

  LOG(DEBUG2) << "Starting disps->forces offline.";
  boost::any AlgDToF_offline = AlgDToF.offline(*source_mesh, *forces_mesh, AlgDToF_params);
  LOG(DEBUG2) << "Done with disps->forces offline.";
  LOG(DEBUG2) << "Starting forces->disps offline.";
  boost::any AlgFToD_offline = AlgFToD.offline(*forces_mesh, *disps_mesh,  AlgFToD_params);
  LOG(DEBUG2) << "Done with forces->disps offline.";

  const arma::mat& mFD = boost::any_cast<const arma::mat&>(AlgFToD_offline);
  for (size_t i = 0; i < mFD.n_rows; ++i) {
    if (std::isnan(mFD(i,0))) {
      LOG(DEBUG3) << "x: " << disps_mesh->node(i).x << "y: " << disps_mesh->node(i).y;
    }
  }

  LOG(DEBUG1) << "Done with offline calculations.";

  LOG(DEBUG2) << "Updating data in the mesh."; 
  skin_provider.update(natural_mesh->getRawValues());
  LOG(DEBUG2) << "Done updating data in the mesh.";

  if (interpolator) {
    LOG(DEBUG2) << "Starting online interpolation phase.";
    interpolator->interpolate(*natural_mesh, *interp_mesh);
    LOG(DEBUG2) << "Done with online interpolation phase.";
  }
  LOG(DEBUG2) << "Running disps->forces online.";
  AlgDToF.run(*source_mesh, *forces_mesh, AlgDToF_params, AlgDToF_offline);
  LOG(DEBUG2) << "Done with disps->forces online.";
  LOG(DEBUG2) << "Running forces->disps online.";
  AlgFToD.run(*forces_mesh, *disps_mesh,  AlgFToD_params, AlgFToD_offline);
  LOG(DEBUG2) << "Done with forces->disps online.";

  dumpForPlot(*natural_mesh, "natural");
  if (interpolator)
    dumpForPlot(*interp_mesh, "interpolated");
  dumpForPlot(*forces_mesh, "forces");
  dumpForPlot(*disps_mesh,  "reconstructed");

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


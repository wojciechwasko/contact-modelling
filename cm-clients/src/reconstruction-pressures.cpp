/**
 * \file  
 * \brief   Source file for an executable which allows to perform contact reconstruction on an input
 *          file.
 */

#include "reconstruction-pressures.hpp"

#include <iostream>
#include <string>
#include <memory>
#include <boost/program_options.hpp>
#include <cmath>
#include <algorithm>

#include "MeshNatural.hpp"
#include "MeshRegularSquare.hpp"
#include "InterpolatorLinearDelaunay.hpp"
#include "SkinYamlProvider.hpp"
#include "AlgPressuresToDisplacements.hpp"
#include "AlgDisplacementsToPressures.hpp"
#include "helpers/plot.hpp"
#include "helpers/log.hpp"

#include "external/armadillo.hpp"

namespace po = boost::program_options;
using helpers::plot::dumpForPlot;

int main(int argc, char** argv)
{
  opts options;
  if (process_options(argc, argv, options))
    return 0;

  LOG_SET_LEVEL(DEBUG)

  SkinYamlProvider skin_provider(options.input_filename);
  std::unique_ptr<MeshInterface> natural_mesh((MeshInterface*) skin_provider.createMesh());
  std::unique_ptr<MeshInterface> interp_mesh;
  std::unique_ptr<InterpolatorInterface> interpolator;
  if (options.source_pitch > 0) {
    // interpolate
    interp_mesh.reset(new MeshRegularSquare(*natural_mesh, options.source_pitch));
    interpolator.reset(new InterpolatorLinearDelaunay(NIPP::InterpolateToZero));
  }
  std::unique_ptr<MeshInterface> pressures_mesh(
    new MeshRegularSquare(*natural_mesh, options.pressures_pitch, 1)
  );
  std::unique_ptr<MeshInterface> disps_mesh(
    new MeshRegularSquare(*natural_mesh, options.displacements_pitch, 1)
  );

  AlgDisplacementsToPressures AlgDToP;
  AlgPressuresToDisplacements AlgPToD;

  AlgDisplacementsToPressures::params_type AlgDToP_params;
  AlgPressuresToDisplacements::params_type AlgPToD_params;

  AlgDToP_params.skin_props = skin_provider.getAttributes();
  AlgPToD_params.skin_props = skin_provider.getAttributes();

  LOG(DEBUG1) << "Constructed all the required objects.";
  LOG(DEBUG1) << "Starting offline calculations.";

  std::unique_ptr<MeshInterface>& source_mesh = (interpolator) ? interp_mesh : natural_mesh;

  if (interpolator) {
    LOG(DEBUG2) << "Starting offline interpolation phase.";
    interpolator->offline(*natural_mesh, *interp_mesh);
    LOG(DEBUG2) << "Done with offline interpolation phase.";
  }

  LOG(DEBUG2) << "Starting disps->pressures offline.";
  boost::any AlgDToP_offline = AlgDToP.offline(*source_mesh, *pressures_mesh, AlgDToP_params);
  LOG(DEBUG2) << "Done with disps->pressures offline.";
  LOG(DEBUG2) << "Starting pressures->disps offline.";
  boost::any AlgPToD_offline = AlgPToD.offline(*pressures_mesh, *disps_mesh,  AlgPToD_params);
  LOG(DEBUG2) << "Done with pressures->disps offline.";

  //const arma::mat& mFD = boost::any_cast<const arma::mat&>(AlgPToD_offline);
  //for (size_t i = 0; i < mFD.n_rows; ++i) {
  //  if (std::isnan(mFD(i,0))) {
  //    LOG(DEBUG3) << "x: " << disps_mesh->node(i).x << "y: " << disps_mesh->node(i).y;
  //  }
  //}

  LOG(DEBUG1) << "Done with offline calculations.";

  LOG(DEBUG2) << "Updating data in the mesh."; 
  skin_provider.update(natural_mesh->getRawValues());
  LOG(DEBUG2) << "Done updating data in the mesh.";

  if (interpolator) {
    LOG(DEBUG2) << "Starting online interpolation phase.";
    interpolator->interpolate(*natural_mesh, *interp_mesh);
    LOG(DEBUG2) << "Done with online interpolation phase.";
  }
  LOG(DEBUG2) << "Running disps->pressures online.";
  AlgDToP.run(*source_mesh, *pressures_mesh, AlgDToP_params, AlgDToP_offline);
  LOG(DEBUG2) << "Done with disps->pressures online.";
  LOG(DEBUG2) << "Running pressures->disps online.";
  AlgPToD.run(*pressures_mesh, *disps_mesh,  AlgPToD_params, AlgPToD_offline);
  LOG(DEBUG2) << "Done with pressures->disps online.";

  double total_force = 0;
  for (size_t i = 0; i < pressures_mesh->no_nodes(); ++i) {
    total_force += pressures_mesh->getValue(i, 0) * pressures_mesh->node_area(i);
  }
  std::cout << "Total force: " << total_force << std::endl;

  dumpForPlot(*natural_mesh, "natural");
  if (interpolator)
    dumpForPlot(*interp_mesh, "interpolated");
  dumpForPlot(*pressures_mesh, "pressures");
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
    ("pressures_pitch,p",
      po::value<double>(&options.pressures_pitch)->default_value(0.001),
      "Pitch of the pressures mesh, i.e. distance between two neighbouring nodes in either x or y "
      "direction, in meters. Default: 0.001 [m].")
    ("displacements_pitch,d",
      po::value<double>(&options.displacements_pitch)->default_value(0.001),
      "Pitch of the (resulting) displacements mesh, i.e. distance between two neighbourint nodes in "
      "either x or y direction, in meters. Default: 0.001 [m].")
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


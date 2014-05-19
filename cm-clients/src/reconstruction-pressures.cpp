/**
 * \file  
 * \brief   Source file for an executable which allows to perform contact reconstruction on an input
 *          file.
 */

#include "reconstruction-pressures.hpp"

#include <iostream>
#include <string>
#include <memory>
#include <cmath>
#include <algorithm>

#include <armadillo>
#include <boost/program_options.hpp>

#include "cm/cm.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  opts options;
  if (process_options(argc, argv, options))
    return 0;

  LOG_SET_LEVEL(DEBUG)

  cm::SkinProviderYaml skin_provider(options.input_filename);
  std::unique_ptr<cm::Grid> natural_grid(skin_provider.createGrid());
  std::unique_ptr<cm::Grid> interp_grid;
  std::unique_ptr<cm::InterpolatorInterface> interpolator;
  if (options.source_pitch > 0) {
    // interpolate
    //interp_grid.reset(new cm::MeshSquareBase(*natural_grid, options.source_pitch));
    interp_grid.reset(cm::Grid::fromFill(1, cm::Square(options.source_pitch), *natural_grid));
    interpolator.reset(new cm::InterpolatorLinearDelaunay(cm::NIPP::InterpolateToZero));
  }
  std::unique_ptr<cm::Grid> pressures_grid(
    cm::Grid::fromFill(1, cm::Square(options.pressures_pitch), *natural_grid)
  );
  std::unique_ptr<cm::Grid> disps_grid(
    cm::Grid::fromFill(1, cm::Square(options.displacements_pitch), *natural_grid)
  );

  cm::AlgDisplacementsToPressures AlgDToP;
  cm::AlgPressuresToDisplacements AlgPToD;

  cm::AlgDisplacementsToPressures::params_type AlgDToP_params;
  cm::AlgPressuresToDisplacements::params_type AlgPToD_params;

  AlgDToP_params.skin_props = skin_provider.getAttributes();
  AlgPToD_params.skin_props = skin_provider.getAttributes();

  LOG(DEBUG1) << "Constructed all the required objects.";
  LOG(DEBUG1) << "Starting offline calculations.";

  std::unique_ptr<cm::Grid>& source_grid = (interpolator) ? interp_grid : natural_grid;

  if (interpolator) {
    LOG(DEBUG2) << "Starting offline interpolation phase.";
    interpolator->offline(*natural_grid, *interp_grid);
    LOG(DEBUG2) << "Done with offline interpolation phase.";
  }

  LOG(DEBUG2) << "Starting disps->pressures offline.";
  boost::any AlgDToP_offline = AlgDToP.offline(*source_grid, *pressures_grid, AlgDToP_params);
  LOG(DEBUG2) << "Done with disps->pressures offline.";
  LOG(DEBUG2) << "Starting pressures->disps offline.";
  boost::any AlgPToD_offline = AlgPToD.offline(*pressures_grid, *disps_grid,  AlgPToD_params);
  LOG(DEBUG2) << "Done with pressures->disps offline.";

  //const arma::mat& mFD = boost::any_cast<const arma::mat&>(AlgPToD_offline);
  //for (size_t i = 0; i < mFD.n_rows; ++i) {
  //  if (std::isnan(mFD(i,0))) {
  //    LOG(DEBUG3) << "x: " << disps_grid->cell(i).x << "y: " << disps_grid->cell(i).y;
  //  }
  //}

  LOG(DEBUG1) << "Done with offline calculations.";

  LOG(DEBUG2) << "Updating data in the grid."; 
  skin_provider.update(natural_grid->getRawValues());
  LOG(DEBUG2) << "Done updating data in the grid.";

  if (interpolator) {
    LOG(DEBUG2) << "Starting online interpolation phase.";
    interpolator->interpolate(*natural_grid, *interp_grid);
    LOG(DEBUG2) << "Done with online interpolation phase.";
  }
  LOG(DEBUG2) << "Running disps->pressures online.";
  AlgDToP.run(*source_grid, *pressures_grid, AlgDToP_params, AlgDToP_offline);
  LOG(DEBUG2) << "Done with disps->pressures online.";
  LOG(DEBUG2) << "Running pressures->disps online.";
  AlgPToD.run(*pressures_grid, *disps_grid,  AlgPToD_params, AlgPToD_offline);
  LOG(DEBUG2) << "Done with pressures->disps online.";

  double total_force = 0;
  for (size_t i = 0; i < pressures_grid->num_cells(); ++i) {
    total_force += pressures_grid->getValue(i, 0) * pressures_grid->getCellShape().area();
  }
  std::cout << "Total force: " << total_force << std::endl;

  dumpForPlot(*natural_grid, "natural");
  if (interpolator)
    dumpForPlot(*interp_grid, "interpolated");
  dumpForPlot(*pressures_grid, "pressures");
  dumpForPlot(*disps_grid,  "reconstructed");

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
      "Pitch of the grid the sensors readings should be interpolated into. If <= 0, no interpolation "
      "will be performed (a natural grid will be used. Pitch is the distance between two neighbouring "
      "cells in either x or y direction, in meters. Default: 0.001 [m].")
    ("pressures_pitch,p",
      po::value<double>(&options.pressures_pitch)->default_value(0.001),
      "Pitch of the pressures grid, i.e. distance between two neighbouring cells in either x or y "
      "direction, in meters. Default: 0.001 [m].")
    ("displacements_pitch,d",
      po::value<double>(&options.displacements_pitch)->default_value(0.001),
      "Pitch of the (resulting) displacements grid, i.e. distance between two neighbourint cells in "
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


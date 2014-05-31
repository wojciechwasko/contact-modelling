#include "reconstruction.hpp"
#include <exception>

#include "cm/cm.hpp"
#include "cm/details/string.hpp"

using cm::details::sb;

int main_impl(int argc, char** argv);

void offline(suite_type& suite);
void run(suite_type& suite);
void dump(suite_type& suite);

int main_impl(int argc, char** argv) {
  LOG_SET_LEVEL(DEBUG3);

  options_type options = process_options(argc, argv);
  suite_type suite = construct_suite(options);

  std::cout << "Constructed the suite, commencing offline calculations.\n";
  offline(suite);
  std::cout << "Done with offline calculations. Continuing with online.\n";
  run(suite);
  std::cout << "Done with online calculations. Dumping data.\n";
  dump(suite);
  std::cout << "Data dumped.\n";

  if (options.traction_type == TractionType::pressures) {
    double acc = 0;
    for (const auto& v : suite.tractions_grid->getRawValues()) {
      acc += v;
    }
    acc *= suite.tractions_grid->getCellShape().area();
    std::cout << "Force calculated from pressures: " << acc << std::endl;
  } else if (options.traction_type == TractionType::forces) {
    double acc = 0;
    for (const auto& v : suite.tractions_grid->getRawValues()) {
      acc += v;
    }
    std::cout << "Force: " << acc << std::endl;
  }

  return 0;
}

void offline(suite_type& suite)
{
  if (suite.interpolator && suite.interp_grid) {
    suite.interpolator->offline(*suite.raw_grid, *suite.interp_grid); 
    std::cout << "Offline interpolation -- done.\n";
    suite.to_tractions_precomputed = suite.to_tractions->offline(
      *suite.interp_grid, *suite.tractions_grid, suite.to_tractions_params
    );
    std::cout << "Offline to_tractions -- done.\n";
  } else {
    suite.to_tractions_precomputed = suite.to_tractions->offline(
      *suite.raw_grid, *suite.tractions_grid, suite.to_tractions_params
    );
    std::cout << "Offline to_tractions -- done.\n";
  }

  suite.to_reconstructed_precomputed = suite.to_reconstructed->offline(
    *suite.tractions_grid, *suite.reconstructed_grid, suite.to_reconstructed_params
  );
    std::cout << "Offline to_reconstructed -- done.\n";
}

void run(suite_type& suite)
{
  // update values in the source mesh
  suite.raw_grid->setRawValues(suite.skin_provider->update());

  if (suite.interpolator && suite.interp_grid) {
    suite.interpolator->interpolate(*suite.raw_grid, *suite.interp_grid);
    std::cout << "Online interpolation -- done.\n";

    suite.to_tractions->run(
      *suite.interp_grid,
      *suite.tractions_grid,
      suite.to_tractions_params,
      suite.to_tractions_precomputed
    );
  } else {
    suite.to_tractions->run(
      *suite.raw_grid,
      *suite.tractions_grid,
      suite.to_tractions_params,
      suite.to_tractions_precomputed
    );
  }

  suite.to_reconstructed->run(
    *suite.tractions_grid,
    *suite.reconstructed_grid,
    suite.to_reconstructed_params,
    suite.to_reconstructed_precomputed
  );
}

void dump(suite_type& suite)
{
  
  dumpForPlot(*suite.raw_grid, "raw.grid");
  if (suite.interpolator && suite.interp_grid)
    dumpForPlot(*suite.interp_grid, "interpolated.grid");
  dumpForPlot(*suite.tractions_grid, "tractions.grid");
  dumpForPlot(*suite.reconstructed_grid,  "reconstructed.grid");
}

int main(int argc, char** argv)
{
try {
  return main_impl(argc,argv);
} catch (std::exception& e) {
  std::cerr << e.what() << std::endl <<std::flush;
  std::exit(1);
} catch (...) {
  std::cerr << "Unknown exception happened! Aborting!" << std::endl << std::flush;
  std::exit(1);
}
}

#include <boost/test/unit_test.hpp>
#include "custom_test_macros.hpp"

#include <cstddef>
#include <array>
#include <vector>

#include "cm/algorithm/forces_to_displacements.hpp"

#include "cm/details/external/armadillo.hpp"
#include "cm/grid/grid.hpp"
#include "cm/grid/cell_shapes.hpp"

struct MockCell {
  MockCell(double x, double y) : relative_position{x,y} {};
  std::array<double, 2> relative_position;
};

struct MockGrid {
  std::vector<MockCell> cells;

  std::unique_ptr<cm::Grid> create(size_t dim, const cm::GridCellShape& cell_shape)
  {
    return std::unique_ptr<cm::Grid>(cm::Grid::fromSensors(dim, cell_shape, cells.cbegin(), cells.cend()));
  }
};

// generated with gen/gen.py
std::vector<double> expected_diff_3_3 = {4.77612499182e-06,3.32211154015e-06,-1.15517619829e-05, 3.32211154015e-06,7.05414433363e-06,-1.61724667761e-05, -1.15517619829e-05,-1.61724667761e-05,-2.21751661574e-06};
std::vector<double> expected_diff_1_3 = {-1.15517619829e-05,-1.61724667761e-05,-2.21751661574e-06};
std::vector<double> expected_diff_3_1 = {-1.15517619829e-05,-1.61724667761e-05,-2.21751661574e-06};
std::vector<double> expected_diff_1_1 = {-2.21751661574e-06};
std::vector<double> expected_same_3_3 = {1.93137101012e-05,0,0, 0,1.93137101012e-05,0, 0,0,3.86274202023e-05};
std::vector<double> expected_same_1_3 = {0,0,3.86274202023e-05};
std::vector<double> expected_same_3_1 = {0,0,3.86274202023e-05};
std::vector<double> expected_same_1_1 = {3.86274202023e-05};

// -- TEST CASES: grid cells "far away", i.e. dx > eps_samepoint && dy > eps_samepoint
// !begin

BOOST_AUTO_TEST_SUITE(elastic_models__forces)
BOOST_AUTO_TEST_CASE(em_other_cells_1_1_dim_3_3)
{
  MockGrid forces_source;
  MockGrid disps_source;

  forces_source.cells.push_back({0.1,0.1});
  disps_source.cells.push_back({0.105,0.107});

  auto forces_grid = forces_source.create(3, cm::Square(0.044721));
  auto disps_grid = disps_source.create(3, cm::Square(0.044721));

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  params_forces_to_disps.skin_props.nu = 0.5;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(*forces_grid, *disps_grid, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_diff_3_3, 1e-3);
}

BOOST_AUTO_TEST_CASE(em_other_cells_1_1_dim_1_3)
{
  MockGrid forces_source;
  MockGrid disps_source;

  forces_source.cells.push_back({0.1,0.1});
  disps_source.cells.push_back({0.105,0.107});

  auto forces_grid = forces_source.create(1, cm::Square(0.044721));
  auto disps_grid = disps_source.create(3, cm::Square(0.044721));

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  params_forces_to_disps.skin_props.nu = 0.5;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(*forces_grid, *disps_grid, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_diff_1_3, 1e-3);
}

BOOST_AUTO_TEST_CASE(em_other_cells_1_1_dim_3_1)
{
  MockGrid forces_source;
  MockGrid disps_source;

  forces_source.cells.push_back({0.1,0.1});
  disps_source.cells.push_back({0.105,0.107});

  auto forces_grid = forces_source.create(3, cm::Square(0.044721));
  auto disps_grid = disps_source.create(1, cm::Square(0.044721));

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  params_forces_to_disps.skin_props.nu = 0.5;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(*forces_grid, *disps_grid, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_diff_3_1, 1e-3);
}

BOOST_AUTO_TEST_CASE(em_other_cells_1_1_dim_1_1)
{
  MockGrid forces_source;
  MockGrid disps_source;

  // note: for generator dx = 0.0001, dy = 0.0004 (it'd disps.x/y - forces.x/y)
  forces_source.cells.push_back({0.1,0.1});
  disps_source.cells.push_back({0.105,0.107});

  auto forces_grid = forces_source.create(1, cm::Square(0.044721));
  auto disps_grid = disps_source.create(1, cm::Square(0.044721));

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  params_forces_to_disps.skin_props.nu = 0.5;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(*forces_grid, *disps_grid, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_diff_1_1, 1e-3);
}
// !end


// -- TEST CASES: grid cells "very close", i.e. dx < eps_samepoint && dy < eps_samepoint
// !begin

BOOST_AUTO_TEST_CASE(em_same_cells_1_1_dim_3_3)
{
  MockGrid forces_source;
  MockGrid disps_source;

  // note: for generator dx = 1e-5, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_source.cells.push_back({0.1,0.1});
  disps_source.cells.push_back({0.1, 0.1});

  auto forces_grid = forces_source.create(3, cm::Square(0.044721));
  auto disps_grid = disps_source.create(3, cm::Square(0.044721));

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  params_forces_to_disps.skin_props.nu = 0.5;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(*forces_grid, *disps_grid, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_same_3_3, 1e-3);
}

BOOST_AUTO_TEST_CASE(em_same_cells_1_1_dim_1_3)
{
  MockGrid forces_source;
  MockGrid disps_source;

  // note: for generator dx = 1e-5, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_source.cells.push_back({0.1,0.1});
  disps_source.cells.push_back({0.1, 0.1});

  auto forces_grid = forces_source.create(1, cm::Square(0.044721));
  auto disps_grid = disps_source.create(3, cm::Square(0.044721));

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  params_forces_to_disps.skin_props.nu = 0.5;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(*forces_grid, *disps_grid, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_same_1_3, 1e-3);
}

BOOST_AUTO_TEST_CASE(em_same_cells_1_1_dim_3_1)
{
  MockGrid forces_source;
  MockGrid disps_source;

  // note: for generator dx = 1e-5, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_source.cells.push_back({0.1,0.1});
  disps_source.cells.push_back({0.1, 0.1});

  auto forces_grid = forces_source.create(3, cm::Square(0.044721));
  auto disps_grid = disps_source.create(1, cm::Square(0.044721));

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  params_forces_to_disps.skin_props.nu = 0.5;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(*forces_grid, *disps_grid, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_same_3_1, 1e-3);
}

BOOST_AUTO_TEST_CASE(em_same_cells_1_1_dim_1_1)
{
  MockGrid forces_source;
  MockGrid disps_source;

  // note: for generator dx = 1e-5, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_source.cells.push_back({0.1,0.1});
  disps_source.cells.push_back({0.1, 0.1});

  auto forces_grid = forces_source.create(1, cm::Square(0.044721));
  auto disps_grid = disps_source.create(1, cm::Square(0.044721));

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  params_forces_to_disps.skin_props.nu = 0.5;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(*forces_grid, *disps_grid, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_same_1_1, 1e-3);
}

BOOST_AUTO_TEST_SUITE_END()
// !end

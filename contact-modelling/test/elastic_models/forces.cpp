#include <boost/test/unit_test.hpp>
#include "custom_test_macros.hpp"

#include <cstddef>
#include <array>
#include <vector>

#include "cm/algorithm/forces_to_displacements.hpp"
#include "cm/algorithm/displacements_to_forces.hpp"

#include "cm/details/external/armadillo.hpp"
#include "cm/grid/grid.hpp"
#include "cm/grid/cell_shapes.hpp"

#include "forces_fixture.hpp"

// make it have internal linkage
static const double eps = 1e-5;

BOOST_FIXTURE_TEST_SUITE(elastic_models__forces, BoussinesqFixture)

BOOST_AUTO_TEST_CASE(test_alg_force1_disp1)
{
  std::vector<double> local_copy(disps11_grid->getRawValues());
  cm::AlgForcesToDisplacements alg;
  cm::AlgForcesToDisplacements::params_type   params;
  params.skin_props = skin_attr;

  boost::any pre = alg.offline(*force11_grid, *disps11_grid, params);
  alg.run(*force11_grid, *disps11_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, disps11_grid->getRawValues(), eps);
};

BOOST_AUTO_TEST_CASE(test_alg_force1_disp3)
{
  std::vector<double> local_copy(disps13_grid->getRawValues());
  cm::AlgForcesToDisplacements alg;
  cm::AlgForcesToDisplacements::params_type   params;
  params.skin_props = skin_attr;

  boost::any pre = alg.offline(*force13_grid, *disps13_grid, params);
  alg.run(*force13_grid, *disps13_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, disps13_grid->getRawValues(), eps);
};

BOOST_AUTO_TEST_CASE(test_alg_force3_disp1)
{
  std::vector<double> local_copy(disps31_grid->getRawValues());
  cm::AlgForcesToDisplacements alg;
  cm::AlgForcesToDisplacements::params_type   params;
  params.skin_props = skin_attr;

  boost::any pre = alg.offline(*force31_grid, *disps31_grid, params);
  alg.run(*force31_grid, *disps31_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, disps31_grid->getRawValues(), eps);
};

BOOST_AUTO_TEST_CASE(test_alg_force3_disp3)
{
  std::vector<double> local_copy(disps33_grid->getRawValues());
  cm::AlgForcesToDisplacements alg;
  cm::AlgForcesToDisplacements::params_type   params;
  params.skin_props = skin_attr;

  boost::any pre = alg.offline(*force33_grid, *disps33_grid, params);
  alg.run(*force33_grid, *disps31_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, disps33_grid->getRawValues(), eps);
};

BOOST_AUTO_TEST_CASE(test_alg_disps1_disp1)
{
  std::vector<double> local_copy(force11_grid->getRawValues());
  cm::AlgDisplacementsToForces alg;
  cm::AlgDisplacementsToForces::params_type   params;
  params.skin_props = skin_attr;

  boost::any pre = alg.offline(*disps11_grid, *force11_grid, params);
  alg.run(*disps11_grid, *force11_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, force11_grid->getRawValues(), eps);
};

BOOST_AUTO_TEST_CASE(test_alg_disps1_disp3)
{
  std::vector<double> local_copy(force13_grid->getRawValues());
  cm::AlgDisplacementsToForces alg;
  cm::AlgDisplacementsToForces::params_type   params;
  params.skin_props = skin_attr;

  boost::any pre = alg.offline(*disps13_grid, *force13_grid, params);
  alg.run(*disps13_grid, *force13_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, force13_grid->getRawValues(), eps);
};

BOOST_AUTO_TEST_CASE(test_alg_disps3_disp1)
{
  std::vector<double> local_copy(force31_grid->getRawValues());
  cm::AlgDisplacementsToForces alg;
  cm::AlgDisplacementsToForces::params_type   params;
  params.skin_props = skin_attr;

  boost::any pre = alg.offline(*disps31_grid, *force31_grid, params);
  alg.run(*disps31_grid, *force31_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, force31_grid->getRawValues(), eps);
};

BOOST_AUTO_TEST_CASE(test_alg_disps3_disp3)
{
  std::vector<double> local_copy(force33_grid->getRawValues());
  cm::AlgDisplacementsToForces alg;
  cm::AlgDisplacementsToForces::params_type   params;
  params.skin_props = skin_attr;

  boost::any pre = alg.offline(*disps33_grid, *force33_grid, params);
  alg.run(*disps33_grid, *force31_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, force33_grid->getRawValues(), eps);
};

BOOST_AUTO_TEST_SUITE_END()

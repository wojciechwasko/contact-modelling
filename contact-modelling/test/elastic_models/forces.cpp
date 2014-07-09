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
#include "cm/details/elastic_model_boussinesq.hpp"

#include "forces_fixture.hpp"

// NOTE: when checking matrices or forces/displacements
// those things have some magniture -- forces have a magnitude of around 1e0 --
// 1e2, displacement: 1e-3 -- 1e-8 and matrix elements the ratios of the two.
// Therefore, it doesn't make much sense to be using the same espilon for both.
// As side note, one should consider moving the displacements to e.g.
// micrometers to make them in a more reasonable range.
static const double eps_small_nums  = 1e-1; // 0.1%
static const double eps_normal_nums = 1e-5; // 0.00001%

// For the matrices (in particular), in the pseudoinverses, there are elements
// which should (theoretically) be zero, but due to limited precision of
// calculations, they're equal to roughly 1e-12, 1e-14. All other elements are
// fairly large (~1e+2) and the check fail because the "small" values returned
// by Matlab are quite different from those returned by armadillo -- up to 800%
// IIRC. It's still 1e-12 vs 1e-14 but yeah we have to be able to ignore them.
// This is why CHECK_CLOSE_COLLECTION_IGNORE_SMALL was written.
//
// God, I love floating point.
//
// Not.
static const double small_threshold = 1e-5;

BOOST_FIXTURE_TEST_SUITE(elastic_models__forces, BoussinesqFixture)

BOOST_AUTO_TEST_CASE(test_alg_force1_disp1)
{
  std::vector<double> local_copy(disps11_grid->getRawValues());
  cm::AlgForcesToDisplacements alg;
  cm::AlgForcesToDisplacements::params_type   params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  boost::any pre = alg.offline(*force11_grid, *disps11_grid, params);
  alg.run(*force11_grid, *disps11_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, disps11_grid->getRawValues(), eps_small_nums);
};

BOOST_AUTO_TEST_CASE(test_alg_force1_disp3)
{
  std::vector<double> local_copy(disps13_grid->getRawValues());
  cm::AlgForcesToDisplacements alg;
  cm::AlgForcesToDisplacements::params_type   params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  boost::any pre = alg.offline(*force13_grid, *disps13_grid, params);
  alg.run(*force13_grid, *disps13_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, disps13_grid->getRawValues(), eps_small_nums);
};

BOOST_AUTO_TEST_CASE(test_alg_force3_disp1)
{
  std::vector<double> local_copy(disps31_grid->getRawValues());
  cm::AlgForcesToDisplacements alg;
  cm::AlgForcesToDisplacements::params_type   params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  boost::any pre = alg.offline(*force31_grid, *disps31_grid, params);
  alg.run(*force31_grid, *disps31_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, disps31_grid->getRawValues(), eps_small_nums);
};

BOOST_AUTO_TEST_CASE(test_alg_force3_disp3)
{
  std::vector<double> local_copy(disps33_grid->getRawValues());
  cm::AlgForcesToDisplacements alg;
  cm::AlgForcesToDisplacements::params_type   params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  boost::any pre = alg.offline(*force33_grid, *disps33_grid, params);
  alg.run(*force33_grid, *disps33_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, disps33_grid->getRawValues(), eps_small_nums);
};

BOOST_AUTO_TEST_CASE(test_alg_disps1_force1)
{
  std::vector<double> local_copy(force11_grid->getRawValues());
  cm::AlgDisplacementsToForces alg;
  cm::AlgDisplacementsToForces::params_type   params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  boost::any pre = alg.offline(*disps11_grid, *force11_grid, params);
  alg.run(*disps11_grid, *force11_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, force11_grid->getRawValues(), eps_normal_nums);
};

BOOST_AUTO_TEST_CASE(test_alg_disps3_force1)
{
  std::vector<double> local_copy(force13_grid->getRawValues());
  cm::AlgDisplacementsToForces alg;
  cm::AlgDisplacementsToForces::params_type   params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  boost::any pre = alg.offline(*disps13_grid, *force13_grid, params);
  alg.run(*disps13_grid, *force13_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, force13_grid->getRawValues(), eps_normal_nums);
};

BOOST_AUTO_TEST_CASE(test_alg_disps3_force3)
{
  std::vector<double> local_copy(force33_grid->getRawValues());
  cm::AlgDisplacementsToForces alg;
  cm::AlgDisplacementsToForces::params_type   params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  boost::any pre = alg.offline(*disps33_grid, *force33_grid, params);
  alg.run(*disps33_grid, *force33_grid, params, pre);

  CHECK_CLOSE_COLLECTION(local_copy, force33_grid->getRawValues(), eps_normal_nums);
};

BOOST_AUTO_TEST_CASE(test_matrix_force1_disps1)
{
  cm::AlgForcesToDisplacements alg;
  cm::AlgForcesToDisplacements::params_type params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  arma::mat calc_mat = cm::details::forces_to_displacements_matrix(
      *force11_grid,
      *disps11_grid,
      skin_attr,
      true);

  BOOST_REQUIRE_EQUAL(force1_disps1.n_rows, calc_mat.n_rows);
  BOOST_REQUIRE_EQUAL(force1_disps1.n_cols, calc_mat.n_cols);
  CHECK_CLOSE_COLLECTION(force1_disps1, calc_mat, eps_small_nums);
};

BOOST_AUTO_TEST_CASE(test_matrix_force3_disps1)
{
  cm::AlgForcesToDisplacements alg;
  cm::AlgForcesToDisplacements::params_type params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  arma::mat calc_mat = cm::details::forces_to_displacements_matrix(
      *force31_grid,
      *disps31_grid,
      skin_attr,
      true);

  BOOST_REQUIRE_EQUAL(force3_disps1.n_rows, calc_mat.n_rows);
  BOOST_REQUIRE_EQUAL(force3_disps1.n_cols, calc_mat.n_cols);
  CHECK_CLOSE_COLLECTION(force3_disps1, calc_mat, eps_small_nums);
};

BOOST_AUTO_TEST_CASE(test_matrix_force1_disps3)
{
  cm::AlgForcesToDisplacements alg;
  cm::AlgForcesToDisplacements::params_type params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  arma::mat calc_mat = cm::details::forces_to_displacements_matrix(
      *force13_grid,
      *disps13_grid,
      skin_attr,
      true);

  BOOST_REQUIRE_EQUAL(force1_disps3.n_rows, calc_mat.n_rows);
  BOOST_REQUIRE_EQUAL(force1_disps3.n_cols, calc_mat.n_cols);
  CHECK_CLOSE_COLLECTION(force1_disps3, calc_mat, eps_small_nums);
};


BOOST_AUTO_TEST_CASE(test_matrix_force3_disps3)
{
  cm::AlgForcesToDisplacements alg;
  cm::AlgForcesToDisplacements::params_type params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  arma::mat calc_mat = cm::details::forces_to_displacements_matrix(
      *force33_grid,
      *disps33_grid,
      skin_attr,
      true);

  BOOST_REQUIRE_EQUAL(force3_disps3.n_rows, calc_mat.n_rows);
  BOOST_REQUIRE_EQUAL(force3_disps3.n_cols, calc_mat.n_cols);
  CHECK_CLOSE_COLLECTION(force3_disps3, calc_mat, eps_small_nums);
};


BOOST_AUTO_TEST_CASE(test_matrix_disps1_force1)
{
  cm::AlgDisplacementsToForces alg;
  cm::AlgDisplacementsToForces::params_type params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  arma::mat calc_mat = cm::details::displacements_to_forces_matrix(
      *disps11_grid,
      *force11_grid,
      skin_attr,
      true);

  BOOST_REQUIRE_EQUAL(disps1_force1.n_rows, calc_mat.n_rows);
  BOOST_REQUIRE_EQUAL(disps1_force1.n_cols, calc_mat.n_cols);
  CHECK_CLOSE_COLLECTION(disps1_force1, calc_mat, eps_normal_nums);
};
 
// 2 errors
BOOST_AUTO_TEST_CASE(test_matrix_disps3_force1)
{
  cm::AlgDisplacementsToForces alg;
  cm::AlgDisplacementsToForces::params_type params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  arma::mat calc_mat = cm::details::displacements_to_forces_matrix(
      *disps13_grid,
      *force13_grid,
      skin_attr,
      true);

  BOOST_REQUIRE_EQUAL(disps3_force1.n_rows, calc_mat.n_rows);
  BOOST_REQUIRE_EQUAL(disps3_force1.n_cols, calc_mat.n_cols);
  CHECK_CLOSE_COLLECTION_IGNORE_SMALL(disps3_force1, calc_mat, eps_normal_nums, small_threshold);
};

// 2 errors
BOOST_AUTO_TEST_CASE(test_matrix_disps1_force3)
{
  cm::AlgDisplacementsToForces alg;
  cm::AlgDisplacementsToForces::params_type params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  arma::mat calc_mat = cm::details::displacements_to_forces_matrix(
      *disps31_grid,
      *force31_grid,
      skin_attr,
      true);

  BOOST_REQUIRE_EQUAL(disps1_force3.n_rows, calc_mat.n_rows);
  BOOST_REQUIRE_EQUAL(disps1_force3.n_cols, calc_mat.n_cols);
  CHECK_CLOSE_COLLECTION_IGNORE_SMALL(disps1_force3, calc_mat, eps_normal_nums, small_threshold);
};

// 4 errors, some weird numbers popping up
BOOST_AUTO_TEST_CASE(test_matrix_disps3_force3)
{
  cm::AlgDisplacementsToForces alg;
  cm::AlgDisplacementsToForces::params_type params;
  params.skin_props = skin_attr;
  params.psi_exact = true;

  arma::mat calc_mat = cm::details::displacements_to_forces_matrix(
      *disps33_grid,
      *force33_grid,
      skin_attr,
      true);

  BOOST_REQUIRE_EQUAL(disps3_force3.n_rows, calc_mat.n_rows);
  BOOST_REQUIRE_EQUAL(disps3_force3.n_cols, calc_mat.n_cols);
  CHECK_CLOSE_COLLECTION_IGNORE_SMALL(disps3_force3, calc_mat, eps_normal_nums, small_threshold);
};

BOOST_AUTO_TEST_CASE(test_pinv_disps3_force3)
{
  arma::mat calc_mat = arma::pinv(force3_disps3);

  BOOST_REQUIRE_EQUAL(disps3_force3.n_rows, calc_mat.n_rows);
  BOOST_REQUIRE_EQUAL(disps3_force3.n_cols, calc_mat.n_cols);
  CHECK_CLOSE_COLLECTION_IGNORE_SMALL(disps3_force3, calc_mat, eps_normal_nums, small_threshold);
};

BOOST_AUTO_TEST_CASE(bouss_xx)
{
  using cm::details::impl::CoeffsBouss;
  using cm::details::impl::bouss_xx;
  CoeffsBouss cb(skin_attr.E, x_for_bouss, y_for_bouss, skin_attr.h, s_for_appro);
  BOOST_CHECK_CLOSE(bouss_xx_v, bouss_xx(skin_attr, cb, x_for_bouss, y_for_bouss), eps_small_nums);
}

BOOST_AUTO_TEST_CASE(bouss_xy)
{
  using cm::details::impl::CoeffsBouss;
  using cm::details::impl::bouss_xy;
  CoeffsBouss cb(skin_attr.E, x_for_bouss, y_for_bouss, skin_attr.h, s_for_appro);
  BOOST_CHECK_CLOSE(bouss_xy_v, bouss_xy(skin_attr, cb, x_for_bouss, y_for_bouss), eps_small_nums);
}

BOOST_AUTO_TEST_CASE(bouss_xz)
{
  using cm::details::impl::CoeffsBouss;
  using cm::details::impl::bouss_xz;
  CoeffsBouss cb(skin_attr.E, x_for_bouss, y_for_bouss, skin_attr.h, s_for_appro);
  BOOST_CHECK_CLOSE(bouss_xz_v, bouss_xz(skin_attr, cb, x_for_bouss, y_for_bouss), eps_small_nums);
}

BOOST_AUTO_TEST_CASE(bouss_yx)
{
  using cm::details::impl::CoeffsBouss;
  using cm::details::impl::bouss_yx;
  CoeffsBouss cb(skin_attr.E, x_for_bouss, y_for_bouss, skin_attr.h, s_for_appro);
  BOOST_CHECK_CLOSE(bouss_yx_v, bouss_yx(skin_attr, cb, x_for_bouss, y_for_bouss), eps_small_nums);
}

BOOST_AUTO_TEST_CASE(bouss_yy)
{
  using cm::details::impl::CoeffsBouss;
  using cm::details::impl::bouss_yy;
  CoeffsBouss cb(skin_attr.E, x_for_bouss, y_for_bouss, skin_attr.h, s_for_appro);
  BOOST_CHECK_CLOSE(bouss_yy_v, bouss_yy(skin_attr, cb, x_for_bouss, y_for_bouss), eps_small_nums);
}

BOOST_AUTO_TEST_CASE(bouss_yz)
{
  using cm::details::impl::CoeffsBouss;
  using cm::details::impl::bouss_yz;
  CoeffsBouss cb(skin_attr.E, x_for_bouss, y_for_bouss, skin_attr.h, s_for_appro);
  BOOST_CHECK_CLOSE(bouss_yz_v, bouss_yz(skin_attr, cb, x_for_bouss, y_for_bouss), eps_small_nums);
}

BOOST_AUTO_TEST_CASE(bouss_zx)
{
  using cm::details::impl::CoeffsBouss;
  using cm::details::impl::bouss_zx;
  CoeffsBouss cb(skin_attr.E, x_for_bouss, y_for_bouss, skin_attr.h, s_for_appro);
  BOOST_CHECK_CLOSE(bouss_zx_v, bouss_zx(skin_attr, cb, x_for_bouss, y_for_bouss), eps_small_nums);
}

BOOST_AUTO_TEST_CASE(bouss_zy)
{
  using cm::details::impl::CoeffsBouss;
  using cm::details::impl::bouss_zy;
  CoeffsBouss cb(skin_attr.E, x_for_bouss, y_for_bouss, skin_attr.h, s_for_appro);
  BOOST_CHECK_CLOSE(bouss_zy_v, bouss_zy(skin_attr, cb, x_for_bouss, y_for_bouss), eps_small_nums);
}

BOOST_AUTO_TEST_CASE(bouss_zz)
{
  using cm::details::impl::CoeffsBouss;
  using cm::details::impl::bouss_zz;
  CoeffsBouss cb(skin_attr.E, x_for_bouss, y_for_bouss, skin_attr.h, s_for_appro);
  BOOST_CHECK_CLOSE(bouss_zz_v, bouss_zz(skin_attr, cb, x_for_bouss, y_for_bouss), eps_small_nums);
}

BOOST_AUTO_TEST_CASE(appro_xx)
{
  using cm::details::impl::CoeffsBouss;
  using cm::details::impl::appro_xx;
  CoeffsBouss cb(skin_attr.E, x_for_bouss, y_for_bouss, skin_attr.h, s_for_appro);
  BOOST_CHECK_CLOSE(appro_xx_v, appro_xx(skin_attr, cb), eps_small_nums);
}

BOOST_AUTO_TEST_CASE(appro_yy)
{
  using cm::details::impl::CoeffsBouss;
  using cm::details::impl::appro_yy;
  CoeffsBouss cb(skin_attr.E, x_for_bouss, y_for_bouss, skin_attr.h, s_for_appro);
  BOOST_CHECK_CLOSE(appro_yy_v, appro_yy(skin_attr, cb), eps_small_nums);
}

BOOST_AUTO_TEST_CASE(appro_zz)
{
  using cm::details::impl::CoeffsBouss;
  using cm::details::impl::appro_zz;
  CoeffsBouss cb(skin_attr.E, x_for_bouss, y_for_bouss, skin_attr.h, s_for_appro);
  BOOST_CHECK_CLOSE(appro_zz_v, appro_zz(skin_attr, cb), eps_small_nums);
}

BOOST_AUTO_TEST_CASE(z0h)
{
  using cm::details::impl::z0h;
  BOOST_CHECK_CLOSE(z0h_from_val, z0h(val_for_z0h), eps_small_nums);
}

BOOST_AUTO_TEST_CASE(psi)
{
  using cm::details::impl::psi;
  BOOST_CHECK_CLOSE(psi_from_val, psi(val_for_psi), eps_small_nums);
}


BOOST_AUTO_TEST_SUITE_END()

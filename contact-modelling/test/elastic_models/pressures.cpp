#include <boost/test/unit_test.hpp>
#include <boost/any.hpp>
#include "custom_test_macros.hpp"

#include <cstddef>
#include <array>
#include <vector>
#include <stdexcept>
#include <memory>

#include "cm/algorithm/pressures_to_displacements.hpp"
#include "cm/algorithm/displacements_to_pressures.hpp"
#include "cm/details/elastic_model_love.hpp"
#include "cm/details/string.hpp"

#include "cm/details/external/armadillo.hpp"
#include "cm/grid/grid.hpp"
#include "cm/skin/attributes.hpp"
#include "cm/grid/cell_shapes.hpp"

#include "pressures_fixture.hpp"

using cm::details::sb;

BOOST_FIXTURE_TEST_SUITE(elastic_models__pressures, LoveFixture)

BOOST_AUTO_TEST_CASE(two_pressure_elements)
{
  arma::mat calc_press2disps = cm::details::pressures_to_displacements_matrix(
    *press_grid,
    *disps_grid,
    skin_attr
  );

  CHECK_CLOSE_COLLECTION(calc_press2disps, press2disps, 1e-5);

  arma::mat calc_disps2press = cm::details::displacements_to_pressures_matrix(
    *disps_grid,
    *press_grid,
    skin_attr
  );

  CHECK_CLOSE_COLLECTION(calc_disps2press, disps2press, 1e-5);
}

BOOST_AUTO_TEST_CASE(alg_pressures_to_disps)
{
  typedef cm::AlgPressuresToDisplacements A_p_d;
  A_p_d::params_type params_p_d;
  params_p_d.skin_props = skin_attr;
  boost::any pre_p_d = A_p_d().offline(*press_grid, *disps_grid, params_p_d);
  A_p_d().run(*press_grid, *disps_grid, params_p_d, pre_p_d);

  CHECK_CLOSE_COLLECTION(disps_grid->getRawValues(), expected_disps, 1e-5);
}

BOOST_AUTO_TEST_CASE(alg_disps_to_pressures)
{
  typedef cm::AlgDisplacementsToPressures A_d_p;
  A_d_p::params_type params_d_p;
  params_d_p.skin_props = skin_attr;
  boost::any pre_d_p = A_d_p().offline(*disps_grid, *press_grid, params_d_p);
  A_d_p().run(*disps_grid, *press_grid, params_d_p, pre_d_p);

  CHECK_CLOSE_COLLECTION(press_grid->getRawValues(), expected_press, 1e-5);
}

/* HARD-CODED pre-computated values follow */

BOOST_AUTO_TEST_CASE(love_r10)
{
  using cm::details::impl::love::rj0;
  const double r10 = 0.18547; // calculated by MATLAB(R) script
  const double cal = rj0<1>(0.1, 0.2, -1, 0.3, 0, 0.12);
  BOOST_CHECK_CLOSE(r10,cal,1e-2);
}

BOOST_AUTO_TEST_CASE(love_r20)
{
  using cm::details::impl::love::rj0;
  const double r20 = 0.52383; // calculated by MATLAB(R) script
  const double cal = rj0<2>(0.1, 0.2, -1, 0.3, 0, 0.12);
  BOOST_CHECK_CLOSE(r20,cal,1e-2);
}

BOOST_AUTO_TEST_CASE(love_beta10)
{
  using cm::details::impl::love::betaj0;
  const double beta10 = 0.1562; // calculated by MATLAB(R) script
  const double cal = betaj0<1>(0.1, 0.2, -1, 0.3, 0, 0.12);
  BOOST_CHECK_CLOSE(beta10,cal,1e-2);
}

BOOST_AUTO_TEST_CASE(love_beta20)
{
  using cm::details::impl::love::betaj0;
  const double beta20 = 0.5142; // calculated by MATLAB(R) script
  const double cal = betaj0<2>(0.1, 0.2, -1, 0.3, 0, 0.12);
  BOOST_CHECK_CLOSE(beta20,cal,1e-2);
}

BOOST_AUTO_TEST_CASE(love_psi10)
{
  using cm::details::impl::love::psij0;
  double psi10 = 0.29267; // calculated by MATLAB(R) script
  const double cal = psij0<1>(0.1, 0.2, -1, 0.3, 0, 0.12);
  BOOST_CHECK_CLOSE(psi10,cal,1e-2);
}

BOOST_AUTO_TEST_CASE(love_psi20)
{
  using cm::details::impl::love::psij0;
  const double psi20 = 0.096336; // calculated by MATLAB(R) script
  const double cal = psij0<2>(0.1, 0.2, -1, 0.3, 0, 0.12);
  BOOST_CHECK_CLOSE(psi20,cal,1e-2);
}

BOOST_AUTO_TEST_CASE(love_L1)
{
  using cm::details::impl::love::Lj;
  const double L1 = -0.27221;
  const double cal = Lj<1>(0.1, 0.2, -1, 0.3, 0, 0.12);
  BOOST_CHECK_CLOSE(L1,cal,1e-2);
}

BOOST_AUTO_TEST_CASE(love_L1_z_0)
{
  using cm::details::impl::love::Lj;
  const double L1 = -0.50653;
  const double cal = Lj<1>(0.1, 0.2, -1, 0.3, 0, 0);
  BOOST_CHECK_CLOSE(L1,cal,1e-2);
}

BOOST_AUTO_TEST_CASE(love_L1_x_a)
{
  using cm::details::impl::love::Lj;
  const double L1 = -0.31505;
  const double cal = Lj<1>(0.1, 0.2, -1, 0.2, 0, 0.01);
  BOOST_CHECK_CLOSE(L1,cal,1e-2);
}

BOOST_AUTO_TEST_CASE(love_L2)
{
  using cm::details::impl::love::Lj;
  const double L2 = -0.40631;
  const double cal = Lj<2>(0.1, 0.2, -1, 0.3, 0, 0.12);
  BOOST_CHECK_CLOSE(L2,cal,1e-2);
}

BOOST_AUTO_TEST_CASE(love_L2_z_0)
{
  using cm::details::impl::love::Lj;
  const double L2 = -0.66085;
  const double cal = Lj<2>(0.1, 0.2, -1, 0.3, 0, 0);
  BOOST_CHECK_CLOSE(L2,cal,1e-2);
}

BOOST_AUTO_TEST_CASE(love_L2_x_a)
{
  using cm::details::impl::love::Lj;
  const double L2 = -0.60831;
  const double cal = Lj<2>(0.1, 0.2, -1, 0.2, 0, 0.01);
  BOOST_CHECK_CLOSE(L2,cal,1e-2);
}

BOOST_AUTO_TEST_CASE(love_coeff_z0)
{
  using cm::details::impl::love_coeff;
  const double l_coeff = 0.00043407;
  const double cal = love_coeff(0.001, 0.001, 210000, 0.49, 0.008, 0.007, 0);
}

BOOST_AUTO_TEST_CASE(love_coeff_zn0)
{
  using cm::details::impl::love_coeff;
  const double l_coeff = 0.00035869;
  const double cal = love_coeff(0.001, 0.001, 210000, 0.49, 0.008, 0.007, 0.02);
}

BOOST_AUTO_TEST_SUITE_END()

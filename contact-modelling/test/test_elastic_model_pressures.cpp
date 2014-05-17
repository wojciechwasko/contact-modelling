#define BOOST_TEST_MODULE elastic_model_pressures
#include <boost/test/unit_test.hpp>
#include <boost/any.hpp>
#include "inc/custom_test_macros.hpp"

#include <cstddef>
#include <array>
#include <vector>
#include <stdexcept>

#include "cm/algorithm/pressures_to_displacements.hpp"
#include "cm/algorithm/displacements_to_pressures.hpp"
#include "cm/details/elastic_model_love.hpp"
#include "cm/details/string.hpp"

#include "cm/details/external/armadillo.hpp"
#include "cm/mesh/rectangular_base.hpp"
#include "cm/mesh/interface.hpp"
#include "cm/skin/attributes.hpp"

using cm::details::sb;

struct MockNode {
  double x;
  double y;
};

class MockMesh : public cm::MeshInterface {
private:
  size_t offset;

public:
  MockMesh(size_t dim, size_t no_nodes) : cm::MeshInterface(dim, no_nodes), offset(0) {}
  void add_node(MockNode n)
  {
    node(offset).x = n.x;
    node(offset).y = n.y;
    for (size_t i = 0; i < D; ++i)
      setValue(offset, i, 0);
    ++offset;
  }

private:
  double impl_node_area(size_t i) const
  {
    return 0.002;
  }
};

cm::SkinAttributes generate_skin_attr()
{
  cm::SkinAttributes skin_attr;
  skin_attr.E = 210000;
  skin_attr.nu = 0.49;
  skin_attr.h = 0.002;
  return skin_attr;
}

MockMesh generate_disps_mesh()
{
  MockMesh disps_mesh(1, 3);
  disps_mesh.add_node({-0.015, -0.007});
  disps_mesh.add_node({0, 0});
  disps_mesh.add_node({0.01, 0.005});
  return disps_mesh;
}

cm::MeshRectangularBase generate_pressures_mesh() 
{
  // I don't like this. This requires cm::MeshRectangularBase to work properly
  // But without hacking the code too much, I can't figure a better way.
  // Will (probably) check if the nodes are created the way I want them to.
  // The nodes should be:
  // (-0.01, -0.005), (-0.01, 0.005), (0.01, -0.005), (0.01, 0.005) (in that order)
  cm::MeshRectangularBase pressure_mesh(1, -0.01, -0.005, 0.01+1e-7, 0.005+1e-7, 0.02, 0.01);
  if (pressure_mesh.no_nodes() != 4) {
    throw std::runtime_error((sb() << "Wrong number of nodes in pressure_mesh: " << pressure_mesh.no_nodes()));
  }

  if (pressure_mesh.node(0).x != -0.01 || pressure_mesh.node(0).y != -0.005) {
    throw std::runtime_error(sb() 
      << "Mismatch with first node: "
      << "(" << pressure_mesh.node(0).x << "," << pressure_mesh.node(0).y << ")"
    );
  }

  if (pressure_mesh.node(1).x != -0.01 || pressure_mesh.node(1).y != 0.005) {
    throw std::runtime_error(sb() 
      << "Mismatch with second node: "
      << "(" << pressure_mesh.node(1).x << "," << pressure_mesh.node(1).y << ")"
    );
  }

  if (pressure_mesh.node(2).x != 0.01 || pressure_mesh.node(2).y != -0.005) {
    throw std::runtime_error(sb() 
      << "Mismatch with third node: "
      << "(" << pressure_mesh.node(2).x << "," << pressure_mesh.node(2).y << ")"
    );
  }

  if (pressure_mesh.node(3).x != 0.01 || pressure_mesh.node(3).y != 0.005) {
    throw std::runtime_error(sb() 
      << "Mismatch with fourth node: "
      << "(" << pressure_mesh.node(3).x << "," << pressure_mesh.node(3).y << ")"
    );
  }
  return pressure_mesh;
}

BOOST_AUTO_TEST_CASE(two_pressure_elements)
{
  cm::MeshRectangularBase pressure_mesh = generate_pressures_mesh();
  MockMesh disps_mesh = generate_disps_mesh();
  cm::SkinAttributes skin_attr = generate_skin_attr();

  arma::mat expected(3,4);
  expected
    <<  2.45988911657728e-09 << -2.16107966933311e-10 << -3.75074813275411e-11 << -2.5150807594233e-11 << arma::endr
    <<  3.13761739997307e-10 <<  3.13761739997307e-10 <<   3.1376173999729e-10 <<   3.137617399973e-10 << arma::endr
    << -5.32421873288007e-11 << -8.80372111993546e-11 << -3.98913429923369e-10 << 2.12497799334448e-09 << arma::endr;

  arma::mat calculated = cm::details::pressures_to_displacements_matrix(
    pressure_mesh,
    disps_mesh,
    skin_attr
  );

  CHECK_CLOSE_COLLECTION(calculated, expected, 1e-5);

  arma::mat expected_pinv(4,3);
  expected_pinv
    <<  387011438.208556 <<  141875358.77385 << -13995419.3511459 << arma::endr
    <<  -189503898.19852 << 1336658241.44806 << -166734005.195562 << arma::endr
    << -167844429.803222 << 1388928221.52505 << -237938130.374302 << arma::endr
    <<  -29663110.206823 << 319669917.159797 <<  418667554.921006 << arma::endr;

  arma::mat calculated_pinv = cm::details::displacements_to_pressures_matrix(
    disps_mesh,
    pressure_mesh,
    skin_attr
  );

  CHECK_CLOSE_COLLECTION(calculated_pinv, expected_pinv, 1e-5);
}

BOOST_AUTO_TEST_CASE(alg_pressures_to_disps)
{
  cm::MeshRectangularBase pressure_mesh = generate_pressures_mesh();
  MockMesh disps_mesh = generate_disps_mesh();

  typedef cm::AlgPressuresToDisplacements A_p_d;
  A_p_d::params_type params_p_d;
  params_p_d.skin_props = generate_skin_attr();

  boost::any pre_p_d = A_p_d().offline(pressure_mesh, disps_mesh, params_p_d);

  pressure_mesh.setValue(0,0,0.489764395788231);
  pressure_mesh.setValue(1,0,0.445586200710899);
  pressure_mesh.setValue(2,0,0.646313010111265);
  pressure_mesh.setValue(3,0,0.709364830858073);

  A_p_d().run(pressure_mesh, disps_mesh, params_p_d, pre_p_d);
  std::vector<double> expected_disps_values;
  expected_disps_values.push_back(1.06638870742382e-09);
  expected_disps_values.push_back(7.18837068923401e-10);
  expected_disps_values.push_back(1.18425742099131e-09);

  CHECK_CLOSE_COLLECTION(disps_mesh.getRawValues(), expected_disps_values, 1e-5);
}

BOOST_AUTO_TEST_CASE(alg_disps_to_pressures)
{
  cm::MeshRectangularBase pressure_mesh = generate_pressures_mesh();
  MockMesh disps_mesh = generate_disps_mesh();
  cm::SkinAttributes skin_attr = generate_skin_attr();

  typedef cm::AlgDisplacementsToPressures A_d_p;
  A_d_p::params_type params_d_p;
  params_d_p.skin_props = generate_skin_attr();

  boost::any pre_d_p = A_d_p().offline(disps_mesh, pressure_mesh, params_d_p);
  disps_mesh.setValue(0,0,7.54686681982361e-06);
  disps_mesh.setValue(1,0,2.76025076998578e-06);
  disps_mesh.setValue(2,0,6.79702676853675e-06);

  A_d_p().run(disps_mesh, pressure_mesh, params_d_p, pre_d_p);
  std::vector<double> expected_pressure_values;
  expected_pressure_values.push_back(3217.20811023926);
  expected_pressure_values.push_back(1126.05576208363);
  expected_pressure_values.push_back(949.818793335635);
  expected_pressure_values.push_back(3504.20017070488);
 
  CHECK_CLOSE_COLLECTION(pressure_mesh.getRawValues(), expected_pressure_values, 1e-5);
}

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

/*
 * Test solutions developed with a Matlab(R) prototype of the solutions. To generate output, just
 * run generate in Matlab(R).
 *
 * The script, compressed with xz and encoded as base64 (to uncompress, base64 -d | tar xJvf -):
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
oAEAaHhU/bHEZ/sCAAAAAARZWg==
*/

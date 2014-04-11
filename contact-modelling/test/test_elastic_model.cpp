#define BOOST_TEST_MODULE elastic_model
#include <boost/test/unit_test.hpp>
#include "inc/custom_test_macros.hpp"

#include <cstddef>
#include <array>
#include <vector>

#include "AlgForcesToDisplacements.hpp"

#include "external/armadillo.hpp"
#include "MeshInterface.hpp"

struct MockNode {
  double x;
  double y;
};

class MockMesh : public MeshInterface {
private:
  size_t offset;

public:
  MockMesh(size_t dim, size_t no_nodes) : MeshInterface(dim, no_nodes), offset(0) {}
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

// -- TEST CASES: mesh nodes "far away", i.e. dx > eps_samepoint && dy > eps_samepoint
// !begin

BOOST_AUTO_TEST_CASE(em_other_nodes_1_1_dim_3_3)
{
  MockMesh forces_mesh(3, 1);
  MockMesh disps_mesh(3, 1);

  // note: for generator dx = 0.0005, dy = 0.0007 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1005,0.1007});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
    0.000284267757585335,
    0.000136847848007961,
    -2.57040388378672e-05,
    0.000136847848007961,
    0.000378106281933651,
    -3.59856543730141e-05,
    -2.57040388378672e-05,
    -3.59856543730141e-05,
    8.37031393710367e-05
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_other_nodes_1_1_dim_1_3)
{
  MockMesh forces_mesh(3, 1);
  MockMesh disps_mesh(1, 1);

  // note: for generator dx = 0.0002, dy = 0.0003 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1002,0.1003});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
    -1.2641650958631e-05,
    -1.89624764379465e-05,
    0.000478752428959904
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_other_nodes_1_1_dim_3_1)
{
  MockMesh forces_mesh(1, 1);
  MockMesh disps_mesh(3, 1);

  // note: for generator dx = 0.0003, dy = 0.0004 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1003,0.1004});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
    -1.81650558119599e-05,
    -2.42200744159466e-05,
    0.000280746959558536
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_other_nodes_1_1_dim_1_1)
{
  MockMesh forces_mesh(1, 1);
  MockMesh disps_mesh(1, 1);

  // note: for generator dx = 0.0001, dy = 0.0004 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1001,0.1004});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
    0.000388846272447214
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}
// !end


// -- TEST CASES: mesh nodes "very close", i.e. dx < eps_samepoint && dy < eps_samepoint
// !begin

BOOST_AUTO_TEST_CASE(em_same_nodes_1_1_dim_3_3)
{
  MockMesh forces_mesh(3, 1);
  MockMesh disps_mesh(3, 1);

  // note: for generator dx = 1e-5, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1 + 1e-5, 0.1 + 1e-6});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
    1.93137101011595e-05,
                       0,
                       0,
                       0,
    1.93137101011595e-05,
                       0,
                       0,
                       0,
     3.8627420202319e-05

  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_same_nodes_1_1_dim_1_3)
{
  MockMesh forces_mesh(3, 1);
  MockMesh disps_mesh(1, 1);

  // note: for generator dx = 1e-5, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1 + 1e-5, 0.1 + 1e-6});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
                      0,
                      0,
    3.8627420202319e-05
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_same_nodes_1_1_dim_3_1)
{
  MockMesh forces_mesh(1, 1);
  MockMesh disps_mesh(3, 1);

  // note: for generator dx = 1e-5, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1 + 1e-5, 0.1 + 1e-6});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
                      0,
                      0,
    3.8627420202319e-05
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_same_nodes_1_1_dim_1_1)
{
  MockMesh forces_mesh(1, 1);
  MockMesh disps_mesh(1, 1);

  // note: for generator dx = 1e-5, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1 + 1e-5, 0.1 + 1e-6});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
    3.8627420202319e-05
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}
// !end

// -- TEST CASES: mesh nodes "very close" in one direction (i.e. dx > eps_samepoint xor dy > eps_samepoint)
// !begin

BOOST_AUTO_TEST_CASE(em_onedirx_nodes_1_1_dim_3_3)
{
  MockMesh forces_mesh(3, 1);
  MockMesh disps_mesh(3, 1);

  // note: for generator dx = 1e-6, dy = 0.0007 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1 + 1e-6, 0.1007});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
    0.000253757610931623,
    5.21825396597814e-07,
    -5.57608288082757e-08,
    5.21825396597814e-07,
     0.00061903464308524,
    -3.9032580165793e-05,
    -5.57608288082757e-08,
    -3.9032580165793e-05,
    0.000142235207850219
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_onediry_nodes_1_1_dim_3_3)
{
  MockMesh forces_mesh(3, 1);
  MockMesh disps_mesh(3, 1);

  // note: for generator dx = 0.0005, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1, 0.1});
  disps_mesh.add_node({0.1 + 0.0005, 0.1 + 1e-6});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
    0.000924790809052427,
    1.04588904661277e-06,
    -3.02750823346094e-05,
    1.04588904661277e-06,
    0.000401848377524135,
    -6.05501646692188e-08,
    -3.02750823346094e-05,
    -6.05501646692188e-08,
    0.000280745956407604
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_onedirx_nodes_1_1_dim_1_3)
{
  MockMesh forces_mesh(3, 1);
  MockMesh disps_mesh(1, 1);

  // note: for generator dx = 1e-6, dy = 0.0003 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1 + 1e-6, 0.1003});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
    -6.4137755190822e-08,
    -1.92413265572466e-05,
    0.000624751964323888
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_onediry_nodes_1_1_dim_1_3)
{
  MockMesh forces_mesh(3, 1);
  MockMesh disps_mesh(1, 1);

  // note: for generator dx = 0.0002, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1002, 0.1 + 1e-6});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
    -1.30664215002714e-05,
    -6.53321075013572e-08,
     0.00106363950928189
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_onedirx_nodes_1_1_dim_3_1)
{
  MockMesh forces_mesh(1, 1);
  MockMesh disps_mesh(3, 1);

  // note: for generator dx = 1e-6, dy = 0.0004 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1 + 1e-6, 0.1004});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
    -6.25257233026628e-08,
    -2.50102893210651e-05,
    0.000408038541558231
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_onediry_nodes_1_1_dim_3_1)
{
  MockMesh forces_mesh(1, 1);
  MockMesh disps_mesh(3, 1);

  // note: for generator dx = 0.0003, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1003, 0.1 + 1e-6});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
    -1.92413265572466e-05,
    -6.4137755190822e-08,
    0.000624751964323888
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_onedirx_nodes_1_1_dim_1_1)
{
  MockMesh forces_mesh(1, 1);
  MockMesh disps_mesh(1, 1);

  // note: for generator dx = 1e-6, dy = 0.0004 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1 + 1e-6, 0.1004});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
    0.000408038541558231
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_onediry_nodes_1_1_dim_1_1)
{
  MockMesh forces_mesh(1, 1);
  MockMesh disps_mesh(1, 1);

  // note: for generator dx = 0.0002, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1002, 0.1 + 1e-6});

  typedef AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  std::vector<double> expected = {
    0.00106363950928189
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);
}
// !end

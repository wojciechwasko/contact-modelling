#define BOOST_TEST_MODULE test_mesh_interface
#include <boost/test/unit_test.hpp>
#include "inc/custom_test_macros.hpp"

#include "MeshRegularSquare.hpp"

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

BOOST_AUTO_TEST_CASE(test_clone_structure)
{
  MockMesh s(4, 2);
  s.add_node({1.23,4.56});
  s.add_node({2.34,5.67});

  MeshRegularSquare t(1, 0.01);
  t.clone_structure((const MeshInterface&) s);

  BOOST_CHECK_EQUAL(t.no_nodes(), s.no_nodes());
  BOOST_CHECK_EQUAL(t.getRawValues().size(), 1*s.no_nodes());
  const double eps = 1e-6;
  BOOST_CHECK_CLOSE(t.node(0).x, s.node(0).x, eps);
  BOOST_CHECK_CLOSE(t.node(0).y, s.node(0).y, eps);
  BOOST_CHECK_CLOSE(t.node(1).x, s.node(1).x, eps);
  BOOST_CHECK_CLOSE(t.node(1).y, s.node(1).y, eps);
  BOOST_CHECK_NO_THROW(t.getMetadata(0));
  BOOST_CHECK_NO_THROW(t.getMetadata(1));
};

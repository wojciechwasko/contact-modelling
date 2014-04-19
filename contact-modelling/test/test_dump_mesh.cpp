#define BOOST_TEST_MODULE dump_mesh
#include <boost/test/unit_test.hpp>
#include "inc/custom_test_macros.hpp"

#include <vector>
#include <sstream>
#include <fstream>
#include <boost/filesystem.hpp>

#include "MeshInterface.hpp"
#include "helpers/plot.hpp"

struct MockNode {
  double x;
  double y;
};

class MockMesh_nodxdy : public MeshInterface {
private:
  size_t offset;

public:
  MockMesh_nodxdy(size_t dim, size_t no_nodes) : MeshInterface(dim, no_nodes), offset(0) {}
  void add_node(MockNode n, std::vector<double> vals)
  {
    node(offset).x = n.x;
    node(offset).y = n.y;
    for (size_t i = 0; i < D; ++i)
      setValue(offset, i, vals.at(i));
    ++offset;
  }

private:
  double impl_node_area(size_t i) const
  {
    return 0.02;
  }
};

class MockMesh_dxdy : public MockMesh_nodxdy {
public:
  MockMesh_dxdy(size_t dim, size_t no_nodes) : MockMesh_nodxdy(dim, no_nodes) {}
private:
  double impl_dx() const
  {
    return 0.002;
  }

  double impl_dy() const
  {
    return 0.001;
  }
};

BOOST_AUTO_TEST_CASE(dump_to_ss)
{
  MockMesh_dxdy m(1,2);
  m.add_node({0.1, 0.2}, {1.23});
  m.add_node({0.3, 0.4}, {5.67});
  std::stringstream ss;
  helpers::plot::dumpForPlot(m, ss);

  // comparing strings would be pretty useless due to the differences in precision between stream
  // output values on various platforms. We'll read the values from the ss and compare floats by
  // BOOST_CHECK_CLOSE
  std::vector<double> result(std::istream_iterator<double>(ss), {});
  std::vector<double> expected({0.002,0.001,0.1,0.3,0.2,0.4,1.23,5.67});
  CHECK_CLOSE_COLLECTION(result,expected, 0.1);
};

BOOST_AUTO_TEST_CASE(dump_to_ss_nodxdy)
{
  MockMesh_nodxdy m(1,2);
  m.add_node({0.1, 0.2}, {1.23});
  m.add_node({0.3, 0.4}, {5.67});
  std::stringstream ss;
  helpers::plot::dumpForPlot(m, ss);

  // comparing strings would be pretty useless due to the differences in precision between stream
  // output values on various platforms. We'll read the values from the ss and compare floats by
  // BOOST_CHECK_CLOSE
  std::vector<double> result(std::istream_iterator<double>(ss), {});
  // dx and dy are == sqrt(node_area(0)) == 0.14142
  std::vector<double> expected({0.14142,0.14142,0.1,0.3,0.2,0.4,1.23,5.67});
  CHECK_CLOSE_COLLECTION(result,expected, 0.1);
};

BOOST_AUTO_TEST_CASE(dump_to_f)
{
  boost::filesystem::path tmp = boost::filesystem::unique_path("test-dump-mesh-%%%%-%%%%-%%%%-%%%%");
  const std::string tmpstr    = tmp.native();  // optional

  MockMesh_dxdy m(1,2);
  m.add_node({0.1, 0.2}, {1.23});
  m.add_node({0.3, 0.4}, {5.67});
  helpers::plot::dumpForPlot(m, tmpstr);

  // comparing strings would be pretty useless due to the differences in precision between stream
  // output values on various platforms. We'll read the values from the ss and compare floats by
  // BOOST_CHECK_CLOSE
  std::ifstream i(tmpstr);
  std::vector<double> result(std::istream_iterator<double>(i), {});
  std::vector<double> expected({0.002,0.001,0.1,0.3,0.2,0.4,1.23,5.67});
  CHECK_CLOSE_COLLECTION(result,expected, 0.1);
};

BOOST_AUTO_TEST_CASE(dump_to_f_nodxdy)
{
  boost::filesystem::path tmp = boost::filesystem::unique_path("test-dump-mesh-%%%%-%%%%-%%%%-%%%%");
  const std::string tmpstr    = tmp.native();  // optional

  MockMesh_nodxdy m(1,2);
  m.add_node({0.1, 0.2}, {1.23});
  m.add_node({0.3, 0.4}, {5.67});
  helpers::plot::dumpForPlot(m, tmpstr);

  // comparing strings would be pretty useless due to the differences in precision between stream
  // output values on various platforms. We'll read the values from the ss and compare floats by
  // BOOST_CHECK_CLOSE
  std::ifstream i(tmpstr);
  std::vector<double> result(std::istream_iterator<double>(i), {});
  // dx and dy are == sqrt(node_area(0)) == 0.14142
  std::vector<double> expected({0.14142,0.14142,0.1,0.3,0.2,0.4,1.23,5.67});
  CHECK_CLOSE_COLLECTION(result,expected, 0.1);
};

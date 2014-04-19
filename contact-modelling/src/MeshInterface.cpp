#include "MeshInterface.hpp"

#include <algorithm>
#include <fstream>
#include <limits>

#include "helpers/container_algorithms.hpp"
#include "helpers/log.hpp"

MeshInterface::MeshInterface(const size_t dim, const size_t no_nodes)
  : D(dim),
    values_(dim * no_nodes),
    nodes_(no_nodes),
    metadata_(no_nodes)
{}

size_t
MeshInterface::no_nodes() const
{
  return nodes_.size(); 
}

size_t
MeshInterface::size() const
{
  return no_nodes(); 
}

MeshInterface::nodes_iterator
MeshInterface::nodes_begin() 
{ 
  return nodes_.begin(); 
}

MeshInterface::nodes_const_iterator
MeshInterface::nodes_cbegin() const
{
  return nodes_.cbegin();
}

MeshInterface::nodes_iterator
MeshInterface::nodes_end()
{
  return nodes_.end();
}

MeshInterface::nodes_const_iterator 
MeshInterface::nodes_cend() const
{
  return nodes_.cend();
}

const MeshInterface::node_type& 
MeshInterface::node(size_t i) const
{
  return nodes_.at(i);
}

double 
MeshInterface::node_area(size_t i) const
{
  return impl_node_area(i);
}

MeshInterface::node_type& 
MeshInterface::node(size_t i)
{
  return nodes_.at(i);
}

const MeshInterface::values_container& 
MeshInterface::getRawValues() const
{
  return values_;
}

MeshInterface::values_container& 
MeshInterface::getRawValues() 
{
  return values_;
}

std::vector<MeshInterface::value_type>
MeshInterface::getValues(size_t i) const
{
  std::vector<value_type> ret;
  ret.resize(D);
  for (size_t it = 0; it < D; ++it)
    ret.at(it) = values_.at(i*D + it);
  return ret;
}

MeshInterface::value_type 
MeshInterface::getValue(size_t i, size_t vi) const
{
  return values_.at(i*D + vi);
}

void 
MeshInterface::setValue(size_t i, size_t vi, value_type v)
{
  values_.at(i*D + vi) = v;
}

const MeshInterface::metadata_type&
MeshInterface::getMetadata(const size_t i) const
{
  return metadata_.at(i);
}

void 
MeshInterface::setMetadata(const size_t i, const metadata_type& rhs)
{
  metadata_.at(i) = rhs;
}

void 
MeshInterface::setBadNodes(const MeshInterface::bad_nodes_type& rhs)
{
  bad_nodes_.assign(rhs.cbegin(), rhs.cend());
}

const MeshInterface::bad_nodes_type&
MeshInterface::getBadNodes() const
{
  return bad_nodes_;
}

MeshInterface::bad_nodes_type&
MeshInterface::getBadNodes()
{
  return bad_nodes_;
}

void 
MeshInterface::erase(const std::vector<size_t>& indices)
{
  if (indices.size() == 0)
    return;
  using helpers::container_algorithms::erase_by_indices;
  erase_by_indices(nodes_,      indices);
  erase_by_indices(metadata_,   indices);
  erase_by_indices(values_,     indices, D);
  generateMinMax();
}

void
MeshInterface::generateMinMax()
{
  double min_x = std::numeric_limits<double>::max();
  double min_y = std::numeric_limits<double>::max();
  double max_x = std::numeric_limits<double>::lowest();
  double max_y = std::numeric_limits<double>::lowest();

  std::for_each(this->nodes_cbegin(), this->nodes_cend(), [&](const node_type& n) {
    if (n.x < min_x) min_x = n.x;
    if (n.x > max_x) max_x = n.x;
    if (n.y < min_y) min_y = n.y;
    if (n.y > max_y) max_y = n.y;
  });
  // save min/max to member variables
  min_x_ = min_x;
  min_y_ = min_y;
  max_x_ = max_x;
  max_y_ = max_y;
}

double
MeshInterface::minX() const
{
  return min_x_;
}

double
MeshInterface::minY() const
{
  return min_y_;
}

double
MeshInterface::maxX() const
{
  return max_x_;
}

double
MeshInterface::maxY() const
{
  return max_y_;
}

double
MeshInterface::dx() const
{
  return impl_dx();
}

double
MeshInterface::dy() const
{
  return impl_dy();
}

double 
MeshInterface::impl_dx() const
{
  LOG(WARN) << "Using default implementation of MeshInterface's dx, which means we're simplifying "
    << "all the nodes in the mesh to be represented as square-based, with length of a side of the "
    << "square defined as square root of the first node's area";
  return sqrt(node_area(0));
}

double 
MeshInterface::impl_dy() const
{
  LOG(WARN) << "Using default implementation of MeshInterface's dy, which means we're simplifying "
    << "all the nodes in the mesh to be represented as square-based, with length of a side of the "
    << "square defined as square root of the first node's area";
  return sqrt(node_area(0));
}

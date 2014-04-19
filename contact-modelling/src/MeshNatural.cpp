#include "MeshNatural.hpp"

#include <stdexcept>

double 
MeshNatural::impl_node_area(size_t i) const
{
  if (isForSkin())
    return taxelArea_;
  else
    throw std::runtime_error("Node areas for Natural Mesh are not yet implemented. "
    "Do it if you have the time! :D");
}

void
MeshNatural::setForSkin()
{
  forSkin_ = true;
}

void
MeshNatural::setNotForSkin()
{
  forSkin_ = false;
}

bool
MeshNatural::isForSkin() const
{
  return forSkin_;
}


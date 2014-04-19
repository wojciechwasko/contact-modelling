#include "SkinProviderInterface.hpp"
#include "MeshNatural.hpp"


SkinProviderInterface::SkinProviderInterface(
  const size_t dim
)
  :
    D(dim)
{}

MeshNatural*
SkinProviderInterface::createMesh() const
{
  MeshNatural* m = impl_createMesh();
  m->setForSkin();
  return m; 
}

void
SkinProviderInterface::update(SkinProviderInterface::target_values_type& target_vec) const
{
  impl_update(target_vec);
}

SkinAttributes
SkinProviderInterface::getAttributes() const
{
  return impl_getAttributes();
}

#include "SkinProviderInterface.hpp"


SkinProviderInterface::SkinProviderInterface(
  const size_t dim
)
  :
    D(dim)
{}

MeshNatural*
SkinProviderInterface::createMesh() const
{
  return impl_createMesh(); 
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

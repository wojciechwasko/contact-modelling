#include "SkinProviderInterface.hpp"


SkinProviderInterface::SkinProviderInterface(
  const size_t dim,
  SkinProviderInterface::converter_type conv
)
  :
    D(dim),
    converter_(conv)
{}

MeshNatural*
SkinProviderInterface::createMesh()
{
  return impl_createMesh(); 
}

void
SkinProviderInterface::update(SkinProviderInterface::target_values_type& target_vec)
{
  impl_update(target_vec);
}

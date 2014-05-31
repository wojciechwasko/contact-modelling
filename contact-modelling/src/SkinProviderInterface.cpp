#include "cm/skin_provider/interface.hpp"
#include "cm/grid/grid.hpp"

namespace cm {

SkinProviderInterface::SkinProviderInterface(
  const size_t dim
)
  :
    D(dim)
{}

Grid*
SkinProviderInterface::createGrid() const
{
  return impl_createGrid();
}

SkinProviderInterface::target_values_type
SkinProviderInterface::update(void) const
{
  return impl_update();
}

SkinAttributes
SkinProviderInterface::getAttributes() const
{
  return impl_getAttributes();
}

} /* namespace cm */

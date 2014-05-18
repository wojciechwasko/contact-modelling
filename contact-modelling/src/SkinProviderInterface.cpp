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

} /* namespace cm */

#include "cm/algorithm/interface.hpp"

namespace cm {

boost::any AlgInterface::offline(
  const Grid& input,
  const Grid& output,
  const boost::any& params
)
{
  // automatic wrapping in boost::any
  return impl_offline(input, output, params);
}

void AlgInterface::run(
  const Grid& input,
        Grid& output,
  const boost::any& params,
  const boost::any& precomputed
)
{
  impl_run(input,output,params,precomputed);
}

} /* namespace cm */

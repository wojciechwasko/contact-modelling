#include "cm/algorithm/displacements_to_nonnegative_normal_forces.hpp"

#include <stdexcept>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>

#include "cm/details/external/armadillo.hpp"
#include "libtsnnls/tsnnls.h"

#include "cm/grid/grid.hpp"
#include "cm/log/log.hpp"
#include "cm/details/string.hpp"
#include "cm/details/elastic_model_boussinesq.hpp"

namespace cm {
using details::sb;

/**
 * \cond DEV
 */
namespace details {
struct precomputed_type {
  typedef std::shared_ptr<taucs_ccs_matrix> sh_ptr_type;
  sh_ptr_type taucs_m;
};
}
/**
 * \endcond
 */

boost::any AlgDisplacementsToNonnegativeNormalForces::impl_offline(
  const Grid& disps,
  const Grid& forces,
  const boost::any& params
)
{
  if (disps.dim() != 1 && disps.dim() != 3)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the displacements grid: "
            << disps.dim() << "; supported dimensionalities: (1,3)"
    );

  if (forces.dim() != 1)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the forces grid: "
            << disps.dim() << "; supported dimensionalities: (1,)"
    );

  using cm::details::forces_to_displacements_matrix;
  const params_type& p = boost::any_cast<const params_type&>(params);
  arma::mat fd_matrix  = forces_to_displacements_matrix(forces, disps, p.skin_props);
  // 1st : taucs_construct_sorted_ccs_matrix requires row-major ordering (as per README of
  // libtsnnls).
  std::vector<double> tempvec;
  tempvec.reserve(fd_matrix.size());
  std::for_each(
    fd_matrix.begin_row(0),
    fd_matrix.end_row(fd_matrix.n_rows - 1),
    [&](double value) {
      tempvec.push_back(value);
    }
  );

  details::precomputed_type ret;
  ret.taucs_m = details::precomputed_type::sh_ptr_type(
    taucs_construct_sorted_ccs_matrix(tempvec.data(), fd_matrix.n_cols, fd_matrix.n_rows),
    taucs_ccs_free
  );

  return ret;
}

void AlgDisplacementsToNonnegativeNormalForces::impl_run(
  const Grid& disps,
        Grid& forces,
  const boost::any& params,
  const boost::any& precomputed
)
{
  if (disps.dim() != 1 && disps.dim() != 3)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the displacements grid: "
            << disps.dim() << "; supported dimensionalities: (1,3)"
    );

  if (forces.dim() != 1)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the forces grid: "
            << disps.dim() << "; supported dimensionalities: (1,)"
    );

  const details::precomputed_type& pre = boost::any_cast<details::precomputed_type>(precomputed);
  // taucs_double is just double (as per taucs.h:117
  // so when we have to pass taucs_double* as b, we can take the data from disps.getRawValues()
  // (which is std::vector<double>)
  double residualNorm;
  // libtsnnls requires double* instead of const double* (meh), so we either have to const_cast the
  // stuff or create a local copy
  std::vector<double> d_vals;
  d_vals.assign(disps.getRawValues().cbegin(), disps.getRawValues().cend());
  double* solution = t_snnls(pre.taucs_m.get(), d_vals.data(), &residualNorm, -1, 1);
  if (!solution) {
    throw std::runtime_error(
      sb() << "libtsnnls returned nullptr as the solution"
    );
  }
  LOG(DEBUG) << "nnls residual norm: " << residualNorm;
  forces.getRawValues().clear();
  forces.getRawValues().reserve(pre.taucs_m->n);
  for (size_t i = 0; i < (size_t)pre.taucs_m->n; ++i) {
    forces.getRawValues().push_back(*(solution+i));
  }
  free(solution);
}

} /* namespace cm */

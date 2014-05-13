#include "AlgDisplacementsToNonnegativePressures.hpp"

#include <stdexcept>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>

#include "libtsnnls/tsnnls.h"

#include "MeshInterface.hpp"
#include "MeshRegularRectangular.hpp"
#include "external/armadillo.hpp"
#include "helpers/string.hpp"
#include "helpers/log.hpp"

struct precomputed_type {
  typedef std::shared_ptr<taucs_ccs_matrix> sh_ptr_type;
  sh_ptr_type taucs_m;
};
using helpers::string::sb;

boost::any
AlgDisplacementsToNonnegativePressures::impl_offline(
  const MeshInterface& disps,
  const MeshInterface& pressures,
  const boost::any& params
)
{
  const MeshRegularRectangular* p_mesh;
  try {
    p_mesh = dynamic_cast<const MeshRegularRectangular*>(&pressures);
  } catch (const std::bad_cast& e) {
    throw std::runtime_error(
      sb()  << "Error downcasting MeshInterface to MeshRegularRectangular. "
            << "Note: only rectangular-element-based meshes are currently supported for "
            << "pressures calculations. Original exception message: " << e.what()
    );
  }

  if (disps.D != 1)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the displacements mesh: "
            << disps.D << "; supported dimensionalities: (1,)"
    );

  if (pressures.D != 1)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the pressures mesh: "
            << disps.D << "; supported dimensionalities: (1,)"
    );

  using helpers::elastic_linear_model::pressures_to_displacements_matrix;
  const params_type& p = boost::any_cast<const params_type&>(params);
  arma::mat pd_matrix  = pressures_to_displacements_matrix(*p_mesh, disps, p.skin_props);
  // taucs_construct_sorted_ccs_matrix requires row-major ordering (as per README of libtsnnls).
  std::vector<double> tempvec;
  tempvec.reserve(pd_matrix.size());
  std::for_each(
    pd_matrix.begin_row(0),
    pd_matrix.end_row(pd_matrix.n_rows - 1),
    [&](double value) {
      tempvec.push_back(value);
    }
  );

  precomputed_type ret;
  ret.taucs_m = precomputed_type::sh_ptr_type(
    taucs_construct_sorted_ccs_matrix(tempvec.data(), pd_matrix.n_cols, pd_matrix.n_rows),
    taucs_ccs_free
  );

  return ret;
}

void
AlgDisplacementsToNonnegativePressures::impl_run(
  const MeshInterface& disps,
        MeshInterface& pressures,
  const boost::any& params,
  const boost::any& precomputed
)
{
  if (disps.D != 1)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the displacements mesh: "
            << disps.D << "; supported dimensionalities: (1,)"
    );

  if (pressures.D != 1)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the pressures mesh: "
            << disps.D << "; supported dimensionalities: (1,)"
    );

  const precomputed_type& pre = boost::any_cast<precomputed_type>(precomputed);
  // taucs_double is just double (as per taucs.h:117
  // so when we have to pass taucs_double* as b, we can take the data from disps.getRawValues()
  // (which is std::vector<double>)
  double residualNorm;
  // libtsnnls requires double* instead of const double* (meh), so we either have to const_cast the
  // stuff or create a local copy
  std::vector<double> d_vals;
  d_vals.assign(disps.getRawValues().cbegin(), disps.getRawValues().cend());
  double* solution = t_snnls(pre.taucs_m.get(), d_vals.data(), &residualNorm, -1, 1);
  LOG(DEBUG) << "nnls residual norm: " << residualNorm;
  pressures.getRawValues().clear();
  pressures.getRawValues().reserve(pre.taucs_m->n);
  for (size_t i = 0; i < pre.taucs_m->n; ++i) {
    pressures.getRawValues().push_back(*(solution+i));
  }
  free(solution);
}

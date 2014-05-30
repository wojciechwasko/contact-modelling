#include <stdexcept>

#include "reconstruction.hpp"

cm::SkinProviderInterface* getSkinProvider(const options_type& options);

suite_type construct_suite(const options_type& opts)
{
  suite_type ret;
  ret.skin_provider.reset(getSkinProvider(opts));
  ret.raw_grid.reset(ret.skin_provider->createGrid());
  if (opts.source_pitch > 0) {
    ret.interp_grid.reset(cm::Grid::fromFill(1, cm::Square(opts.source_pitch), *(ret.raw_grid)));
    ret.interpolator.reset(new cm::InterpolatorLinearDelaunay(opts.interpolator_policy));
  }
  ret.tractions_grid.reset(cm::Grid::fromFill(1, cm::Square(opts.tractions_pitch), *(ret.raw_grid)));
  ret.reconstructed_grid.reset(cm::Grid::fromFill(1, cm::Square(opts.reconstructed_pitch), *(ret.raw_grid)));

  if (opts.traction_type == TractionType::pressures) {
    ret.to_reconstructed.reset(new cm::AlgPressuresToDisplacements());
    auto tmp = cm::AlgPressuresToDisplacements::params_type();
    tmp.skin_props = ret.skin_provider->getAttributes();
    ret.to_reconstructed_params = tmp;
    if (opts.nonnegative_tractions) {
      ret.to_tractions.reset(new cm::AlgDisplacementsToNonnegativePressures());
      auto tmp = cm::AlgDisplacementsToNonnegativePressures::params_type();
      tmp.skin_props = ret.skin_provider->getAttributes();
      ret.to_tractions_params = tmp;
    } else {
      ret.to_tractions.reset(new cm::AlgDisplacementsToPressures());
      auto tmp = cm::AlgDisplacementsToPressures::params_type();
      tmp.skin_props = ret.skin_provider->getAttributes();
      ret.to_tractions_params = tmp;
    }
  } else if (opts.traction_type == TractionType::forces) {
    ret.to_reconstructed.reset(new cm::AlgForcesToDisplacements());
    auto tmp = cm::AlgForcesToDisplacements::params_type();
    tmp.skin_props = ret.skin_provider->getAttributes();
    ret.to_reconstructed_params = tmp;
    if (opts.nonnegative_tractions) {
      ret.to_tractions.reset(new cm::AlgDisplacementsToNonnegativeNormalForces());
      auto tmp = cm::AlgDisplacementsToNonnegativeNormalForces::params_type();
      tmp.skin_props = ret.skin_provider->getAttributes();
      ret.to_tractions_params = tmp;
    } else {
      ret.to_tractions.reset(new cm::AlgDisplacementsToForces());
      auto tmp = cm::AlgDisplacementsToForces::params_type();
      tmp.skin_props = ret.skin_provider->getAttributes();
      ret.to_tractions_params = tmp;
    }
  } else {
    throw std::runtime_error("Unknown traction type while constructing the suite.");
  }

  return ret;
}

cm::SkinProviderInterface* getSkinProvider(const options_type& options)
{
  if (options.input_type == InputType::yaml) {
    return new cm::SkinProviderYaml(options.input);
  } else if (options.input_type == InputType::luca) {
    return new cm::SkinProviderLuca(options.input);
  }
  throw std::runtime_error("getSkinProvider: unrecognized option for InputType");
}

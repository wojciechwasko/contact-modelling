#ifndef SKINWAREPROVIDER_HPP
#define SKINWAREPROVIDER_HPP

#include <cstdint>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include "SkinAttributes.hpp"
#include "SkinProviderInterface.hpp"
#include "MeshNatural.hpp"

#include "skin.h"
#include "skin_sensor.h"
#include "skin_object.h"
#include "skin_module.h"
#include "skin_patch.h"
#include "skin_region.h"
#include "skin_sub_region.h"

#include "helpers/skin.hpp"
#include "helpers/traits.hpp"

class MeshNatural;

/**
 * \brief   Interface abstracting SkinWare.
 * \tparam  source  where are the sensors coming from? Unfortunately SkinWare's skin/layer/module
 *                  sensor iterators are not inter-operable.
 *
 * Here be dragons. To be cleaned up and covered by unit tests, up to the possible extent.
 */
template <class source=skin_object>
class SkinWareProvider : public SkinProviderInterface
{
public:
  using typename SkinProviderInterface::converter_type;
  using typename SkinProviderInterface::target_values_type;

public:
  /**
   * \brief Takes whole skin (skin_object) as the source of sensors.
   * \param conv  Fuction pointer; function shall take a uint16_t and return a double in [m].
   *
   * I know function pointers are ugly. But they work for now.
   *
   * In a more elaborate version, we could use sensors coming only from one given module or such.
   * Though we'd probably have to make this class templated and expose the correct iterator type
   * (what a PITA).
   */
  template <class SS=source, helpers::traits::EnableIf<std::is_same<SS, skin_object>>...>
  SkinWareProvider(converter_type converter, SkinAttributes attr)
    :
      SkinProviderInterface(1, converter), // hard-coded to 1D only
      sensor_layer_(SKIN_ALL_SENSOR_TYPES),
      s_object_(),
      s_reader_(s_object_.reader()),
      sensors_source_(&s_object_),
      skin_attributes_(attr)
  {
    init();
  }

  /**
   * \brief Takes a part of the skin (source) as the source of sensors.
   * \param conv  Fuction pointer; function shall take a uint16_t and return a double in [m].
   *
   * I know function pointers are ugly. But they work for now.
   *
   * In a more elaborate version, we could use sensors coming only from one given module or such.
   * Though we'd probably have to make this class templated and expose the correct iterator type
   * (what a PITA).
   */
//  template <class SS=source, traits_helpers::DisableIf<std::is_same<SS, skin_object>>...>
  template <class SS=source, helpers::traits::DisableIf<std::is_same<SS, skin_object>>...>
  SkinWareProvider(converter_type converter, source& sensors_source, SkinAttributes attr)
    :
      SkinProviderInterface(1, converter), // hard-coded to 1D only
      sensor_layer_(SKIN_ALL_SENSOR_TYPES),
      s_object_(),
      s_reader_(s_object_.reader()),
      sensors_source_(&sensors_source),
      skin_attributes_(attr)
  {
    init();
  }

private:
  typedef typename source::skin_sensor_iterator sensor_iterator;
  /**
   * First - fail (true/false), second - failure text.
   */
  typedef std::pair<bool, std::string> fail_error;

  const skin_sensor_type_id sensor_layer_;
  skin_object     s_object_;
  // FIXME check if this pointer needs to deleted at destruction <- SkinWare docs
  skin_reader*    s_reader_;
  source*  sensors_source_;
  // it's not const since we have to do it after the skin initialisation and without extra magic
  // it'd be too hard/tricky to make it const
  std::size_t     no_sensors_; 
  sensor_iterator s_begin_;
  sensor_iterator s_end_;
  SkinAttributes  skin_attributes_;

  void init()
  {
    fail_error ret_load = loadSkin(&s_object_);
    if (ret_load.first)
      throw std::runtime_error("SkinWareProvider construction failed : " + ret_load.second);
  
    fail_error ret_read = startReader(s_reader_, sensor_layer_);
    if (ret_read.first)
      throw std::runtime_error("SkinWareProvider construction failed : " + ret_load.second);
  
    s_begin_    = sensors_source_->sensors_iter_begin();
    s_end_      = sensors_source_->sensors_iter_end();
    no_sensors_ = helpers::skin::distance(s_begin_, s_end_);
  }

  MeshNatural* impl_createMesh() const
  {
    return new MeshNatural(D, s_begin_, s_end_);
  }

  void impl_update(target_values_type& target_vec) const
  {
    target_vec.resize(no_sensors_ * D);
  
    const int req_res = s_reader_->request(sensor_layer_);
    switch (req_res) {
      case SKIN_BAD_ID :
        throw std::runtime_error("skin_reader::request() returned SKIN_BAD_ID");
      case SKIN_FAIL :
        throw std::runtime_error("skin_reader::request() returned SKIN_FAIL");
      case SKIN_PARTIAL :
        throw std::runtime_error("skin_reader::request() returned SKIN_PARTIAL");
    }
  
    // wrap this in a lambda which would extract .get_response()
    // and pass it on to std::transform?
    std::size_t i = 0;
    std::for_each(s_begin_, s_end_, [&](skin_sensor & s) {
      target_vec[i] = this->converter_(s.get_response());
      ++i;
    });
  }

  SkinAttributes impl_getAttributes() const
  {
    return skin_attributes_;
  }

  fail_error loadSkin(skin_object* so)
  {
    const int obj_load_res = so->load();
    switch (obj_load_res) {
      case SKIN_SUCCESS :
        return std::make_pair(false, "");
      case SKIN_FAIL :
        return std::make_pair(true, "skin_object::load() returned SKIN_FAIL");
      case SKIN_NO_MEM :
        return std::make_pair(true, "skin_object::load() returned SKIN_NO_MEM");
    }
    return std::make_pair(true, "skin_object::load() returned unkown error");
  }
  
  fail_error startReader(skin_reader* sr, skin_sensor_type_id sensor_layer)
  {
    const int reader_start_res = sr->start(sensor_layer, SKIN_ACQUISITION_SPORADIC);
    switch (reader_start_res) {
      case SKIN_SUCCESS :
        return std::make_pair(false, "");
      case SKIN_BAD_ID :
        return std::make_pair(true, "skin_reader::start() returned SKIN_BAD_ID");
      case SKIN_PARTIAL :
        return std::make_pair(true, "skin_reader::start() returned SKIN_PARTIAL");
      case SKIN_FAIL :
        return std::make_pair(true, "skin_reader::start() returned SKIN_FAIL");
    }
    return std::make_pair(true, "skin_reader::start() returned unkown error");
  }
};

#endif /* SKINWAREPROVIDER_HPP */

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

#include "SkinProviderInterface.hpp"

#include "skin_sensor.h"

#include "helpers/skin.hpp"
#include "traits_helpers.hpp"

template <class source, class raw, class conv>
class SkinWareProvider;

template <class source, class raw, class conv>
struct SkinProvider_traits<SkinWareProvider<source, raw, conv>>
{
  typedef typename source::skin_sensor_iterator sensor_iterator;
  typedef raw rawT;
  typedef conv convT;
};

/**
 * \brief   Interface abstracting SkinWare.
 * \tparam  source  where are the sensors coming from? Unfortunately SkinWare's skin/layer/module
 *                  sensor iterators are not inter-operable.
 * \tparam  raw     typw in which the raw readings are given, uint16_t by default
 * \tparam  conv    to which type to convert? double by default
 *
 * Here be dragons. To be cleaned up and covered by unit tests, up to the possible extent.
 */
template <class source, class raw = uint16_t, class conv = double>
class SkinWareProvider : public SkinProviderInterface<SkinWareProvider<source, raw, conv>>
{
friend class SkinProviderInterface<SkinWareProvider<source, raw, conv>>;
  public:

    using typename SkinProviderInterface<SkinWareProvider<source, raw, conv>>::sensor_iterator;
    using typename SkinProviderInterface<SkinWareProvider<source, raw, conv>>::convT;
    using typename SkinProviderInterface<SkinWareProvider<source, raw, conv>>::rawT;
    using typename SkinProviderInterface<SkinWareProvider<source, raw, conv>>::converter_type;

  private:

    const skin_sensor_type_id sensor_layer_;
    skin_object     s_object_;
    skin_reader*    s_reader_;
    source*  sensors_source_;
    // it's not const since we have to do it after the skin initialisation and without extra magic
    // it'd be too hard/tricky to make it const
    std::size_t     no_sensors_; 
    sensor_iterator s_begin_;
    sensor_iterator s_end_;

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
    template <class SS=source, traits_helpers::EnableIf<std::is_same<SS, skin_object>>...>
    SkinWareProvider(converter_type converter)
      :
        SkinProviderInterface<SkinWareProvider<source,raw,conv>>(converter),
        sensor_layer_(SKIN_ALL_SENSOR_TYPES),
        s_object_(),
        s_reader_(s_object_.reader()),
        sensors_source_(&s_object_)
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
    template <class SS=source, traits_helpers::DisableIf<std::is_same<SS, skin_object>>...>
    SkinWareProvider(converter_type converter, source& sensors_source)
      :
        SkinProviderInterface<SkinWareProvider<source,raw,conv>>(converter),
        sensor_layer_(SKIN_ALL_SENSOR_TYPES),
        s_object_(),
        s_reader_(s_object_.reader()),
        sensors_source_(&sensors_source)
    {
      init();
    }

  protected:

    sensor_iterator impl_sensors_begin() { return s_begin_; }
    sensor_iterator impl_sensors_end()   { return s_end_;   } 

    void impl_update(std::vector<convT>& target_vec)
    {
      const int req_res = s_reader_->request(sensor_layer_);
      switch (req_res) {
        case SKIN_BAD_ID :
          throw std::runtime_error("skin_reader::request() returned SKIN_BAD_ID");
        case SKIN_FAIL :
          throw std::runtime_error("skin_reader::request() returned SKIN_FAIL");
        case SKIN_PARTIAL :
          throw std::runtime_error("skin_reader::request() returned SKIN_PARTIAL");
      }

      std::size_t i = 0;
      std::for_each(s_begin_, s_end_, [&](skin_sensor & s) {
        target_vec[i] = this->converter_(s.get_response());
        ++i;
      });
    }

    std::vector<convT> impl_update()
    {
      std::vector<convT> ret(no_sensors_);
      this->update(ret);
      return ret;
    }

  private:

    void init()
    {
      fail_error ret_load = loadSkin();
      if (ret_load.first)
        throw std::runtime_error("SkinWareProvider construction failed : " + ret_load.second);

      fail_error ret_read = startReader();
      if (ret_read.first)
        throw std::runtime_error("SkinWareProvider construction failed : " + ret_load.second);

      s_begin_    = sensors_source_->sensors_iter_begin();
      s_end_      = sensors_source_->sensors_iter_end();
      no_sensors_ = helpers::skin::distance(s_begin_, s_end_);
    }
    /**
     * First - fail (true/false), second - failure text.
     */
    typedef std::pair<bool, std::string> fail_error;

    fail_error loadSkin()
    {
      const int obj_load_res = s_object_.load();
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

    fail_error startReader()
    {
      const int reader_start_res = s_reader_->start(sensor_layer_, SKIN_ACQUISITION_SPORADIC);
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

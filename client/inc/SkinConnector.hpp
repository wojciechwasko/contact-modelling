#ifndef SKINCONNECTOR_HPP
#define SKINCONNECTOR_HPP

#include <cstdint>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>

#include <Eigen/Dense>

#include "skin_sensor.h"

#include "skin_helpers.hpp"
#include "traits_helpers.hpp"

/**
 * \brief   Interface abstracting SkinWare.
 *
 * \note  This class uses Eigen::VectorXd directly, without templating over different possible vector
 *        types. This reduces the flexibility of the code but on the other hand allows to reduce
 *        vector rewriting overhead.
 */
template <class SensorsSource>
class SkinConnector {
  public:
    typedef typename SensorsSource::skin_sensor_iterator sensor_iterator;

  private:
    typedef double (*converter_type)(uint16_t);

    converter_type conv_;
    const skin_sensor_type_id sensor_layer_;
    skin_object     s_object_;
    skin_reader*    s_reader_;
    SensorsSource*  sensors_source_;
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
    template <class SS=SensorsSource, traits_helpers::EnableIf<std::is_same<SS, skin_object>>...>
    SkinConnector(converter_type conv)
      :
        conv_(conv),
        sensor_layer_(SKIN_ALL_SENSOR_TYPES),
        s_object_(),
        s_reader_(s_object_.reader()),
        sensors_source_(&s_object_)
    {
      init();
    }

    /**
     * \brief Takes a part of the skin (SensorsSource) as the source of sensors.
     * \param conv  Fuction pointer; function shall take a uint16_t and return a double in [m].
     *
     * I know function pointers are ugly. But they work for now.
     *
     * In a more elaborate version, we could use sensors coming only from one given module or such.
     * Though we'd probably have to make this class templated and expose the correct iterator type
     * (what a PITA).
     */
    template <class SS=SensorsSource, traits_helpers::DisableIf<std::is_same<SS, skin_object>>...>
    SkinConnector(SensorsSource& sensors_source, converter_type conv)
      :
        conv_(conv),
        sensor_layer_(SKIN_ALL_SENSOR_TYPES),
        s_object_(),
        s_reader_(s_object_.reader()),
        sensors_source_(&sensors_source)
    {
      init();
    }

    sensor_iterator sensors_begin() { return s_begin_; }
    sensor_iterator sensors_end()   { return s_end_;   } 

    void update(Eigen::VectorXd& target_vec)
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
        target_vec[i] = this->conv_(s.get_response());
        ++i;
      });
    }

    Eigen::VectorXd update()
    {
      Eigen::VectorXd ret(no_sensors_);
      update(ret);
      return ret;
    }

  private:

    void init()
    {
      fail_error ret_load = loadSkin();
      if (ret_load.first)
        throw std::runtime_error("SkinConnector construction failed : " + ret_load.second);

      fail_error ret_read = startReader();
      if (ret_read.first)
        throw std::runtime_error("SkinConnector construction failed : " + ret_load.second);

      s_begin_    = sensors_source_->sensors_iter_begin();
      s_end_      = sensors_source_->sensors_iter_end();
      no_sensors_ = skin_helpers::distance(s_begin_, s_end_);
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

#endif /* SKINCONNECTOR_HPP */

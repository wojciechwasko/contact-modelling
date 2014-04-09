#ifndef SKINPROVIDERINTERFACE_HPP
#define SKINPROVIDERINTERFACE_HPP

/**
 * \def COMMA
 * A nifty hack to enable processing of multiple templated classes in macros.
 * Inspired by http://stackoverflow.com/a/19841470/312650
 */
#ifndef COMMA
#define COMMA ,
#endif

template <class derived_t>
struct SkinProvider_traits;

template <class Derived>
class SkinProviderInterface {
  public:
    typedef typename SkinProvider_traits<Derived>::sensor_iterator sensor_iterator;

    sensor_iterator sensors_begin()
    {
      return static_cast<Derived*>(this)->impl_sensors_begin(); 
    }

    sensor_iterator sensors_end() 
    {
      return static_cast<Derived*>(this)->impl_sensors_end(); 
    }

    const sensor_iterator sensors_cbegin() const
    {
      return static_cast<Derived*>(this)->impl_sensors_cbegin(); 
    }

    const sensor_iterator sensors_cend() const 
    {
      return static_cast<Derived*>(this)->impl_sensors_cend(); 
    }

    std::vector<double> update()
    {
      return static_cast<Derived*>(this)->impl_update();
    }

    void update(std::vector<double>& target_vec)
    {
      static_cast<Derived*>(this)->impl_update(target_vec);
    }

  protected:
    SkinProviderInterface()                                         = default;
    SkinProviderInterface& operator=(const SkinProviderInterface&)  = default;
    SkinProviderInterface(const SkinProviderInterface&)             = default;
    SkinProviderInterface& operator=(SkinProviderInterface&&)       = default;
    SkinProviderInterface(SkinProviderInterface&&)                  = default;
    ~SkinProviderInterface()                                        = default;
};


#endif /* SKINPROVIDERINTERFACE_HPP */

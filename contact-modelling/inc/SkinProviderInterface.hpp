#ifndef SKINPROVIDERINTERFACE_HPP
#define SKINPROVIDERINTERFACE_HPP

#include <vector>
#include <cstdint>
#include <functional>

class MeshNatural;

/**
 * \brief   A class which will provide the skin (location of taxels and their response values).
 *
 * \note  Response is taken to be a uint16_t, which later on may be a little bit limiting. So far
 *        we don't consider any skins which don't have a uint16_t response, but this is something to
 *        look out for.
 */
class SkinProviderInterface {
public:
  typedef std::function<double(uint16_t)> converter_type;
  typedef std::vector<double> target_values_type;

  /**
   * Sensor response value dimensionality
   */
  const size_t D;
  
  /**
   * \brief   Return a new Natural Mesh.
   * \param   dim   dimensionality of the Netural Mesh'es values
   * \return  a *bare* pointer to a newly constructed natural Mesh
   *
   * \note  This newly created mesh does *not* have values initialised.
   */
  MeshNatural* createMesh();

  void update(target_values_type& target_vec);

protected:
  // TODO ? probably make this private and expose access through member methods
  converter_type converter_;

  SkinProviderInterface(const size_t dim, converter_type conv);

  SkinProviderInterface& operator=(const SkinProviderInterface&)  = default;
  SkinProviderInterface(const SkinProviderInterface&)             = default;
  SkinProviderInterface& operator=(SkinProviderInterface&&)       = default;
  SkinProviderInterface(SkinProviderInterface&&)                  = default;

public:
  virtual ~SkinProviderInterface()                                = default;

private:
  virtual MeshNatural* impl_createMesh() = 0;
  virtual void impl_update(target_values_type& target_vec) = 0;
};


#endif /* SKINPROVIDERINTERFACE_HPP */

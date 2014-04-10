#ifndef ALGINTERFACE_HPP
#define ALGINTERFACE_HPP

#include <boost/any.hpp>

class MeshInterface;

/**
 * \brief   Common base class for algorithms.
 *
 * This base class ensures a uniform interface is used across the algorithms.
 *
 * \note  Contrary to the (here) widely used idiom of taking ownership of input/output, algorithms
 *        do *not* claim ownership. This is done for a simple reason - two algorithms
 *        (from-displacements-to-forces and from-forces-(back-)to-displacements) would have to own a
 *        common forces instance. Rather, there is an offline phase (->offline()) which returns data
 *        to be stored by the user and later provided for the online (run) phase.
 *
 * \cond  PRIVATE
 * \todo  Could applying the same interface to e.g. Interpolators be beneficial?
 * \endcond
 */
class AlgInterface {
public:
  /**
   * \brief   Perform offline computation
   * \param   input   The "from" for the algorithm
   * \param   output  The "to" for the algorithm
   * \return  Precomputed data used later on for the online phase of the algorithm
   *
   */
  boost::any offline(
    const MeshInterface& input,
    const MeshInterface& output,
    const boost::any& params
  );

  /**
   * \brief   Perform the online phase of the algorithm
   * \param   input   The "from" for the algorithm
   * \param   output  The "to" for the algorithm. Notice the lack of const-ness!
   * \param   precomputed   Precomputed data returned from a previous call to offline()
   */
  void run(
    const MeshInterface& input,
          MeshInterface& output,
    const boost::any& params,
    const boost::any& precomputed
  );

protected:
  AlgInterface()                               = default;
  AlgInterface& operator=(const AlgInterface&) = default;
  AlgInterface(const AlgInterface&)            = default;
  AlgInterface& operator=(AlgInterface&&)      = default;
  AlgInterface(AlgInterface&&)                 = default;

public:
  virtual ~AlgInterface()                      = default;

private:
  /**
   * \brief   To be overriden by implementation (Derived class).
   */
  virtual boost::any impl_offline(
    const MeshInterface&  input,
    const MeshInterface&  output,
    const boost::any& params
  ) = 0;

  /**
   * \brief   To be overriden by implementation (Derived class).
   */
  virtual void impl_run(
    const MeshInterface&  input,
          MeshInterface&  output,
    const boost::any& params,
    const boost::any& precomputed
  ) = 0;
};


#endif /* ALGINTERFACE_HPP */

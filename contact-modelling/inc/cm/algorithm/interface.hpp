#ifndef ALGINTERFACE_HPP
#define ALGINTERFACE_HPP

/**
 * \file
 * \brief   Interface all algorithms implement.
 */

#include <boost/any.hpp>

namespace cm {

class Grid;

/**
 * \brief   Common base class for algorithms.
 *
 * This base class ensures a uniform interface is used across the algorithms.
 *
 * \note  Algorithms do *not* claim ownership. This is done for a simple reason
 *        - two algorithms (from-displacements-to-forces and
 *        from-forces-(back-)to-displacements) would have to own a common forces
 *        instance. Rather, there is an offline phase (->offline()) which
 *        returns data to be stored by the user and later provided for the
 *        online (run) phase.
 *
 * \cond  DEV
 * This class hierarchy follows the non-virtual public interface idiom (see:
 * [GotW Virtuality][gotw_virtuality]), i.e. the actual online/offline phase
 * must be overriden as `impl_offline` and `impl_run`.
 *
 * \todo  Would applying the Algorithm interface to Interpolators be beneficial?
 *        After all, interpolators work on two distinct meshes as well.
 *
 * [gotw_virtuality]: http://www.gotw.ca/publications/mill18.htm
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
    const Grid& input,
    const Grid& output,
    const boost::any& params
  );

  /**
   * \brief   Perform the online phase of the algorithm
   * \param   input   The "from" for the algorithm
   * \param   output  The "to" for the algorithm. Notice the lack of const-ness!
   * \param   precomputed   Precomputed data returned from a previous call to offline()
   */
  void run(
    const Grid& input,
          Grid& output,
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
   * \brief   To be overriden by implementation.
   */
  virtual boost::any impl_offline(
    const Grid&  input,
    const Grid&  output,
    const boost::any& params
  ) = 0;

  /**
   * \brief   To be overriden by implementation.
   */
  virtual void impl_run(
    const Grid&  input,
          Grid&  output,
    const boost::any& params,
    const boost::any& precomputed
  ) = 0;
};

} /* namespace cm */


#endif /* ALGINTERFACE_HPP */

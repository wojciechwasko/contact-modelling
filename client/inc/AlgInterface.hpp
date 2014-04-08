#ifndef ALGINTERFACE_HPP
#define ALGINTERFACE_HPP

#include <type_traits>

#include <boost/any.hpp>

/**
 * \def COMMA
 * A nifty hack to enable processing of multiple templated classes in macros.
 * Inspired by http://stackoverflow.com/a/19841470/312650
 */
#ifndef COMMA
#define COMMA ,
#endif

#define INJECT_ALG_INTERFACE_TYPES(name) \
  using typename AlgInterface< name >::input_type; \
  using typename AlgInterface< name >::output_type; \
  using typename AlgInterface< name >::params_type; \
  using typename AlgInterface< name >::precomputed_type; \
  using typename AlgInterface< name >::input_cref; \
  using typename AlgInterface< name >::output_cref; \
  using typename AlgInterface< name >::params_cref; \
  using typename AlgInterface< name >::precomputed_cref; \
  using typename AlgInterface< name >::output_ref; 

template <class Derived>
struct Alg_traits
{};

/**
 * \brief   Common mixin class for algorithms.
 *
 * This will not be instantiated by user directly, but by the algorithms themselves.  However, it
 * ensures a uniform interface is used across the algorithms.
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
 *
 * \note  Algorithms are designed to be stateless, which is why all the methods here are declared
 *        static. Implementation should follow suit.
 */
template <class Derived>
class AlgInterface {
  public:
    typedef typename Alg_traits<Derived>::input_type          input_type;
    typedef typename Alg_traits<Derived>::output_type         output_type;
    typedef typename Alg_traits<Derived>::params_type         params_type;
    typedef typename Alg_traits<Derived>::precomputed_type    precomputed_type;

    typedef const input_type&        input_cref;
    typedef const output_type&       output_cref;
    typedef const params_type&       params_cref;
    typedef const precomputed_type&  precomputed_cref;

    typedef output_type&       output_ref;
  protected:
    AlgInterface() {};
    /**
     * \brief   To be overriden by implementation (Derived class).
     */
    static precomputed_type impl_offline(
      input_cref  input,
      output_cref output,
      params_cref params
    );

    /**
     * \brief   To be overriden by implementation (Derived class).
     */
    static void impl_run(
      input_cref  input,
      output_ref  output,
      params_cref params,
      precomputed_cref precomputed
    );

  public:
    /**
     * \brief   Perform offline computation
     * \param   input   The "from" for the algorithm
     * \param   output  The "to" for the algorithm
     * \return  Precomputed data used later on for the online phase of the algorithm
     *
     * Although this function the exact type required by online phase, it may just as well be stored
     * into boost::any since there's an interface to run() which accepts boost::any as a parameter
     * and performs the any_cast.
     */
    static precomputed_type offline(
      input_cref  input,
      output_cref output,
      params_cref params
    )
    {
      return Derived::impl_offline(input, output, params);
    }

    /**
     * \brief   Perform the online phase of the algorithm
     * \param   input   The "from" for the algorithm
     * \param   output  The "to" for the algorithm. Notice the lack of const-ness!
     * \param   precomputed   Precomputed data returned from a previous call to offline()
     *
     */
    static void run(
      input_cref  input,
      output_ref  output,
      params_cref params,
      precomputed_cref precomputed
    )
    {
      Derived::impl_run(input, output, params, precomputed); 
    }

    /**
     * \brief   Perform the online phase of the algorithm
     * \param   input   The "from" for the algorithm
     * \param   output  The "to" for the algorithm. Notice the lack of const-ness!
     * \param   precomputed   Precomputed data returned from a previous call to offline(), wrapped
     *                        in boost::any.
     *
     * This method performs boost::any_cast and calls the regular run().
     */
    static void run(
      input_cref  input,
      output_ref  output,
      params_cref params,
      const boost::any&  precomputed
    )
    {
      AlgInterface::run(
        input,
        output,
        params,
        boost::any_cast<precomputed_cref>(precomputed)
      );
    }
};


#endif /* ALGINTERFACE_HPP */

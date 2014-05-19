#ifndef LOG_HPP
#define LOG_HPP

#include <ostream>

/**
 * \file
 * Logging framework for the Contact-Modelling library
 */

#ifndef MAX_LOGGING_LEVEL
/**
 * \brief Maximum (compile-time) logging level
 *
 * This is used for compile-time if-branch removal, should the speed of
 * execution become an issue.
 */
#define MAX_LOGGING_LEVEL DEBUG4
#endif

/**
 * \cond DEV
 * \brief   Helper macro to get into the proper namespace
 */
#define LOG_NS(what) cm::log::what

/**
 * \cond DEV
 * \brief   Helper macro to get logging level into the proper namespace
 */
#define LOG_NSLEVEL(level) LOG_NS(loglvl::level)
/**
 * \endcond
 */

/**
 * \brief   Main helper macro, returns logging 'sink' for given level
 */
#define LOG(level) \
  if (LOG_NSLEVEL(level) > LOG_NSLEVEL(MAX_LOGGING_LEVEL)) ; \
  else \
    if (LOG_NSLEVEL(level) > LOG_NS(Logger)::getReportingLevel()) ; \
    else LOG_NS(Logger)(LOG_NSLEVEL(level))

/**
 * \brief   Used to check if something would be logged at specified level
 */
#define IFLOG(level) \
      if (LOG_NSLEVEL(level) <= LOG_NSLEVEL(MAX_LOGGING_LEVEL) && \
          LOG_NSLEVEL(level) <= LOG_NS(Logger)::getReportingLevel())

/**
 * \brief   Get the stream the logger is outputting to
 * \sa Logger::getOutputStream()
 */
#define LOG_GET_STREAM(stream) LOG_NS(Logger)::getOutputStream();
/**
 * \brief   Get the current reporting level of the logger
 * \sa Logger::getReportingLevel()
 */
#define LOG_GET_LEVEL(level) LOG_NS(Logger)::getReportingLevel();

/**
 * \brief   Set the stream the logger is outputting to
 * \sa Logger::setOutputStream()
 */
#define LOG_SET_STREAM(stream) LOG_NS(Logger)::setOutputStream(stream);

/**
 * \brief   Set the reporting level of the logger
 * \sa Logger::setReportingLevel()
 */
#define LOG_SET_LEVEL(level) LOG_NS(Logger)::setReportingLevel(LOG_NSLEVEL(level));

/**
 * \brief   Enable terminating each log message with a newline
 * \sa Logger::enableNewline()
 */
#define LOG_ENABLE_NEWLINE() LOG_NS(Logger)::enableNewline();

/**
 * \brief   Disable terminating each log message with a newline
 * \sa Logger::disableNewline()
 */
#define LOG_DISABLE_NEWLINE() LOG_NS(Logger)::disableNewline();

/**
 * \brief   Enable flushing the output stream after each message completes
 * \sa Logger::enableFlush()
 */
#define LOG_ENABLE_FLUSH() LOG_NS(Logger)::enableFlush();

/**
 * \brief   Disable flushing the output stream after each message completes
 * \sa Logger::disableFlush()
 */
#define LOG_DISABLE_FLUSH() LOG_NS(Logger)::disableFlush();

namespace cm {

/**
 * \brief Logging utilities
 */
namespace log {

/**
 * \brief   Empty sink, something like `/dev/null` on Linux
 *
 * The magic behind it is simply that the onull stream is initiated with a null
 * pointer. Lenghty explanation and justification follows, thanks to
 * http://stackoverflow.com/a/6240980/312650
 *
 * > The standard guarantees this works, beginning from 27.6.2.2
 * > [lib.ostream.cons] p1 which describes the constructor of ostream
 * > that takes a pointer to a streambuf:
 * >> Effects: Constructs an object of class basic_ostream, assigning
 * >> initial values to the base class by calling
 * >> basic_ios<charT,traits>::init(sb).
 * > The relevant function from basic_ios, 27.4.4.1 [lib.basic.ios.cons]
 * > p3:
 * >> void init(basic_streambuf<charT,traits>* sb);
 * >> Postconditions: The postconditions of this function are indicated in
 * >> Table 89
 * > The important row from Table 89:
 * >> rdstate() -- goodbit if sb is not a null pointer, otherwise badbit.
 * > What happens if the badbit is set is described under 27.6.2.6
 * > [lib.ostream.unformatted]:
 * >> Each unformatted output function begins execution by constructing an
 * >> object of class sentry. If this object returns true, while
 * >> converting to a value of type bool, the function endeavors to
 * >> generate the requested output.
 * > This implies that, in case the sentry is false, it does not. Here is
 * > how the sentry converts to bool, taken from 27.6.2.3
 * > [lib.ostream::sentry] p3 & p5:
 * >> 3) If, after any preparation is completed, os.good() is true, ok_ ==
 * >> true otherwise, ok_ == false.
 * >> 5) operator bool();
 * >> Effects: Returns ok_.
 * > (ok_ is a member of ostream::sentry of type bool.)
 */
extern std::ostream onull;

/**
 * \brief   Logging level
 */
enum class loglvl {
  ERROR,
  WARN,
  INFO,
  DEBUG,
  DEBUG1,
  DEBUG2,
  DEBUG3,
  DEBUG4
};

/**
 * \brief   Human-readable output of the logging level
 */
std::ostream& operator<<(std::ostream&, const loglvl&);

/**
 * \brief   Workhorse of the logging facilities.
 */
class Logger {
public:
  Logger(loglvl lvl);
  /**
   * \brief   Destructor; may append newline to and/or flush the stream
   */
  ~Logger();

  Logger& operator=(const Logger&)  = delete;
  Logger(const Logger&)             = delete;
  Logger& operator=(Logger&&)       = delete;
  Logger(Logger&&)                  = delete;

  static loglvl getReportingLevel();
  static std::ostream& getOutputStream();
  static void setReportingLevel(loglvl);
  static void setOutputStream(std::ostream&);
  static void enableNewline();
  static void disableNewline();
  static void enableFlush();
  static void disableFlush();

  template <typename T>
  std::ostream& operator<<(T v)
  {
    return *os << curr_lvl << ": " << v;
  }

private:
  /**
   * Logging level used in this instance.
   */
  loglvl  curr_lvl;

  /**
   * Maximum level that will be reported. Anything below that will be redirected to a null
   * stream.
   *
   * \note  Initially set to loglvl::INFO
   * \note  You can alter this via the LOG_SET_LEVEL macro.
   */
  static loglvl   max_lvl;

  /**
   * Destination stream
   *
   * \note  Initially set to std::clog
   * \note  You can alter this via the LOG_SET_STREAM macro.
   */
  static std::ostream* os;

  /**
   * Whether to append newline when the stream is to be closed.
   *
   * \note  Initially set to true.
   * \note  You can alter this via the LOG_DISABLE_NEWLINE or LOG_ENABLE_NEWLINE macro
   */
  static bool do_newline;

  /**
   * Whether to flush the stream when the stream is to be closed.
   *
   * \note  Initially set to true.
   * \note  You can alter this via the LOG_DISABLE_FLUSH or LOG_ENABLE_FLUSH macro
   */
  static bool do_flush;
};

} /* namespace log */
} /* namespace cm */

#endif /* LOG_HPP */

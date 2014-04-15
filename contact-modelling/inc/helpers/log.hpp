#ifndef LOG_HPP
#define LOG_HPP

#include <ostream>

#ifndef MAX_LOGGING_LEVEL
#define MAX_LOGGING_LEVEL DEBUG4
#endif

#define LOG_NS(what) helpers::log::what
#define LOG_NSLEVEL(level) LOG_NS(loglvl::level)

#define LOG(level) \
  if (LOG_NSLEVEL(level) > LOG_NSLEVEL(MAX_LOGGING_LEVEL)) ; \
  else \
    if (LOG_NSLEVEL(level) > LOG_NS(Logger)::getReportingLevel()) ; \
    else LOG_NS(Logger)(LOG_NSLEVEL(level))

#define IFLOG(level) \
      if (LOG_NSLEVEL(level) <= LOG_NSLEVEL(MAX_LOGGING_LEVEL) && \
          LOG_NSLEVEL(level) <= LOG_NS(Logger)::getReportingLevel())

#define LOG_SET_STREAM(stream) LOG_NS(Logger)::setOutputStream(stream);
#define LOG_SET_LEVEL(level) LOG_NS(Logger)::setReportingLevel(LOG_NSLEVEL(level));

#define LOG_ENABLE_NEWLINE() LOG_NS(Logger)::enableNewline();
#define LOG_DISABLE_NEWLINE() LOG_NS(Logger)::disableNewline();

#define LOG_ENABLE_FLUSH() LOG_NS(Logger)::enableFlush();
#define LOG_DISABLE_FLUSH() LOG_NS(Logger)::disableFlush();

namespace helpers {
  namespace log {
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

    std::ostream& operator<<(std::ostream&, const loglvl&);

    class Logger {
    public:
      Logger(loglvl lvl);
      ~Logger();

      Logger& operator=(const Logger&)  = delete;
      Logger(const Logger&)             = delete;
      Logger& operator=(Logger&&)       = delete;
      Logger(Logger&&)                  = delete;

      static loglvl getReportingLevel();
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
  }
}

#endif /* LOG_HPP */

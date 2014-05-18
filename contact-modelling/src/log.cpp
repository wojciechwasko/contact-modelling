#include "cm/log/log.hpp"

#include <iostream>

namespace cm {
namespace log {

std::ostream& operator<<(std::ostream& stream, const loglvl& lvl)
{
  switch (lvl) {
    case loglvl::ERROR:
      stream << "ERROR";
      break;
    case loglvl::WARN:
      stream << "WARN";
      break;
    case loglvl::INFO:
      stream << "INFO";
      break;
    case loglvl::DEBUG:
      stream << "DEBUG";
      break;
    case loglvl::DEBUG1:
      stream << "DEBUG1";
      break;
    case loglvl::DEBUG2:
      stream << "DEBUG2";
      break;
    case loglvl::DEBUG3:
      stream << "DEBUG3";
      break;
    case loglvl::DEBUG4:
      stream << "DEBUG4";
      break;
    default:
      stream << "UNKOWN";
  }
  return stream;
}

std::ostream onull(0); 
std::ostream* Logger::os  = &std::clog;
loglvl Logger::max_lvl    = loglvl::INFO;
bool Logger::do_newline   = true;
bool Logger::do_flush     = true;

Logger::Logger(loglvl lvl)
  : curr_lvl(lvl)
{

}

Logger::~Logger()
{
  if (do_newline) {
    *os << std::endl;
  }
  if (do_flush) {
    *os << std::flush;
  }
}

loglvl Logger::getReportingLevel()
{
  return Logger::max_lvl;  
}

void Logger::setReportingLevel(loglvl lvl)
{
  Logger::max_lvl = lvl;
}

void Logger::setOutputStream(std::ostream& rhs)
{
  Logger::os = &rhs;
}

std::ostream& Logger::getOutputStream()
{
  return *os;
}

void Logger::enableNewline()
{
  Logger::do_newline = true;
}

void Logger::disableNewline()
{
  Logger::do_newline = false;
}

void Logger::enableFlush()
{
  Logger::do_flush = true;
}

void Logger::disableFlush()
{
  Logger::do_flush = false;
}

} /* namespace log */
} /* namespace cm */

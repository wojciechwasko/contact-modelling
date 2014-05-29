#include <boost/test/unit_test.hpp>

#include "cm/details/exception.hpp"

class MyException
{
public:
  const char* what() { return "My awesome exception!"; }
};

BOOST_AUTO_TEST_SUITE(ExceptionGuardTest)

BOOST_AUTO_TEST_CASE(disabled)
{
  cm::details::ExceptionGuard<MyException> guard({}); 
  guard.disable();
  BOOST_CHECK_NO_THROW(guard.check());
};

BOOST_AUTO_TEST_CASE(enabled)
{
  auto always_true = [](const MyException& e) { return true; };
  cm::details::ExceptionGuard<MyException> guard({}); 
  BOOST_CHECK_EXCEPTION(guard.check(), MyException, always_true);
};

BOOST_AUTO_TEST_CASE(disabled_through_make)
{
  auto guard = cm::details::make_ExceptionGuard(MyException()); 
  guard.disable();
  BOOST_CHECK_NO_THROW(guard.check());
};

BOOST_AUTO_TEST_CASE(enabled_through_make)
{
  auto always_true = [](const MyException& e) { return true; };
  auto guard = cm::details::make_ExceptionGuard(MyException()); 
  BOOST_CHECK_EXCEPTION(guard.check(), MyException, always_true);
};

BOOST_AUTO_TEST_SUITE_END()

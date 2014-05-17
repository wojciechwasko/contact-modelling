#define BOOST_TEST_MODULE erase_by_indices
#include <boost/test/unit_test.hpp>

#include "cm/details/container_algorithms.hpp"

BOOST_AUTO_TEST_CASE(test_simple_erasure)
{
  using cm::details::erase_by_indices;
  std::vector<int> original = {0,1,2,3,4,5,6,7,8,9};
  std::vector<size_t> del_i = {  1,  3,  5,6,7,  9};
  std::vector<int> corr_res = {0,  2,  4,      8  };
  
  erase_by_indices(original, del_i);
  BOOST_CHECK_EQUAL_COLLECTIONS(
    original.begin(), original.end(),
    corr_res.begin(), corr_res.end()
  );
}

BOOST_AUTO_TEST_CASE(test_erasure_beyond)
{
  using cm::details::erase_by_indices;
  std::vector<int> original = {0,1,2,3,4,5,6,7,8,9};
  std::vector<size_t> del_i = {9999};
  std::vector<int> corr_res = {0,1,2,3,4,5,6,7,8,9};
  
  erase_by_indices(original, del_i);
  BOOST_CHECK_EQUAL_COLLECTIONS(
    original.begin(), original.end(),
    corr_res.begin(), corr_res.end()
  );
}

BOOST_AUTO_TEST_CASE(test_erasure_empty)
{
  using cm::details::erase_by_indices;
  std::vector<int> original = {0,1,2,3,4,5,6,7,8,9};
  std::vector<size_t> del_i = {};
  std::vector<int> corr_res = {0,1,2,3,4,5,6,7,8,9};
  
  erase_by_indices(original, del_i);
  BOOST_CHECK_EQUAL_COLLECTIONS(
    original.begin(), original.end(),
    corr_res.begin(), corr_res.end()
  );
}

BOOST_AUTO_TEST_CASE(test_erasure_strided)
{
  using cm::details::erase_by_indices;
  std::vector<int> original = {0,0,0,1,1,1,2,2,2};
  std::vector<size_t> del_i = {1};
  std::vector<int> corr_res = {0,0,0,      2,2,2};
  
  erase_by_indices(original, del_i, 3);
  BOOST_CHECK_EQUAL_COLLECTIONS(
    original.begin(), original.end(),
    corr_res.begin(), corr_res.end()
  );
}

BOOST_AUTO_TEST_CASE(test_erasure_strided_beyond)
{
  using cm::details::erase_by_indices;
  std::vector<int> original = {0,0,0,1,1,1,2,2,2};
  std::vector<size_t> del_i = {999};
  std::vector<int> corr_res = {0,0,0,1,1,1,2,2,2};
  
  erase_by_indices(original, del_i, 3);
  BOOST_CHECK_EQUAL_COLLECTIONS(
    original.begin(), original.end(),
    corr_res.begin(), corr_res.end()
  );
}

BOOST_AUTO_TEST_CASE(test_erasure_strided_empty)
{
  using cm::details::erase_by_indices;
  std::vector<int> original = {0,0,0,1,1,1,2,2,2};
  std::vector<size_t> del_i = {};
  std::vector<int> corr_res = {0,0,0,1,1,1,2,2,2};
  
  erase_by_indices(original, del_i, 3);
  BOOST_CHECK_EQUAL_COLLECTIONS(
    original.begin(), original.end(),
    corr_res.begin(), corr_res.end()
  );
}

BOOST_AUTO_TEST_CASE(test_erasure_stride_larger_than_container)
{
  using cm::details::erase_by_indices;
  std::vector<int> original = {0,0,0,1,1,1,2,2,2};
  std::vector<size_t> del_i = {1};
  std::vector<int> corr_res = {0,0,0,1,1,1,2,2,2};
  
  erase_by_indices(original, del_i, 50);
  BOOST_CHECK_EQUAL_COLLECTIONS(
    original.begin(), original.end(),
    corr_res.begin(), corr_res.end()
  );
}

BOOST_AUTO_TEST_CASE(test_erasure_stride_larger_than_container_2)
{
  using cm::details::erase_by_indices;
  std::vector<int> original = {0,0,0,1,1,1,2,2,2};
  std::vector<size_t> del_i = {0};
  std::vector<int> corr_res = {                 };
  
  erase_by_indices(original, del_i, 50);
  BOOST_CHECK_EQUAL_COLLECTIONS(
    original.begin(), original.end(),
    corr_res.begin(), corr_res.end()
  );
}

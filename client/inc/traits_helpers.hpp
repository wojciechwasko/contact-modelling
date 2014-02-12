#ifndef TRAITS_HELPERS_HPP
#define TRAITS_HELPERS_HPP

#include <type_traits>

namespace traits_helpers {
  namespace you_cant_type_this_by_accident {
    enum class enabler {};
  }
  enum class none_placeholder {};

  // --- all
  template<typename... Conditions> struct all;
  template <typename Condition>
  struct all<Condition> {
    static const bool value = Condition::value;
  };
  template <typename Condition_1, typename... Conditions>
  struct all<Condition_1, Conditions...> {
    static const bool value = Condition_1::value && all<Conditions...>::value;
  };
  
  // --- any
  template<typename... Conditions> struct any;
  template <typename Condition>
  struct any<Condition> {
    static const bool value = Condition::value;
  };
  template <typename Condition_1, typename... Conditions>
  struct any<Condition_1, Conditions...> {
    static const bool value = (Condition_1::value) ? true : any<Conditions...>::value;
  };
  
  template <typename... Conditions>
  using EnableIfAll  = typename std::enable_if<all<Conditions...>::value, you_cant_type_this_by_accident::enabler>::type;
  
  template <typename... Conditions>
  using DisableIfAll = typename std::enable_if<!all<Conditions...>::value, you_cant_type_this_by_accident::enabler>::type;
  
  template <typename... Conditions>
  using EnableIfAny  = typename std::enable_if<any<Conditions...>::value, you_cant_type_this_by_accident::enabler>::type;
  
  template <typename... Conditions>
  using DisableIfAny = typename std::enable_if<!any<Conditions...>::value, you_cant_type_this_by_accident::enabler>::type;

  template <typename Condition>
  using EnableIf = typename std::enable_if<Condition::value, you_cant_type_this_by_accident::enabler>::type;

  template <typename Condition>
  using DisableIf = typename std::enable_if<!(Condition::value), you_cant_type_this_by_accident::enabler>::type;
}



#endif /* TRAITS_HELPERS_HPP */

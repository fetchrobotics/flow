/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 *
 * @brief Defines a utility similar to <code>std::apply</code> (C++14)
 *
 *        For reference, see
 *        <a href="http://en.cppreference.com/w/cpp/utility/apply">here</a>
 */
#ifndef FLOW_UTILITY_APPLY_HPP
#define FLOW_UTILITY_APPLY_HPP

// C++ Standard Library
#include <cstdint>
#include <tuple>
#include <utility>

// Flow
#include <flow/utility/integer_sequence.hpp>

namespace flow
{
#ifndef DOXYGEN_SKIP
namespace detail
{

/**
 * @brief Implementation for flow::apply
 */
template <typename ReturnT, typename UnaryInvocableT, typename ArgTupleT, std::size_t... IntPack>
constexpr typename std::enable_if<std::is_void<ReturnT>::value>::type
apply(UnaryInvocableT&& fn, ArgTupleT&& targs, index_sequence<IntPack...>)
{
  fn(std::get<IntPack>(std::forward<ArgTupleT>(targs))...);
}

/**
 * @copydoc flow::detail::apply
 * @note Participates in overload resolution when \p ReturnT is non-<code>void</code>
 * @return return value of <code>fn</code> when passed arguments from <code>targs</code>
 */
template <typename ReturnT, typename UnaryInvocableT, typename ArgTupleT, std::size_t... IntPack>
constexpr typename std::enable_if<!std::is_void<ReturnT>::value, ReturnT>::type
apply(UnaryInvocableT&& fn, ArgTupleT&& targs, index_sequence<IntPack...>)
{
  return fn(std::get<IntPack>(std::forward<ArgTupleT>(std::forward<ArgTupleT>(targs)))...);
}

/**
 * @brief Implementation for flow::apply_every
 */
template <typename UnaryInvocableT, typename ArgTupleT, std::size_t... IntPack>
constexpr void apply_every(UnaryInvocableT&& fn, ArgTupleT&& targs, index_sequence<IntPack...>)
{
  std::initializer_list<int>{0, (fn(std::get<IntPack>(std::forward<ArgTupleT>(targs))), 0)...};
}

/**
 * @brief Implementation for flow::apply_every
 */
template <typename BinaryInvocableT, typename Arg1TupleT, typename Arg2TupleT, std::size_t... IntPack>
constexpr void apply_every(BinaryInvocableT&& fn, Arg1TupleT&& targs1, Arg2TupleT&& targs2, index_sequence<IntPack...>)
{
  std::initializer_list<int>{
    0,
    (fn(std::get<IntPack>(std::forward<Arg1TupleT>(targs1)), std::get<IntPack>(std::forward<Arg2TupleT>(targs2))),
     0)...};
}


/**
 * @brief Implementation for flow::apply_each
 */
template <typename UnaryInvocableTupleT, typename ArgTupleT, std::size_t... IntPack>
constexpr void apply_each(UnaryInvocableTupleT&& fns, ArgTupleT&& targs, index_sequence<IntPack...>)
{
  std::initializer_list<int>{0, (std::get<IntPack>(fns)(std::get<IntPack>(targs)), 0)...};
}

}  // namespace detail
#endif  // DOXYGEN_SKIP

/**
 * @brief Calls function with with tuple (e.g <code>std::tuple</code>) as argument list
 *
 * @tparam ReturnT     function return type
 * @tparam UnaryInvocableT  (deduced) invocable type:
 *                     - function pointer
 *                     - functor object with <code>Object::operator()</code>
 *                     - lambda function expression
 * @tparam TupleT  (deduced) tuple-like type with element types order according to function argument list
 *
 * @param fn     function pointer or functor object type
 * @param targs  tuple with arguments to pass to <code>f</code>
 */
template <typename ReturnT, typename UnaryInvocableT, typename ArgTupleT>
constexpr typename std::enable_if<std::is_void<ReturnT>::value>::type apply(UnaryInvocableT&& fn, ArgTupleT&& targs)
{
  constexpr auto N = std::tuple_size<typename std::remove_reference<ArgTupleT>::type>::value;
  detail::apply<ReturnT>(std::forward<UnaryInvocableT>(fn), std::forward<ArgTupleT>(targs), make_index_sequence<N>{});
}


/**
 * @copydoc flow::apply
 * @return return value of <code>fn</code> when passed arguments from <code>targs</code>
 * @note Participates in overload resolution when \p ReturnT is non-<code>void</code>
 */
template <typename ReturnT, typename UnaryInvocableT, typename ArgTupleT>
constexpr typename std::enable_if<!std::is_void<ReturnT>::value, ReturnT>::type
apply(UnaryInvocableT&& fn, ArgTupleT&& targs)
{
  constexpr auto N = std::tuple_size<typename std::remove_reference<ArgTupleT>::type>::value;
  return detail::apply<ReturnT>(
    std::forward<UnaryInvocableT>(fn), std::forward<ArgTupleT>(targs), make_index_sequence<N>{});
}


/**
 * @brief Calls function on every element of a tuple (e.g <code>std::tuple</code>)
 *
 * @tparam UnaryInvocableT  (deduced) unary invocable type:
 *                              - function pointer
 *                              - functor object with <code>Object::operator()</code>
 *                              - lambda function expression
 * @tparam ArgTupleT  (deduced) tuple-like type with element types order according to function argument list
 *
 * @param fn     function pointer or functor object type
 * @param targs  tuple with arguments to pass to <code>f</code>
 */
template <typename UnaryInvocableT, typename ArgTupleT>
constexpr void apply_every(UnaryInvocableT&& fn, ArgTupleT&& targs)
{
  constexpr auto N = std::tuple_size<typename std::remove_reference<ArgTupleT>::type>::value;
  detail::apply_every(std::forward<UnaryInvocableT>(fn), std::forward<ArgTupleT>(targs), make_index_sequence<N>{});
}


/**
 * @brief Calls a binary function on each element of two tuples (e.g <code>std::tuple</code>)
 *
 * @tparam BinaryInvocableT  (deduced) binary invocable type:
 *                              - function pointer
 *                              - functor object with <code>Object::operator()</code>
 *                              - lambda function expression
 * @tparam Arg1TupleT  (deduced) tuple-like type with element types order according to function argument list
 * @tparam Arg2TupleT  (deduced) tuple-like type with element types order according to function argument list
 *
 * @param fn    function pointer or functor object
 * @param targs  tuple with arguments to pass to <code>f</code>
 */
template <typename BinaryInvocableT, typename Arg1TupleT, typename Arg2TupleT>
constexpr void apply_every(BinaryInvocableT&& fn, Arg1TupleT&& targs1, Arg2TupleT&& targs2)
{
  constexpr auto N = std::tuple_size<typename std::remove_reference<Arg1TupleT>::type>::value;
  detail::apply_every(
    std::forward<BinaryInvocableT>(fn),
    std::forward<Arg1TupleT>(targs1),
    std::forward<Arg2TupleT>(targs2),
    make_index_sequence<N>{});
}


/**
 * @brief Calls a unique function on each element of a tuple (e.g <code>std::tuple</code>)
 *
 * @tparam UnaryInvocableTupleT  (deduced) binary invocable type:
 *                              - function pointer
 *                              - functor object with <code>Object::operator()</code>
 *                              - lambda function expression
 * @tparam ArgTupleT  (deduced) tuple-like type with element types order according to function argument list
 *
 * @param fns    tuple of function pointers or functor objects
 * @param targs  tuple with arguments to pass to <code>f</code>
 */
template <typename UnaryInvocableTupleT, typename ArgTupleT>
constexpr void apply_each(UnaryInvocableTupleT&& fns, ArgTupleT&& targs)
{
  constexpr auto N = std::tuple_size<typename std::remove_reference<ArgTupleT>::type>::value;
  detail::apply_each(std::forward<UnaryInvocableTupleT>(fns), std::forward<ArgTupleT>(targs), make_index_sequence<N>{});
}

}  // namespace flow

#endif  // FLOW_UTILITY_APPLY_HPP

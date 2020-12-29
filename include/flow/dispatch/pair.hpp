/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 *
 * @file pair.h
 */
#ifndef FLOW_CAPTOR_DISPATCH_PAIR_H
#define FLOW_CAPTOR_DISPATCH_PAIR_H

// C++ Standard Library
#include <utility>

// Flow
#include <flow/dispatch.hpp>

namespace flow
{

/**
 * @copydoc DispatchTraits
 *
 * @note partial specialization for <code>::std::pair</code>
 */
template <typename StampT, typename ValueT> struct DispatchTraits<::std::pair<StampT, ValueT>>
{
  /// Dispatch stamp type
  using stamp_type = StampT;

  /// Dispatch data type
  using value_type = ValueT;
};


/**
 * @copydoc DispatchAccess
 *
 * @note partial specialization for <code>::std::pair</code>
 */
template <typename StampT, typename ValueT> struct DispatchAccess<::std::pair<StampT, ValueT>>
{
  /// Selects type of lesser size to use when returning values
  template <typename T> using return_t = std::conditional_t<(sizeof(T) <= sizeof(T&)), T, const T&>;

  static constexpr return_t<StampT> stamp(const ::std::pair<StampT, ValueT>& dispatch) { return dispatch.first; }

  static constexpr return_t<ValueT> value(const ::std::pair<StampT, ValueT>& dispatch) { return dispatch.second; }
};

}  // namespace flow

#endif  // FLOW_CAPTOR_DISPATCH_PAIR_H

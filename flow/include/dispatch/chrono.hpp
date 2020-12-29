/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 *
 * @file dispatch.h
 */
#ifndef FLOW_CAPTOR_DISPATCH_CHRONO_H
#define FLOW_CAPTOR_DISPATCH_CHRONO_H

// C++ Standard Library
#include <chrono>

// Flow
#include <flow/dispatch.hpp>

namespace flow
{

/**
 * @brief Helper struct used to specify stamp attributes for <chrono> time types
 *
 * @tparam ClockT     clock on which this time point is measured
 * @tparam DurationT  <code>std::chrono::duration</code> type used to measure the time since epoch
 */
template <typename ClockT, typename DurationT> struct StampTraits<std::chrono::time_point<ClockT, DurationT>>
{
  /// Stamp type
  using stamp_type = std::chrono::time_point<ClockT, DurationT>;

  /// Associated duration/offset type
  using offset_type = DurationT;

  /// Returns minimum stamp value
  static constexpr stamp_type min() { return stamp_type::min(); };

  /// Returns maximum stamp value
  static constexpr stamp_type max() { return stamp_type::max(); };
};

}  // namespace flow

#endif  // FLOW_CAPTOR_DISPATCH_CHRONO_H

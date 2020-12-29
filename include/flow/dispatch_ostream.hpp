/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 */
#ifndef FLOW_CAPTOR_DISPATCH_OSTREAM_HPP
#define FLOW_CAPTOR_DISPATCH_OSTREAM_HPP

// C++ Standard Library
#include <ostream>

// Flow
#include <flow/dispatch.hpp>

namespace flow
{

/**
 * @brief Output stream overload for <code>Dispatch</code>
 * @param[in,out] os  output stream
 * @param dispatch  dispatch object
 * @return os
 */
template <typename StampT, typename ValueT>
inline std::ostream& operator<<(std::ostream& os, const Dispatch<StampT, ValueT>& dispatch)
{
  return os << "stamp: " << dispatch.stamp << "\nvalue: " << dispatch.value;
}


/**
 * @brief Output stream overload for <code>CaptureRange</code>
 *
 * @param[in,out] os  output stream
 * @param range  capture stamp range
 * @return os
 */
template <typename StampT> inline std::ostream& operator<<(std::ostream& os, const CaptureRange<StampT>& range)
{
  return os << "lower_stamp: " << range.lower_stamp << ", upper_stamp: " << range.upper_stamp;
}

}  // namespace flow

#endif  // FLOW_CAPTOR_DISPATCH_OSTREAM_HPP

/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 *
 * @warning IMPLEMENTATION ONLY: THIS FILE SHOULD NEVER BE INCLUDED DIRECTLY!
 */
#ifndef FLOW_SRC_FOLLOWER_COUNT_BEFORE_HPP
#define FLOW_SRC_FOLLOWER_COUNT_BEFORE_HPP

// C++ Standard Library
#include <algorithm>
#include <cstdint>
#include <stdexcept>

namespace flow
{
namespace follower
{

template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
CountBefore<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::CountBefore(
  const size_type count,
  const offset_type& delay,
  const ContainerT& container,
  const QueueMonitorT& queue_monitor) :
    PolicyType{container, queue_monitor},
    count_{count},
    delay_{delay}
{
  if (count_ == 0)
  {
    throw std::invalid_argument{"'count' cannot be 0"};
  }
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
template <typename OutputDispatchIteratorT>
State CountBefore<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::capture_follower_impl(
  OutputDispatchIteratorT output,
  const CaptureRange<stamp_type>& range)
{
  const State state = this->dry_capture_follower_impl(range);

  if (state == State::PRIMED)
  {
    // When primed, next n-elements should be captured without removal
    std::copy_n(PolicyType::queue_.begin(), count_, output);
  }

  return state;
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
State CountBefore<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::dry_capture_follower_impl(
  const CaptureRange<stamp_type>& range)
{
  // Retry if queue has no data
  if (PolicyType::queue_.empty())
  {
    return State::RETRY;
  }

  // The boundary before which messages are valid and after which they are not. Non-inclusive.
  const stamp_type boundary = range.upper_stamp - delay_;

  // Scroll to element boundary
  size_type before_boundary_count = 0;
  auto itr = PolicyType::queue_.begin();
  while (itr != PolicyType::queue_.end() and boundary > get_stamp(*itr))
  {
    ++before_boundary_count;
    ++itr;
  }

  // Count elements after boundary
  const bool at_or_after_boundary_count = itr != PolicyType::queue_.end();

  if (before_boundary_count >= count_)
  {
    const auto first_itr = std::prev(itr, count_);
    PolicyType::queue_.remove_before(get_stamp(*first_itr));
    return State::PRIMED;
  }
  else if (at_or_after_boundary_count)
  {
    return State::ABORT;
  }
  else
  {
    return State::RETRY;
  }
}

}  // namespace follower
}  // namespace flow

#endif  // FLOW_SRC_FOLLOWER_COUNT_BEFORE_HPP

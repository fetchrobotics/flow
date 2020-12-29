/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 *
 * @warning IMPLEMENTATION ONLY: THIS FILE SHOULD NEVER BE INCLUDED DIRECTLY!
 */
#ifndef FLOW_IMPL_FOLLOWER_RANGED_HPP
#define FLOW_IMPL_FOLLOWER_RANGED_HPP

// C++ Standard Library
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace flow
{
namespace follower
{

template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
Ranged<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::Ranged(
  const offset_type& delay,
  const ContainerT& container,
  const QueueMonitorT& queue_monitor) :
    PolicyType{container, queue_monitor},
    delay_{delay}
{}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
template <typename OutputDispatchIteratorT>
State Ranged<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::capture_follower_impl(
  OutputDispatchIteratorT&& output,
  const CaptureRange<stamp_type>& range)
{
  // Abort early on empty queue
  if (PolicyType::queue_.empty())
  {
    return State::RETRY;
  }

  // Get iterator to element after first in capture sequence
  const auto after_first_itr = this->find_after_first(range);

  // If we are at the start of the available range, then all elements after this one will be after the valid range
  if (after_first_itr == PolicyType::queue_.begin())
  {
    return State::ABORT;
  }

  // Find initial end position iterator
  const auto before_last_itr = this->find_before_last(range, after_first_itr);

  // If the end of our range
  if (before_last_itr == PolicyType::queue_.end())
  {
    return State::RETRY;
  }

  // Get capture iterator range
  const auto first_itr = std::prev(after_first_itr);
  const auto last_itr = std::next(before_last_itr);

  // Copy captured data over range
  std::copy(first_itr, last_itr, std::forward<OutputDispatchIteratorT>(output));

  // Remove data before first captured element
  PolicyType::queue_.remove_before(get_stamp(*first_itr));

  return State::PRIMED;
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
State Ranged<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::dry_capture_follower_impl(
  const CaptureRange<stamp_type>& range)
{
  // Abort early on empty queue
  if (PolicyType::queue_.empty())
  {
    return State::RETRY;
  }

  // Get iterator to element after first in capture sequence
  const auto after_first_itr = this->find_after_first(range);

  // If we are at the start of the available range, then all elements after this one will be after the valid range
  if (after_first_itr == PolicyType::queue_.begin())
  {
    return State::ABORT;
  }
  // If the end of our range
  else if (this->find_before_last(range, after_first_itr) == PolicyType::queue_.end())
  {
    return State::RETRY;
  }
  else
  {
    // Remove data before first captured element
    PolicyType::queue_.remove_before(get_stamp(*std::prev(after_first_itr)));
    return State::PRIMED;
  }
}

template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
auto Ranged<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::find_after_first(
  const CaptureRange<stamp_type>& range) const
{
  return std::find_if(
    PolicyType::queue_.begin(),
    PolicyType::queue_.end(),
    [offset_lower_stamp = (range.lower_stamp - delay_)](const DispatchT& dispatch) {
      return get_stamp(dispatch) >= offset_lower_stamp;
    });
}

template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
template <typename QueueIteratorT>
auto Ranged<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::find_before_last(
  const CaptureRange<stamp_type>& range,
  const QueueIteratorT after_first) const
{
  return std::find_if_not(
    after_first == PolicyType::queue_.end() ? PolicyType::queue_.begin() : after_first,
    PolicyType::queue_.end(),
    [offset_upper_stamp = (range.upper_stamp - delay_)](const DispatchT& dispatch) {
      return get_stamp(dispatch) <= offset_upper_stamp;
    });
}

template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
void Ranged<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::abort_follower_impl(const stamp_type& t_abort)
{}

}  // namespace follower
}  // namespace flow

#endif  // FLOW_IMPL_FOLLOWER_RANGED_HPP

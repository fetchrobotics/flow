/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl, Derek King
 *
 * @warning IMPLEMENTATION ONLY: THIS FILE SHOULD NEVER BE INCLUDED DIRECTLY!
 */
#ifndef FLOW_SRC_FOLLOWER_LATCHED_HPP
#define FLOW_SRC_FOLLOWER_LATCHED_HPP

// C++ Standard Library
#include <chrono>

namespace flow
{
namespace follower
{

template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
Latched<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::Latched(
  const offset_type min_period,
  const ContainerT& container,
  const QueueMonitorT& queue_monitor) :
    PolicyType{container, queue_monitor},
    min_period_{min_period}
{}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
template <typename OutputDispatchIteratorT>
State Latched<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::capture_follower_impl(
  OutputDispatchIteratorT output,
  const CaptureRange<stamp_type>& range)
{
  const State state = this->dry_capture_follower_impl(range);

  if (state == State::PRIMED)
  {
    // When primed, latched element should be captured
    *(output++) = *latched_;
  }

  return state;
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
State Latched<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::dry_capture_follower_impl(
  const CaptureRange<stamp_type>& range)
{
  if (PolicyType::queue_.empty())
  {
    if (latched_)
    {
      // If we have latched data, return that and report ready state
      return State::PRIMED;
    }
    else
    {
      // Retry if queue has no data and there is no latched data
      return State::RETRY;
    }
  }

  // The boundary before which data is are valid and after which they are not. Non-inclusive.
  const stamp_type boundary = range.lower_stamp - min_period_;

  // Retry if priming is not possible
  if (PolicyType::queue_.oldest_stamp() > boundary)
  {
    if (latched_)
    {
      // If we have latched data, return that and report ready state
      return State::PRIMED;
    }
    else
    {
      // Abort if queue has no data and there is no latched data
      return State::ABORT;
    }
  }

  // Find data closest to boundary, starting from oldest
  // curr_qitr will never start at queue_.end() due to previous empty check
  auto curr_qitr = PolicyType::queue_.begin();
  auto prev_qitr = curr_qitr;
  while (curr_qitr != PolicyType::queue_.end() and get_stamp(*curr_qitr) <= boundary)
  {
    prev_qitr = curr_qitr;
    ++curr_qitr;
  }

  // Set latched element
  latched_ = *prev_qitr;

  // Remove all elements before latched element
  const auto prev_stamp = get_stamp(*prev_qitr);
  PolicyType::queue_.remove_before(prev_stamp);
  return State::PRIMED;
}

template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
void Latched<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::abort_follower_impl(const stamp_type& t_abort)
{
  // don't remove anything abort
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
void Latched<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::reset_follower_impl() noexcept(true)
{
  latched_.reset();
}

}  // namespace follower
}  // namespace flow

#endif  // FLOW_SRC_FOLLOWER_LATCHED_HPP

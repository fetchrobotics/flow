/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 *
 * @warning IMPLEMENTATION ONLY: THIS FILE SHOULD NEVER BE INCLUDED DIRECTLY!
 */
#ifndef FLOW_SRC_FOLLOWER_MATCHED_STAMP_HPP
#define FLOW_SRC_FOLLOWER_MATCHED_STAMP_HPP

// C++ Standard Library
#include <iterator>

namespace flow
{
namespace follower
{

template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
MatchedStamp<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::MatchedStamp(
  const ContainerT& container,
  const QueueMonitorT& queue_monitor) :
    PolicyType{container, queue_monitor}
{}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
template <typename OutputDispatchIteratorT>
State MatchedStamp<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::capture_follower_impl(
  OutputDispatchIteratorT output,
  const CaptureRange<stamp_type>& range)
{
  const State state = this->dry_capture_follower_impl(range);

  if (state == State::PRIMED)
  {
    // When primed, next element should be captured without removal
    *(output++) = *PolicyType::queue_.begin();
  }

  return state;
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
State MatchedStamp<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::dry_capture_follower_impl(
  const CaptureRange<stamp_type>& range)
{
  // Remove all elements before leading time
  PolicyType::queue_.remove_before(range.lower_stamp);

  if (PolicyType::queue_.empty())
  {
    return State::RETRY;
  }
  else if (PolicyType::queue_.oldest_stamp() > range.upper_stamp)
  {
    return State::ABORT;
  }

  // Assign matching element
  return State::PRIMED;
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
void MatchedStamp<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::abort_follower_impl(const stamp_type& t_abort)
{
  PolicyType::queue_.remove_before(t_abort);
}

}  // namespace follower
}  // namespace flow

#endif  // FLOW_SRC_FOLLOWER_MATCHED_STAMP_HPP

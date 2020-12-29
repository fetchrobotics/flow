/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl, Derek King
 *
 * @warning IMPLEMENTATION ONLY: THIS FILE SHOULD NEVER BE INCLUDED DIRECTLY!
 */
#ifndef FLOW_IMPL_FOLLOWER_CLOSEST_BEFORE_HPP
#define FLOW_IMPL_FOLLOWER_CLOSEST_BEFORE_HPP

namespace flow
{
namespace follower
{

template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
ClosestBefore<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::ClosestBefore(
  const offset_type& period,
  const offset_type& delay,
  const ContainerT& container,
  const QueueMonitorT& queue_monitor) :
    PolicyType{container, queue_monitor},
    period_{period},
    delay_{delay}
{}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
template <typename OutputDispatchIteratorT>
State ClosestBefore<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::capture_follower_impl(
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
State ClosestBefore<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::dry_capture_follower_impl(
  const CaptureRange<stamp_type>& range)
{
  // The boundary before which messages are valid and after which they are not. Non-inclusive.
  const stamp_type boundary = range.lower_stamp - delay_;

  // Starting from the oldest data, return on when data found within periodic window
  auto capture_itr = PolicyType::queue_.end();
  for (auto itr = PolicyType::queue_.begin(); itr != PolicyType::queue_.end(); ++itr)
  {
    if (get_stamp(*itr) >= boundary)
    {
      // If oldest element is far ahead of boundary, abort
      return State::ABORT;
    }
    else if (get_stamp(*itr) + period_ >= boundary)
    {
      // If oldest element is first within period, collect
      capture_itr = itr;
      break;
    }
  }

  // Check if inputs are capturable
  // NOTE: If no capture input was set, then all data is at or after boundary
  if (capture_itr != PolicyType::queue_.end())
  {
    PolicyType::queue_.remove_before(get_stamp(*capture_itr));
    return State::PRIMED;
  }
  else
  {
    return State::RETRY;
  }
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
void ClosestBefore<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::abort_follower_impl(const stamp_type& t_abort)
{
  PolicyType::queue_.remove_before(t_abort - delay_ - period_);
}

}  // namespace follower
}  // namespace flow

#endif  // FLOW_IMPL_FOLLOWER_CLOSEST_BEFORE_HPP

/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 *
 * @warning IMPLEMENTATION ONLY: THIS FILE SHOULD NEVER BE INCLUDED DIRECTLY!
 */
#ifndef FLOW_IMPL_DRIVER_TRHOTTLED_HPP
#define FLOW_IMPL_DRIVER_TRHOTTLED_HPP

// C++ Standard Library
#include <iterator>

namespace flow
{
namespace driver
{

template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
Throttled<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::Throttled(
  const offset_type throttle_period,
  const ContainerT& container,
  const QueueMonitorT& queue_monitor) noexcept(false) :
    PolicyType{container, queue_monitor},
    throttle_period_{throttle_period},
    previous_stamp_{StampTraits<stamp_type>::min()}
{}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
template <typename OutputDispatchIteratorT>
State Throttled<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::capture_driver_impl(
  OutputDispatchIteratorT output,
  CaptureRange<stamp_type>& range)
{
  const State state = this->dry_capture_driver_impl(range);

  if (state == State::PRIMED)
  {
    // Remove all elements before lower stamp, first
    PolicyType::queue_.remove_before(range.lower_stamp);

    // Cache previous stamp
    previous_stamp_ = range.lower_stamp;

    // Get next element and remove
    *(output++) = PolicyType::queue_.pop();
  }

  return state;
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
State Throttled<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::dry_capture_driver_impl(
  CaptureRange<stamp_type>& range) const
{
  for (const auto& dispatch : PolicyType::queue_)
  {
    if (
      previous_stamp_ == StampTraits<stamp_type>::min() or (get_stamp(dispatch) - previous_stamp_) >= throttle_period_)
    {
      range.lower_stamp = get_stamp(dispatch);
      range.upper_stamp = range.lower_stamp;

      return State::PRIMED;
    }
  }
  return State::RETRY;
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
void Throttled<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::abort_driver_impl(const stamp_type& t_abort)
{
  PolicyType::queue_.remove_before(t_abort);
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
void Throttled<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::reset_driver_impl()
{
  previous_stamp_ = StampTraits<stamp_type>::min();
}

}  // namespace driver
}  // namespace flow

#endif  // FLOW_IMPL_DRIVER_TRHOTTLED_HPP

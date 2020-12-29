/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 *
 * @warning IMPLEMENTATION ONLY: THIS FILE SHOULD NEVER BE INCLUDED DIRECTLY!
 */
#ifndef FLOW_IMPL_DRIVER_NEXT_HPP
#define FLOW_IMPL_DRIVER_NEXT_HPP

namespace flow
{
namespace driver
{

template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
Next<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::Next(
  const ContainerT& container,
  const QueueMonitorT& queue_monitor) noexcept(false) :
    PolicyType{container, queue_monitor}
{}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
template <typename OutputDispatchIteratorT>
State Next<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::capture_driver_impl(
  OutputDispatchIteratorT output,
  CaptureRange<stamp_type>& range)
{
  const State state = this->dry_capture_driver_impl(range);

  if (state == State::PRIMED)
  {
    // Get next element
    *(output++) = PolicyType::queue_.pop();
  }

  return state;
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
State Next<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::dry_capture_driver_impl(
  CaptureRange<stamp_type>& range) const
{
  if (PolicyType::queue_.empty())
  {
    return State::RETRY;
  }
  else
  {
    range.lower_stamp = PolicyType::queue_.oldest_stamp();
    range.upper_stamp = range.lower_stamp;

    return State::PRIMED;
  }
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
void Next<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::abort_driver_impl(const stamp_type& t_abort)
{
  PolicyType::queue_.remove_before(t_abort);
}

}  // namespace driver
}  // namespace flow

#endif  // FLOW_IMPL_DRIVER_NEXT_HPP

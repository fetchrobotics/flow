/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 *
 * @warning IMPLEMENTATION ONLY: THIS FILE SHOULD NEVER BE INCLUDED DIRECTLY!
 */
#ifndef FLOW_SRC_DRIVER_BATCH_HPP
#define FLOW_SRC_DRIVER_BATCH_HPP

// C++ Standard Library
#include <algorithm>
#include <stdexcept>

namespace flow
{
namespace driver
{

template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
Batch<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::Batch(
  const size_type size,
  const ContainerT& container,
  const QueueMonitorT& queue_monitor) noexcept(false) :
    PolicyType{container, queue_monitor},
    batch_size_{size}
{
  validate();
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
template <typename OutputDispatchIteratorT>
State Batch<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::capture_driver_impl(
  OutputDispatchIteratorT output,
  CaptureRange<stamp_type>& range)
{
  const State state = this->dry_capture_driver_impl(range);

  if (state == State::PRIMED)
  {
    // Collect dispatches
    std::copy_n(PolicyType::queue_.begin(), batch_size_, output);

    // Pop last element
    PolicyType::queue_.pop();
  }

  return state;
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
State Batch<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::dry_capture_driver_impl(
  CaptureRange<stamp_type>& range) const
{
  if (PolicyType::queue_.size() >= batch_size_)
  {
    auto oldest_itr = PolicyType::queue_.begin();

    range.lower_stamp = get_stamp(*oldest_itr);
    range.upper_stamp = get_stamp(*std::next(oldest_itr, batch_size_ - 1));

    return State::PRIMED;
  }
  else
  {
    return State::RETRY;
  }
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
void Batch<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::abort_driver_impl(const stamp_type& t_abort)
{
  PolicyType::queue_.remove_before(t_abort);
}


template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
void Batch<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>::validate() const noexcept(false)
{
  if (batch_size_ == 0)
  {
    throw std::invalid_argument{"'batch_size_' should be greater than zero"};
  }
}

}  // namespace driver
}  // namespace flow

#endif  // FLOW_SRC_DRIVER_BATCH_HPP

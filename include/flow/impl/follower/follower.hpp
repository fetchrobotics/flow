/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 *
 * @warning IMPLEMENTATION ONLY: THIS FILE SHOULD NEVER BE INCLUDED DIRECTLY!
 */
#ifndef FLOW_IMPL_FOLLOWER_FOLLOWER_HPP
#define FLOW_IMPL_FOLLOWER_FOLLOWER_HPP

// C++ Standard Library
#include <utility>

namespace flow
{

template <typename PolicyT>
Follower<PolicyT>::Follower(const DispatchContainerType& container, const DispatchQueueMonitorType& queue_monitor) :
    CaptorType{container, queue_monitor}
{}


template <typename PolicyT>
template <typename OutputDispatchIteratorT>
State Follower<PolicyT>::capture_policy_impl(OutputDispatchIteratorT&& output, const CaptureRange<stamp_type>& range)
{
  if (CaptorType::queue_monitor_.check(CaptorType::queue_, range))
  {
    return derived()->capture_follower_impl(std::forward<OutputDispatchIteratorT>(output), range);
  }
  else
  {
    return State::SKIP_FRAME_QUEUE_PRECONDITION;
  }
}


template <typename PolicyT> State Follower<PolicyT>::dry_capture_policy_impl(const CaptureRange<stamp_type>& range)
{
  return derived()->dry_capture_follower_impl(range);
}


template <typename PolicyT> void Follower<PolicyT>::abort_policy_impl(const stamp_type& t_abort)
{
  derived()->abort_follower_impl(t_abort);
}


template <typename PolicyT> void Follower<PolicyT>::reset_policy_impl() { derived()->reset_follower_impl(); }

}  // namespace flow

#endif  // FLOW_IMPL_FOLLOWER_FOLLOWER_HPP

/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 *
 * @warning IMPLEMENTATION ONLY: THIS FILE SHOULD NEVER BE INCLUDED DIRECTLY!
 */
#ifndef FLOW_SRC_DRIVER_DRIVER_HPP
#define FLOW_SRC_DRIVER_DRIVER_HPP

// C++ Standard Library
#include <utility>

namespace flow
{

template <typename PolicyT>
Driver<PolicyT>::Driver(const DispatchContainerType& container, const DispatchQueueMonitorType& queue_monitor) :
    CaptorType{container, queue_monitor}
{}


template <typename PolicyT>
template <typename OutputDispatchIteratorT>
State Driver<PolicyT>::capture_policy_impl(OutputDispatchIteratorT&& output, CaptureRange<stamp_type>& range)
{
  return derived()->capture_driver_impl(std::forward<OutputDispatchIteratorT>(output), range);
}


template <typename PolicyT> State Driver<PolicyT>::dry_capture_policy_impl(CaptureRange<stamp_type>& range)
{
  return derived()->dry_capture_driver_impl(range);
}


template <typename PolicyT> void Driver<PolicyT>::abort_policy_impl(const stamp_type& t_abort)
{
  derived()->abort_driver_impl(t_abort);
}


template <typename PolicyT> void Driver<PolicyT>::reset_policy_impl() { derived()->reset_driver_impl(); }

}  // namespace flow

#endif  // FLOW_SRC_DRIVER_DRIVER_HPP

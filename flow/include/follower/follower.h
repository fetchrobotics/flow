/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 *
 * @file follower.h
 */
#ifndef FLOW_FOLLOWER_FOLLOWER_H
#define FLOW_FOLLOWER_FOLLOWER_H

// C++ Standard Library
#include <type_traits>

// Flow
#include <flow/captor.h>
#include <flow/captor_state.h>
#include <flow/utility/implement_crtp_base.hpp>

namespace flow
{

// Forward declaration
template <typename PolicyT> class Follower;


/**
 * @copydoc CaptorTraits
 * @tparam PolicyT  CRTP-derived captor with specialized capture policy
 */
template <typename PolicyT> struct CaptorTraits<Follower<PolicyT>> : CaptorTraits<PolicyT>
{};


/**
 * @brief CRTP-base for Follower input-capture policies
 *
 *        Captures data w.r.t to a driving sequencing range, produced by a Driver,
 *        according to a synchronization policy
 *
 * @tparam PolicyT  CRTP-derived captor with specialized capture policy
 */
template <typename PolicyT>
class Follower : public Captor<
                   Follower<PolicyT>,
                   typename CaptorTraits<PolicyT>::LockPolicyType,
                   typename CaptorTraits<PolicyT>::DispatchQueueMonitorType>
{
public:
  /// Underlying dispatch container type
  using DispatchContainerType = typename CaptorTraits<PolicyT>::DispatchContainerType;

  /// Queue monitor/capture preconditioning type
  using DispatchQueueMonitorType = typename CaptorTraits<PolicyT>::DispatchQueueMonitorType;

  /// Data stamp type
  using stamp_type = typename CaptorTraits<PolicyT>::stamp_type;

  /**
   * @brief Initialization constructor
   *
   * @param container  container object with some initial state
   * @param queue_monitor  queue monitor with some initial state
   */
  Follower(const DispatchContainerType& container, const DispatchQueueMonitorType& queue_monitor);

private:
  /**
   * @brief Checks if buffer is in ready state and collects data based on a target time
   *
   * @param[out] output  output data iterator
   * @param[in] range  data capture/sequencing range
   *
   * @retval State::PRIMED    Data have been captured
   * @retval State::RETRY  Captor should continue waiting for messages after prime attempt
   */
  template <typename OutputDispatchIteratorT>
  inline State capture_policy_impl(OutputDispatchIteratorT&& output, const CaptureRange<stamp_type>& range);

  /**
   * @copydoc CaptorInterface::dry_capture
   */
  inline State dry_capture_policy_impl(const CaptureRange<stamp_type>& range);

  /**
   * @brief Defines Captor behavior on <code>ABORT</code>
   *
   *        Triggers data removal before \p t_abort
   *
   * @param t_abort  time at which abort was signaled
   */
  inline void abort_policy_impl(const stamp_type& t_abort);

  /**
   * @brief Defines Captor reset behavior
   */
  inline void reset_policy_impl();

  FLOW_IMPLEMENT_CRTP_BASE(PolicyT);

  using CaptorType = Captor<
    Follower,
    typename CaptorTraits<PolicyT>::LockPolicyType,
    typename CaptorTraits<PolicyT>::DispatchQueueMonitorType>;
  friend CaptorType;

protected:
  using CaptorType::queue_;
  using CaptorType::queue_monitor_;
};


/**
 * @brief Checks if captor object derived from a Follower base
 * @param CaptorT  object to test
 */
template <typename CaptorT>
struct is_follower : std::integral_constant<bool, std::is_base_of<Follower<CaptorT>, CaptorT>::value>
{};

}  // namespace flow

// Flow (implementation)
#include "flow/src/follower/follower.hpp"

#endif  // FLOW_FOLLOWER_FOLLOWER_H

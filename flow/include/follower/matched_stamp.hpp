/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 */
#ifndef FLOW_FOLLOWER_MATCHED_STAMP_HPP
#define FLOW_FOLLOWER_MATCHED_STAMP_HPP

// Flow
#include <flow/follower/follower.hpp>

namespace flow
{
namespace follower
{

/**
 * @brief Captures one element with a stamp which exactly matches the capture range lower bound.
 *
 *        All older elements are removed.
 *
 * @tparam DispatchT  data dispatch type
 * @tparam LockPolicyT  a BasicLockable (https://en.cppreference.com/w/cpp/named_req/BasicLockable) object or NoLock or
 * PollingLock
 * @tparam ContainerT  underlying <code>DispatchT</code> container type
 * @tparam QueueMonitorT  object used to monitor queue state on each insertion; used to precondition capture
 */
template <
  typename DispatchT,
  typename LockPolicyT = NoLock,
  typename ContainerT = DefaultContainer<DispatchT>,
  typename QueueMonitorT = DefaultDispatchQueueMonitor>
class MatchedStamp : public Follower<MatchedStamp<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>>
{
public:
  /// Data stamp type
  using stamp_type = typename CaptorTraits<MatchedStamp>::stamp_type;

  /**
   * @brief Setup constructor
   *
   * @param container  container object with some initial state
   * @param queue_monitor  queue monitor with some initial state
   */
  explicit MatchedStamp(
    const ContainerT& container = ContainerT{},
    const QueueMonitorT& queue_monitor = QueueMonitorT{});

private:
  using PolicyType = Follower<MatchedStamp<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>>;
  friend PolicyType;

  /**
   * @brief Checks if buffer is in ready state and collects data based on a target time
   *
   * @param[out] output  output data iterator
   * @param[in] range  data capture/sequencing range
   *
   * @retval PRIMED  If element(s) with sequencing stamp equal to
   *                 <code>range.upper_stamp</code> is available
   * @retval RETRY   If only element(s) with sequencing stamp less than
   *                 <code>range.upper_stamp</code> is available or queue is empty
   * @retval ABORT   If only element(s) with sequencing stamp greater than
   *                 <code>range.upper_stamp</code> is available
   */
  template <typename OutputDispatchIteratorT>
  inline State capture_follower_impl(OutputDispatchIteratorT output, const CaptureRange<stamp_type>& range);

  /**
   * @copydoc Follower::dry_capture_policy_impl
   */
  inline State dry_capture_follower_impl(const CaptureRange<stamp_type>& range);

  /**
   * @copydoc Follower::abort_policy_impl
   */
  inline void abort_follower_impl(const stamp_type& t_abort);

  /**
   * @copydoc Follower::reset_policy_impl
   */
  inline void reset_follower_impl() noexcept(true) {}
};

}  // namespace follower


/**
 * @copydoc CaptorTraits
 *
 * @tparam DispatchT  data dispatch type
 * @tparam LockPolicyT  a BasicLockable (https://en.cppreference.com/w/cpp/named_req/BasicLockable) object or NoLock or
 * PollingLock
 * @tparam ContainerT  underlying <code>DispatchT</code> container type
 * @tparam QueueMonitorT queue monitor/capture preconditioning type
 */
template <typename DispatchT, typename LockPolicyT, typename ContainerT, typename QueueMonitorT>
struct CaptorTraits<follower::MatchedStamp<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>>
    : CaptorTraitsFromDispatch<DispatchT>
{
  /// Underlying dispatch container type
  using DispatchContainerType = ContainerT;

  /// Queue monitor/capture preconditioning type
  using DispatchQueueMonitorType = QueueMonitorT;

  /// Thread locking policy type
  using LockPolicyType = LockPolicyT;

  /// Indicates that data from this captor will always be captured deterministically, so long as data
  /// injection is monotonically sequenced
  static constexpr bool is_capture_deterministic = true;
};

}  // namespace flow

// Flow (implementation)
#include "flow/src/follower/matched_stamp.hpp"

#endif  // FLOW_FOLLOWER_MATCHED_STAMP_HPP

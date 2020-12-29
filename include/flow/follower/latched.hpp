/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 */
#ifndef FLOW_FOLLOWER_LATCHED_HPP
#define FLOW_FOLLOWER_LATCHED_HPP

// Flow
#include <flow/follower/follower.hpp>
#include <flow/utility/optional.hpp>

namespace flow
{
namespace follower
{

/**
 * @brief Captures one element before the capture range lower bound, minus a minimum period
 *
 *        All older elements are removed. If no newer elements are present on the next capture attempt,
 *        then the last captured element is returned. If a newer element is present on a subsequent capture attempt,
 *        meeting the aforementioned qualifications, this elements is captured and replaces "latched" element state.
 *
 * @tparam DispatchT  data dispatch type
 * @tparam LockPolicyT  a BasicLockable (https://en.cppreference.com/w/cpp/named_req/BasicLockable) object or NoLock or
 * PollingLock
 * @tparam ContainerT  underlying <code>DispatchT</code> container type
 * @tparam QueueMonitorT  object used to monitor queue state on each insertion; used to precondition capture
 *
 * @note Latched won't behave non-deterministically if actual input period (difference between successive
 *       dispatch stamps) is greater than <code>min_period</code>. However, newer data values will not be captured if
 * they are within a <code>min_period<code> offset of <code>range.lower_stamp</code>
 *
 * @warn Latched may never enter a READY state if data never becomes available. Calling application may need to
 * implement a synchronization timeout behavior
 */
template <
  typename DispatchT,
  typename LockPolicyT = NoLock,
  typename ContainerT = DefaultContainer<DispatchT>,
  typename QueueMonitorT = DefaultDispatchQueueMonitor>
class Latched : public Follower<Latched<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>>
{
public:
  /// Data stamp type
  using stamp_type = typename CaptorTraits<Latched>::stamp_type;

  /// Data stamp duration type
  using offset_type = typename CaptorTraits<Latched>::offset_type;

  /**
   * @brief Setup constructor
   *
   * @param min_period  minimum expected difference between data stamps
   * @param container  container object with some initial state
   * @param queue_monitor  queue monitor with some initial state
   *
   * @throw <code>std::invalid_argument</code> if <code>m_after < 1</code>
   */
  explicit Latched(
    const offset_type min_period,
    const ContainerT& container = ContainerT{},
    const QueueMonitorT& queue_monitor = QueueMonitorT{});

private:
  using PolicyType = Follower<Latched<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>>;
  friend PolicyType;

  /**
   * @brief Checks if buffer is in ready state and collects data based on a target time
   *
   * @param[out] output  output data iterator
   * @param[in] range  data capture/sequencing range
   *
   * @retval PRIMED  If data element is available at or before <code>range.lower_stamp - min_period</code>
   * @retval RETRY   If data element is not available at or before <code>range.lower_stamp - min_period</code>
   * @retval ABORT   If data elements are only available after <code>range.lower_stamp - min_period</code>
   */
  template <typename OutputDispatchIteratorT>
  inline State capture_follower_impl(OutputDispatchIteratorT output, const CaptureRange<stamp_type>& range);

  /**
   * @copydoc Follower::dry_capture_policy_impl
   */
  inline State dry_capture_follower_impl(const CaptureRange<stamp_type>& range);

  /**
   * @brief Defines behavior on <code>ABORT</code>
   *
   * @param t_abort  sequencing stamp at which abort was signaled
   */
  inline void abort_follower_impl(const stamp_type& t_abort);

  /**
   * @copydoc Follower::reset_policy_impl
   *
   * @note clears latched data
   */
  inline void reset_follower_impl() noexcept(true);

  /// Latched dispatch
  ::flow::optional<DispatchT> latched_;

  /// Number of message before target to accept before ready
  offset_type min_period_;
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
struct CaptorTraits<follower::Latched<DispatchT, LockPolicyT, ContainerT, QueueMonitorT>>
    : CaptorTraitsFromDispatch<DispatchT>
{
  /// Underlying dispatch container type
  using DispatchContainerType = ContainerT;

  /// Queue monitor/capture preconditioning type
  using DispatchQueueMonitorType = QueueMonitorT;

  /// Thread locking policy type
  using LockPolicyType = LockPolicyT;

  /// Indicates that data from this captor will NOT always be captured deterministically;
  /// i.e. is always dependent on when data is injected, and when captrue is executed
  static constexpr bool is_capture_deterministic = false;
};

}  // namespace flow

// Flow (implementation)
#include <flow/impl/follower/latched.hpp>

#endif  // FLOW_FOLLOWER_LATCHED_HPP

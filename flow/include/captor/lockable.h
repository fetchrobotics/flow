/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 */
#ifndef FLOW_CAPTURE_CAPTOR_LOCKABLE_H
#define FLOW_CAPTURE_CAPTOR_LOCKABLE_H

// C++ Standard Library
#include <algorithm>
#include <condition_variable>
#include <iterator>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>

// Flow
#include <flow/captor.h>

namespace flow
{

/**
 * @copydoc Captor
 * @note Captor implementation which uses data waits/locks on <code>Captor::capture</code> for use in asynchronous,
 * multi-threaded code.
 */
template <typename CaptorT, typename LockableT, typename QueueMonitorT>
class Captor : public CaptorInterface<Captor<CaptorT, LockableT, QueueMonitorT>>
{
public:
  /// Data dispatch type
  using DispatchType = typename CaptorTraits<CaptorT>::DispatchType;

  /// Data dispatch container type
  using DispatchContainerType = typename CaptorTraits<CaptorT>::DispatchContainerType;

  /// Data stamp type
  using stamp_type = typename CaptorTraits<CaptorT>::stamp_type;

  /// Integer size type
  using size_type = typename CaptorTraits<CaptorT>::size_type;

  /**
   * @brief Default constructor
   */
  Captor() : CaptorInterfaceType{0UL, DispatchContainerType{}, QueueMonitorT{}}, capturing_{true} {}

  /**
   * @brief Dispatch container constructor
   *
   * @param container  container object with some initial state
   * @param queue_monitor  queue monitor with some initial state
   */
  Captor(const DispatchContainerType& container, const QueueMonitorT& queue_monitor) :
      CaptorInterfaceType{0UL, container, queue_monitor},
      capturing_{true}
  {}

  /**
   * @brief Destructor
   * @note Releases data waits
   */
  ~Captor() { abort_impl(StampTraits<stamp_type>::max()); }

private:
  /**
   * @copydoc CaptorInterface::reset
   */
  inline void reset_impl()
  {
    {
      LockableT lock{capture_mutex_};

      // Indicate that capture should stop
      capturing_ = false;

      // Run reset behavior specific to this captor
      derived()->reset_policy_impl();

      // Remove all data
      CaptorInterfaceType::queue_.clear();
    }

    // Release capture waits
    capture_cv_.notify_one();
  }

  /**
   * @copydoc CaptorInterface::size
   */
  inline size_type size_impl() const
  {
    LockableT lock{capture_mutex_};
    return CaptorInterfaceType::queue_.size();
  }

  /**
   * @copydoc CaptorInterface::inject
   */
  template <typename... DispatchConstructorArgTs> inline void inject_impl(DispatchConstructorArgTs&&... dispatch_args)
  {
    {
      // Insert new data
      LockableT lock{capture_mutex_};
      CaptorInterfaceType::insert_and_limit(std::forward<DispatchConstructorArgTs>(dispatch_args)...);
    }

    // Notify that new data has arrived
    capture_cv_.notify_one();
  }

  /**
   * @copydoc CaptorInterface::insert
   */
  template <typename FirstForwardDispatchIteratorT, typename LastForwardDispatchIteratorT>
  inline void insert_impl(FirstForwardDispatchIteratorT first, LastForwardDispatchIteratorT last)
  {
    {
      // Insert new data
      LockableT lock{capture_mutex_};
      std::for_each(
        first, last, [this](const DispatchType& dispatch) { CaptorInterfaceType::insert_and_limit(dispatch); });
    }

    // Notify that new data has arrived
    capture_cv_.notify_one();
  }

  /**
   * @copydoc CaptorInterface::remove
   */
  inline void remove_impl(const stamp_type& t_remove)
  {
    {
      // Remove all data before this time
      LockableT lock{capture_mutex_};
      CaptorInterfaceType::queue_.remove_before(t_remove);
    }

    // Notify that data has changed
    capture_cv_.notify_one();
  }


  /**
   * @copydoc CaptorInterface::abort
   */
  inline void abort_impl(const stamp_type& t_abort)
  {
    {
      LockableT lock{capture_mutex_};

      // Indicate that capture should stop
      capturing_ = false;

      // Run abort behavior specific to this captor
      derived()->abort_policy_impl(t_abort);
    }

    // Release capture waits
    capture_cv_.notify_one();
  }

  /**
   * @copydoc CaptorInterface::capture
   */
  template <typename OutputDispatchIteratorT, typename CaptureRangeT, typename ClockT, typename DurationT>
  inline State capture_impl(
    OutputDispatchIteratorT&& output,
    CaptureRangeT&& range,
    const std::chrono::time_point<ClockT, DurationT> timeout)
  {
    LockableT lock{capture_mutex_};

    // Wait for data and attempt capture when data is available
    State state{State::ABORT};
    while (capturing_)
    {
      // Attempt data capture
      state = derived()->capture_policy_impl(
        std::forward<OutputDispatchIteratorT>(output), std::forward<CaptureRangeT>(range));

      // Check capture state, and whether or not a data wait is needed
      if (state != State::RETRY)
      {
        break;
      }
      else if (std::chrono::time_point<ClockT, DurationT>::max() == timeout)
      {
        capture_cv_.wait(lock);
      }
      else if (std::cv_status::timeout == capture_cv_.wait_until(lock, timeout))
      {
        state = State::TIMEOUT;
        break;
      }
    }

    if (capturing_)
    {
      // Return state set in capture loop if not aborted externally
      return state;
    }
    else
    {
      // Make sure capturing flag is reset under lock before next capture attempt
      capturing_ = true;
      return (state == State::RETRY) ? State::ABORT : state;
    }
  }

  /**
   * @copydoc CaptorInterface::dry_capture_impl
   */
  template <typename CaptureRangeT> inline State dry_capture_impl(CaptureRangeT&& range)
  {
    LockableT lock{capture_mutex_};

    return derived()->dry_capture_policy_impl(std::forward<CaptureRangeT>(range));
  }

  /**
   * @copydoc CaptorInterface::inspect
   */
  template <typename InpectCallbackT> inline void inspect_impl(InpectCallbackT&& inspect_dispatch_cb) const
  {
    LockableT lock{capture_mutex_};

    for (const auto& dispatch : CaptorInterfaceType::queue_)
    {
      inspect_dispatch_cb(dispatch);
    }
  }

  /**
   * @copydoc CaptorInterface::update_queue_monitor
   */
  template <typename CaptureRangeT> void update_queue_monitor_impl(CaptureRangeT&& range, const State sync_state)
  {
    LockableT lock{capture_mutex_};
    CaptorInterfaceType::queue_monitor_.update(
      CaptorInterfaceType::queue_, std::forward<CaptureRangeT>(range), sync_state);
  }

  /**
   * @copydoc CaptorInterface::set_capacity
   */
  inline void set_capacity_impl(const size_type capacity)
  {
    LockableT lock{capture_mutex_};
    CaptorInterfaceType::capacity_ = capacity;
  }

  /**
   * @copydoc CaptorInterface::get_capacity
   */
  inline size_type get_capacity_impl() const
  {
    LockableT lock{capture_mutex_};
    return CaptorInterfaceType::capacity_;
  }

  /**
   * @copydoc CaptorInterface::get_available_stamp_range
   */
  inline CaptureRange<stamp_type> get_available_stamp_range_impl() const
  {
    LockableT lock{capture_mutex_};
    return queue_.empty() ? CaptureRange<stamp_type>{}
                          : CaptureRange<stamp_type>{queue_.oldest_stamp(), queue_.newest_stamp()};
  }

  /// Mutex to protect queue and captures
  mutable std::mutex capture_mutex_;

  /// Flag used to indicate that capture loop should continue
  volatile bool capturing_ = true;

  /**
   * @brief Condition variable used to wait for data
   *
   *        Uses <code>std::condition_variable</code> when <code>LockableT == std::unqiue_lock</code>
   *        as is may be more performant than <code>std::condition_variable_any</code> according to
   *        https://en.cppreference.com/w/cpp/thread/condition_variable_any
   */
  mutable typename std::conditional<
    std::is_same<std::unique_lock<std::mutex>, LockableT>::value,
    std::condition_variable,
    std::condition_variable_any>::type capture_cv_;

  using CaptorInterfaceType = CaptorInterface<Captor<CaptorT, LockableT, QueueMonitorT>>;
  friend CaptorInterfaceType;

  FLOW_IMPLEMENT_CRTP_BASE(CaptorT);

protected:
  using CaptorInterfaceType::queue_;
  using CaptorInterfaceType::queue_monitor_;
};

}  // namespace flow

#endif  // FLOW_CAPTURE_CAPTOR_LOCKABLE_H

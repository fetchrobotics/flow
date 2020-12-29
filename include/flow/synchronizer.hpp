/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 */
#ifndef FLOW_SYNCHRONIZER_HPP
#define FLOW_SYNCHRONIZER_HPP

// C++ Standard Library
#include <chrono>
#include <tuple>

// Flow
#include <flow/captor.hpp>
#include <flow/drivers.hpp>
#include <flow/followers.hpp>

namespace flow
{

/**
 * @brief Event synchronization result summary
 *
 * @tparam StampT  capture sequencing stamp type
 */
template <typename StampT> struct Result
{
  /// Captor state on exit
  State state;

  /// Driving sequencing stamp range
  CaptureRange<StampT> range;

  /// Default constructor
  inline Result() : state{State::RETRY} {}

  /**
   * @brief Operator overload to check if synchronization succeeded from details
   */
  inline operator bool() const { return state == State::PRIMED; }
};


/**
 * @brief Object used in place of output iterator as a placeholder with no data capture effects
 */
struct NoCapture
{
  /// No-op
  constexpr NoCapture& operator*() { return *this; }

  /// No-op
  constexpr NoCapture& operator++() { return *this; }

  /// No-op
  constexpr NoCapture& operator++(int) { return *this; }

  /// No-op
  template <typename ValueT> constexpr NoCapture& operator=(ValueT&&) { return *this; }
};


/**
 * @brief Resolves stamp type used by a captor
 *
 * @tparam CaptorT  captor object type
 */
template <typename CaptorT> struct SequenceStampType
{
  using type = typename CaptorTraits<CaptorT>::stamp_type;
};


/**
 * @copydoc SequenceStampType
 *
 * @note partial specialization for CaptureRange
 */
template <typename StampT> struct SequenceStampType<CaptureRange<StampT>>
{
  using type = StampT;
};


/**
 * @brief Provides facilities to synchronize data across several Captors
 */
class Synchronizer
{
public:
  /// Selects type of lesser size to use when passing arguments
  template <typename T> using arg_t = std::conditional_t<(sizeof(T) <= sizeof(T&)), T, T&>;

  /**
   * @brief Stamp type from capture sequence alias
   *
   * @tparam CaptorTupleT  tuple-like type of captors which supports access with <code>std::get</code>
   */
  template <typename CaptorTupleT>
  using stamp_t = typename SequenceStampType<std::remove_reference_t<std::tuple_element_t<0UL, CaptorTupleT>>>::type;

  /**
   * @brief Stamp argument type from capture sequence alias
   *
   * @tparam CaptorTupleT  tuple-like type of captors which supports access with <code>std::get</code>
   */
  template <typename CaptorTupleT> using stamp_arg_t = arg_t<stamp_t<CaptorTupleT>>;

  /**
   * @brief Result type from captor sequence alias
   *
   * @tparam CaptorTupleT  tuple-like type of captors which supports access with <code>std::get</code>
   */
  template <typename CaptorTupleT> using result_t = Result<stamp_t<CaptorTupleT>>;

  /**
   * @brief Removes all possible synchronization frames at and before \p t_remove
   *
   *        This does not necessarily remove data from all captors
   *
   * @tparam CaptorTupleT  tuple-like type of captors which supports access with <code>std::get</code>
   *
   * @param captors  tuple of captors used to perform synchronization
   * @param t_remove  data removal time point
   */
  template <typename CaptorTupleT> static void remove(CaptorTupleT&& captors, const stamp_arg_t<CaptorTupleT> t_remove);

  /**
   * @brief Abort active capture at and before \p t_abort
   *
   *        If a capture uses a data wait, this will notify the wait
   *        Captors will removing buffered data according to their specific abort policy.
   *
   * @tparam CaptorTupleT  tuple-like type of captors which supports access with <code>std::get</code>
   *
   * @param captors  tuple of captors used to perform synchronization
   * @param t_abort  abort time point
   */
  template <typename CaptorTupleT> static void abort(CaptorTupleT&& captors, const stamp_arg_t<CaptorTupleT> t_abort);

  /**
   * @brief Resets internal captors states and removes all buffered data
   *
   *        If a capture uses a data wait, this will notify the wait
   *
   * @tparam CaptorTupleT  tuple-like type of captors which supports access with <code>std::get</code>
   *
   * @param captors  tuple of captors used to perform synchronization
   */
  template <typename CaptorTupleT> static void reset(CaptorTupleT&& captors);

  /**
   * @brief Runs synchronization and data capture across all captors
   *
   * @tparam CaptorTupleT  tuple-like type of captors which supports access with <code>std::get</code>
   * @tparam OutputIteratorTupleT  tuple-like type of iterators which supports access with <code>std::get</code>
   * @tparam ClockT  clock type associated with <code>time_point</code>
   * @tparam DurationT  duration type associated with <code>time_point</code>
   *
   * @param captors  tuple of captors used to perform synchronization
   * @param outputs  tuple of dispatch output iterators, or NoCapture, ordered w.r.t associated Captor
   * @param lower_bound  synchronization stamp lower bound, forces all captured data to have associated
   *                     stamps which are greater than <code>lower_bound</code>
   * @param timeout  synchronization timeout for captors which require a data wait
   *
   * @return capture/synchronization details
   */
  template <typename CaptorTupleT, typename OutputIteratorTupleT, typename ClockT, typename DurationT>
  static result_t<CaptorTupleT> capture(
    CaptorTupleT&& captors,
    OutputIteratorTupleT&& outputs,
    const stamp_arg_t<CaptorTupleT> lower_bound,
    const std::chrono::time_point<ClockT, DurationT>& timeout);

  /**
   * @brief Runs synchronization and data capture across all captors
   *
   * @tparam CaptorTupleT  tuple-like type of captors which supports access with <code>std::get</code>
   * @tparam OutputIteratorTupleT  tuple-like type of iterators which supports access with <code>std::get</code>
   *
   * @param captors  tuple of captors used to perform synchronization
   * @param outputs  tuple of dispatch output iterators, or NoCapture, ordered w.r.t associated Captor
   * @param lower_bound  synchronization stamp lower bound, forces all captured data to have associated
   *                     stamps which are greater than <code>lower_bound</code>
   *
   * @return capture/synchronization details
   */
  template <typename CaptorTupleT, typename OutputIteratorTupleT>
  static result_t<CaptorTupleT> capture(
    CaptorTupleT&& captors,
    OutputIteratorTupleT&& outputs,
    const stamp_arg_t<CaptorTupleT> lower_bound = StampTraits<stamp_t<CaptorTupleT>>::min());

  /**
   * @brief Runs synchronization dry-run across all captors
   *
   *        Tests active next capture state without actually capturing elements. Any data
   *        changes that occur are such that the next call to <code>Synchronizer::capture</code>
   *        will be valid, and will have the same capture result if no changes have been made
   *        to data in the capture queues.
   *
   * @tparam CaptorTupleT  tuple-like type of captors which supports access with <code>std::get</code>
   *
   * @param captors  tuple of captors used to perform synchronization
   * @param lower_bound  synchronization stamp lower bound; forces all captured data to have associated
   *                     stamps which are greater than <code>lower_bound</code>
   *
   * @return dry capture/synchronization details
   */
  template <typename CaptorTupleT>
  static result_t<CaptorTupleT> dry_capture(
    CaptorTupleT&& captors,
    const stamp_arg_t<CaptorTupleT> lower_bound = StampTraits<stamp_t<CaptorTupleT>>::min());
};

}  // namespace flow

// Flow (implementation)
#include <flow/impl/synchronizer.hpp>

#endif  // FLOW_SYNCHRONIZER_HPP

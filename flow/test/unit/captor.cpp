/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 */
#ifndef DOXYGEN_SKIP

// C++ Standard Library
#include <chrono>
#include <memory>
#include <thread>
#include <vector>

// GTest
#include <gtest/gtest.h>

// Flow
#include <flow/captor.hpp>
#include <flow/captor/nolock.hpp>
#include <flow/captor_state.hpp>
#include <flow/captor_state_ostream.hpp>
#include <flow/driver/next.hpp>
#include <flow/follower/before.hpp>


using namespace flow;


TEST(CaptorCheckStampType, DefaultCapacity)
{
  driver::Next<Dispatch<int, int>> captor{};
  ASSERT_EQ(captor.get_capacity(), 0UL);
}


TEST(Captor, InspectCallback)
{
  driver::Next<Dispatch<int, int>> captor{};
  captor.inject(0, 1);

  std::size_t call_count = 0;
  captor.inspect([&call_count](const Dispatch<int, int>& dispatch) {
    ++call_count;
    ASSERT_EQ(get_stamp(dispatch), 0);
    ASSERT_EQ(get_value(dispatch), 1);
  });

  ASSERT_EQ(call_count, 1UL);
}


TEST(Captor, AvailableStampRangeEmpty)
{
  driver::Next<Dispatch<int, int>> captor{};

  const auto range = captor.get_available_stamp_range();

  ASSERT_FALSE(range);
  ASSERT_EQ(range.lower_stamp, StampTraits<int>::max());
  ASSERT_EQ(range.upper_stamp, StampTraits<int>::min());
}


TEST(Captor, AvailableStampRangeNonEmpty)
{
  driver::Next<Dispatch<int, int>> captor{};
  captor.inject(1, 1);
  captor.inject(10, 1);

  const auto range = captor.get_available_stamp_range();

  ASSERT_TRUE(range);
  ASSERT_EQ(range.lower_stamp, 1);
  ASSERT_EQ(range.upper_stamp, 10);
}


TEST(Captor, RemoveAllOnReset)
{
  driver::Next<Dispatch<int, int>> captor{};
  captor.inject(1, 1);
  captor.inject(10, 1);

  ASSERT_EQ(captor.size(), 2UL);

  captor.reset();

  ASSERT_EQ(captor.size(), 0UL);
}


struct AlwaysFalseQueueMonitor
{
  template <typename DispatchT, typename DispatchContainerT, typename StampT>
  static constexpr bool check(DispatchQueue<DispatchT, DispatchContainerT>&, const CaptureRange<StampT>&)
  {
    return false;
  };
};


TEST(Captor, SimpleQueueMonitorSkipState)
{
  follower::Before<Dispatch<int, int>, NoLock, DefaultContainer<Dispatch<int, int>>, AlwaysFalseQueueMonitor> captor{
    1 /*delay*/};

  captor.inject(1, 1);
  captor.inject(10, 1);

  std::vector<Dispatch<int, int>> captured;
  const auto state = captor.capture(std::back_inserter(captured), CaptureRange<int>{0, 0});

  ASSERT_EQ(state, State::SKIP_FRAME_QUEUE_PRECONDITION);
}

#endif  // DOXYGEN_SKIP

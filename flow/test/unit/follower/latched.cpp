/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 */
#ifndef DOXYGEN_SKIP

// C++ Standard Library
#include <cstdint>
#include <iterator>
#include <vector>

// GTest
#include <gtest/gtest.h>

// Flow
#include <flow/captor/nolock.h>
#include <flow/follower/latched.h>

using namespace flow;
using namespace flow::follower;


struct FollowerLatched : ::testing::Test, Latched<Dispatch<int, int>, NoLock>
{
  static constexpr int MIN_PERIOD = 5;

  FollowerLatched() : Latched<Dispatch<int, int>, NoLock>{MIN_PERIOD} {}

  void SetUp() final { this->reset(); }
};
constexpr int FollowerLatched::MIN_PERIOD;


TEST_F(FollowerLatched, CaptureRetryOnEmpty)
{
  std::vector<Dispatch<int, int>> data;

  CaptureRange<int> t_range{0, 0};

  ASSERT_EQ(State::RETRY, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 0UL);
}


TEST_F(FollowerLatched, CaptureAbortOnDataTooNew)
{
  std::vector<Dispatch<int, int>> data;

  this->inject(Dispatch<int, int>{0, 0});

  CaptureRange<int> t_range{MIN_PERIOD - 1, MIN_PERIOD - 1};

  ASSERT_EQ(State::ABORT, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 0UL);
}


TEST_F(FollowerLatched, CapturePrimedOnMinPeriodStamp)
{
  std::vector<Dispatch<int, int>> data;

  this->inject(Dispatch<int, int>{0, 232});

  CaptureRange<int> t_range{MIN_PERIOD, MIN_PERIOD};

  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), 1UL);
  ASSERT_EQ(data.size(), 1UL);
  ASSERT_EQ(get_value(data.front()), 232);
}


TEST_F(FollowerLatched, CapturePrimedOnBeforeMinPeriodStamp)
{
  std::vector<Dispatch<int, int>> data;

  this->inject(Dispatch<int, int>{0, 232});

  CaptureRange<int> t_range{MIN_PERIOD + 1, MIN_PERIOD + 1};

  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), 1UL);
  ASSERT_EQ(data.size(), 1UL);
  ASSERT_EQ(get_value(data.front()), 232);
}


TEST_F(FollowerLatched, CapturePrimedOnBeforeMinPeriodStampTakeNewer)
{
  std::vector<Dispatch<int, int>> data;

  this->inject(Dispatch<int, int>{0, 232});
  this->inject(Dispatch<int, int>{1, 233});

  CaptureRange<int> t_range{MIN_PERIOD + 1, MIN_PERIOD + 1};

  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), 1UL);
  ASSERT_EQ(data.size(), 1UL);
  ASSERT_EQ(get_value(data.front()), 233);
}


TEST_F(FollowerLatched, DryCaptureRetryOnEmpty)
{
  CaptureRange<int> t_range{0, 0};

  ASSERT_EQ(State::RETRY, this->dry_capture(t_range));
}


TEST_F(FollowerLatched, DryCaptureAbortOnDataTooNew)
{
  this->inject(Dispatch<int, int>{0, 0});

  CaptureRange<int> t_range{MIN_PERIOD - 1, MIN_PERIOD - 1};

  ASSERT_EQ(State::ABORT, this->dry_capture(t_range));
}


TEST_F(FollowerLatched, DryCapturePrimedOnMinPeriodStamp)
{
  this->inject(Dispatch<int, int>{0, 232});

  CaptureRange<int> t_range{MIN_PERIOD, MIN_PERIOD};

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerLatched, DryCapturePrimedOnBeforeMinPeriodStamp)
{
  this->inject(Dispatch<int, int>{0, 232});

  CaptureRange<int> t_range{MIN_PERIOD + 1, MIN_PERIOD + 1};

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerLatched, DryCapturePrimedOnBeforeMinPeriodStampTakeNewer)
{
  this->inject(Dispatch<int, int>{0, 232});
  this->inject(Dispatch<int, int>{1, 233});

  CaptureRange<int> t_range{MIN_PERIOD + 1, MIN_PERIOD + 1};

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerLatched, PrimedWithLatchedValue)
{
  std::vector<Dispatch<int, int>> data;

  this->inject(Dispatch<int, int>{0, 232});

  CaptureRange<int> t_range_priming{MIN_PERIOD, MIN_PERIOD};

  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range_priming));

  this->inject(Dispatch<int, int>{MIN_PERIOD, 233});

  CaptureRange<int> t_range_successor{MIN_PERIOD + 1, MIN_PERIOD + 1};

  data.clear();

  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range_successor));
  ASSERT_EQ(this->size(), 2UL);
  ASSERT_EQ(data.size(), 1UL);
  ASSERT_EQ(get_value(data.front()), 232);
}


TEST_F(FollowerLatched, RemovalOnAbort)
{
  // Start injecting data
  const int t0 = 0;
  int t = t0;
  int N = 10;
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, 1});
    t += 1;
  }

  this->abort(2);

  ASSERT_EQ(this->size(), static_cast<std::size_t>(10));
}

#endif  // DOXYGEN_SKIP

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
#include <flow/driver/next.h>

using namespace flow;
using namespace flow::driver;


struct DriverNext : ::testing::Test, Next<Dispatch<int, int>, NoLock>
{
  DriverNext() : Next<Dispatch<int, int>, NoLock>{} {}

  void SetUp() final { this->reset(); }
};


TEST_F(DriverNext, CaptureRetryOnEmpty)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::RETRY, this->capture(std::back_inserter(data), t_range));
}


TEST_F(DriverNext, CapturePrimedWithOldest)
{
  const int t = 1;
  this->inject(Dispatch<int, int>{t + 0, 1});
  this->inject(Dispatch<int, int>{t + 1, 2});

  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));

  EXPECT_EQ(t_range.lower_stamp, t);
  EXPECT_EQ(t_range.upper_stamp, t);
}


TEST_F(DriverNext, DryCaptureRetryOnEmpty)
{
  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::RETRY, this->dry_capture(t_range));
}


TEST_F(DriverNext, DryCapturePrimedWithOldest)
{
  const int t = 1;
  this->inject(Dispatch<int, int>{t + 0, 1});
  this->inject(Dispatch<int, int>{t + 1, 2});

  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));

  EXPECT_EQ(t_range.lower_stamp, t);
  EXPECT_EQ(t_range.upper_stamp, t);
}


TEST_F(DriverNext, RemovalOnAbort)
{
  // Start injecting data
  const int t0 = 0;
  int t = t0;
  int N = 10 + 1;
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, 1});
    t += 1;
  }

  this->abort(10);

  ASSERT_EQ(this->size(), 1UL);
}

#endif  // DOXYGEN_SKIP

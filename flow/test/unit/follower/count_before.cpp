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
#include <flow/follower/count_before.h>

using namespace flow;
using namespace flow::follower;


struct FollowerCountBefore : ::testing::Test, CountBefore<Dispatch<int, int>, NoLock>
{
  static constexpr int COUNT = 3;
  static constexpr int DELAY = 3;

  FollowerCountBefore() : CountBefore<Dispatch<int, int>, NoLock>{COUNT, DELAY} {}

  void SetUp() final { this->reset(); }
};
constexpr int FollowerCountBefore::COUNT;
constexpr int FollowerCountBefore::DELAY;


TEST_F(FollowerCountBefore, CaptureRetryOnEmpty)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::RETRY, this->capture(std::back_inserter(data), t_range));
}


TEST_F(FollowerCountBefore, CaptureRetryOnToFewBefore)
{
  std::size_t N = COUNT - 1;

  int t = 0 - (COUNT + DELAY);
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, t});
    t++;
  }

  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};

  const auto size_before_capture = this->size();
  ASSERT_EQ(State::RETRY, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(size_before_capture, this->size());
}


TEST_F(FollowerCountBefore, CaptureAbortOnToFewBeforeWithDataAfter)
{
  std::size_t N = COUNT;

  int t = 0;
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, t});
    t++;
  }

  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};

  const auto size_before_capture = this->size();
  ASSERT_EQ(State::ABORT, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(size_before_capture, this->size());
}


TEST_F(FollowerCountBefore, CapturePrimedWithExactBefore)
{
  std::size_t N = COUNT;

  int t = 0 - (COUNT + DELAY);
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, t});
    t++;
  }

  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};
  const auto size_before_capture = this->size();
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(size_before_capture, this->size());
  ASSERT_EQ(data.size(), static_cast<std::size_t>(COUNT));
}


TEST_F(FollowerCountBefore, DryCaptureRetryOnEmpty)
{
  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::RETRY, this->dry_capture(t_range));
}


TEST_F(FollowerCountBefore, DryCaptureRetryOnToFewBefore)
{
  std::size_t N = COUNT - 1;

  int t = 0 - (COUNT + DELAY);
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, t});
    t++;
  }

  CaptureRange<int> t_range{0, 0};

  ASSERT_EQ(State::RETRY, this->dry_capture(t_range));
}


TEST_F(FollowerCountBefore, DryCaptureAbortOnToFewBeforeWithDataAfter)
{
  std::size_t N = COUNT;

  int t = 0;
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, t});
    t++;
  }

  CaptureRange<int> t_range{0, 0};

  ASSERT_EQ(State::ABORT, this->dry_capture(t_range));
}


TEST_F(FollowerCountBefore, DryCapturePrimedWithExactBefore)
{
  std::size_t N = COUNT;

  int t = 0 - (COUNT + DELAY);
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, t});
    t++;
  }

  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerCountBefore, PrimedWithExcessBefore)
{
  constexpr static std::size_t EXCESS = 2;

  std::size_t N = COUNT + EXCESS;

  int t = 0 - static_cast<int>(COUNT + EXCESS + DELAY);
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, t});
    t++;
  }

  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};
  const auto size_before_capture = this->size();
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(size_before_capture - EXCESS, this->size());
  ASSERT_EQ(data.size(), static_cast<std::size_t>(COUNT));

  for (const auto& dispatch : data)
  {
    const auto offset = get_stamp(dispatch) + DELAY;
    ASSERT_LT(offset, 0);
    ASSERT_GE(offset, -COUNT);
  }
}


TEST_F(FollowerCountBefore, RemovalOnAbort)
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

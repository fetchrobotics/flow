/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Levon Avagyan, Brian Cairl
 */
#ifndef DOXYGEN_SKIP

// C++ Standard Library
#include <iterator>
#include <vector>

// GTest
#include <gtest/gtest.h>

// Flow
#include <flow/captor/nolock.hpp>
#include <flow/follower/before.hpp>

using namespace flow;
using namespace flow::follower;


struct FollowerBefore : ::testing::Test, Before<Dispatch<int, int>, NoLock>
{
  static constexpr int DELAY = 1;

  FollowerBefore() : Before<Dispatch<int, int>, NoLock>{DELAY} {}

  void SetUp() final { this->reset(); }
};
constexpr int FollowerBefore::DELAY;


TEST_F(FollowerBefore, CaptureRetryOnEmpty)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::RETRY, this->capture(std::back_inserter(data), t_range));
}


TEST_F(FollowerBefore, CapturePrimedOnDataAtBoundary)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY, 1});

  ASSERT_EQ(this->size(), 1U);
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), 1U);

  ASSERT_EQ(data.size(), 0U);
}


TEST_F(FollowerBefore, CapturePrimedOnDataAfterBoundary)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY + 1, 1});

  ASSERT_EQ(this->size(), 1U);
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), 1U);

  ASSERT_EQ(data.size(), 0U);
}


TEST_F(FollowerBefore, CaptureRetryOnDataBeforeBoundary)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 1, 1});

  ASSERT_EQ(this->size(), 1U);
  ASSERT_EQ(State::RETRY, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), 1U);

  ASSERT_EQ(data.size(), 0U);
}


TEST_F(FollowerBefore, CapturePrimedOnDataBeforeAndAfterBoundary)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 1, 1});
  this->inject(Dispatch<int, int>{-DELAY + 1, 1});

  ASSERT_EQ(this->size(), 2U);
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), 1U);

  ASSERT_EQ(data.size(), 1U);
}


TEST_F(FollowerBefore, CapturePrimedOnDataBeforeAndAtBoundary)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 1, 1});
  this->inject(Dispatch<int, int>{-DELAY, 1});

  ASSERT_EQ(this->size(), 2U);
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), 1U);

  ASSERT_EQ(data.size(), 1U);
}


TEST_F(FollowerBefore, CapturePrimedMultiDataBeforeBoundary)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 1, 1});
  this->inject(Dispatch<int, int>{-DELAY - 2, 1});
  this->inject(Dispatch<int, int>{-DELAY + 1, 1});

  ASSERT_EQ(this->size(), 3U);
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), 1U);

  ASSERT_EQ(data.size(), 2U);
}


TEST_F(FollowerBefore, CapturePrimedMultiDataBeforeAndAtBoundary)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 0, 1});
  this->inject(Dispatch<int, int>{-DELAY - 1, 1});
  this->inject(Dispatch<int, int>{-DELAY - 2, 1});

  ASSERT_EQ(this->size(), 3U);
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), 1U);

  ASSERT_EQ(data.size(), 2U);
}


TEST_F(FollowerBefore, DryCaptureRetryOnEmpty)
{
  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::RETRY, this->dry_capture(t_range));
}


TEST_F(FollowerBefore, DryCapturePrimedOnDataAtBoundary)
{
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY, 1});

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerBefore, DryCapturePrimedOnDataAfterBoundary)
{
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY + 1, 1});

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerBefore, DryCaptureRetryOnDataBeforeBoundary)
{
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 1, 1});

  ASSERT_EQ(State::RETRY, this->dry_capture(t_range));
}


TEST_F(FollowerBefore, DryCapturePrimedOnDataBeforeAndAfterBoundary)
{
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 1, 1});
  this->inject(Dispatch<int, int>{-DELAY + 1, 1});

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerBefore, DryCapturePrimedOnDataBeforeAndAtBoundary)
{
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 1, 1});
  this->inject(Dispatch<int, int>{-DELAY, 1});

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerBefore, DryCapturePrimedMultiDataBeforeBoundary)
{
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 1, 1});
  this->inject(Dispatch<int, int>{-DELAY - 2, 1});
  this->inject(Dispatch<int, int>{-DELAY + 1, 1});

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerBefore, DryCapturePrimedMultiDataBeforeAndAtBoundary)
{
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 0, 1});
  this->inject(Dispatch<int, int>{-DELAY - 1, 1});
  this->inject(Dispatch<int, int>{-DELAY - 2, 1});

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerBefore, RemovalOnAbort)
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

  this->abort(5);

  ASSERT_EQ(this->size(), static_cast<std::size_t>(DELAY + 5));
}

#endif  // DOXYGEN_SKIP

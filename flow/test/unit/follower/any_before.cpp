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
#include <flow/captor/nolock.h>
#include <flow/follower/any_before.h>

using namespace flow;
using namespace flow::follower;


struct FollowerAnyBefore : ::testing::Test, AnyBefore<Dispatch<int, int>, NoLock>
{
  static constexpr int DELAY = 1;

  FollowerAnyBefore() : AnyBefore<Dispatch<int, int>, NoLock>{DELAY} {}

  void SetUp() final { this->reset(); }
};
constexpr int FollowerAnyBefore::DELAY;


TEST_F(FollowerAnyBefore, CapturePrimedOnEmpty)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
}


TEST_F(FollowerAnyBefore, CapturePrimedOnDataAtBoundary)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY, 1});

  ASSERT_EQ(this->size(), 1U);
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), 1U);

  ASSERT_EQ(data.size(), 0U);
}


TEST_F(FollowerAnyBefore, CapturePrimedOnDataAfterBoundary)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY + 1, 1});

  ASSERT_EQ(this->size(), 1U);
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), 1U);

  ASSERT_EQ(data.size(), 0U);
}


TEST_F(FollowerAnyBefore, CapturePrimedOnDataAnyBeforeBoundary)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 1, 1});

  ASSERT_EQ(this->size(), 1U);
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), 0U);

  ASSERT_EQ(data.size(), 1U);
}


TEST_F(FollowerAnyBefore, CapturePrimedOnDataAnyBeforeAndAfterBoundary)
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


TEST_F(FollowerAnyBefore, CapturePrimedOnDataAnyBeforeAndAtBoundary)
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


TEST_F(FollowerAnyBefore, CapturePrimedMultiDataAnyBeforeBoundary)
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


TEST_F(FollowerAnyBefore, CapturePrimedMultiDataAnyBeforeAndAtBoundary)
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


TEST_F(FollowerAnyBefore, DryCapturePrimedOnEmpty)
{
  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerAnyBefore, DryCapturePrimedOnDataAtBoundary)
{
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY, 1});

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerAnyBefore, DryCapturePrimedOnDataAfterBoundary)
{
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY + 1, 1});

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerAnyBefore, DryCapturePrimedOnDataAnyBeforeBoundary)
{
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 1, 1});

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerAnyBefore, DryCapturePrimedOnDataAnyBeforeAndAfterBoundary)
{
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 1, 1});
  this->inject(Dispatch<int, int>{-DELAY + 1, 1});

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerAnyBefore, DryCapturePrimedOnDataAnyBeforeAndAtBoundary)
{
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 1, 1});
  this->inject(Dispatch<int, int>{-DELAY, 1});

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerAnyBefore, DryCapturePrimedMultiDataAnyBeforeBoundary)
{
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 1, 1});
  this->inject(Dispatch<int, int>{-DELAY - 2, 1});
  this->inject(Dispatch<int, int>{-DELAY + 1, 1});

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerAnyBefore, DryCapturePrimedMultiDataAnyBeforeAndAtBoundary)
{
  CaptureRange<int> t_range{0, 0};

  this->inject(Dispatch<int, int>{-DELAY - 0, 1});
  this->inject(Dispatch<int, int>{-DELAY - 1, 1});
  this->inject(Dispatch<int, int>{-DELAY - 2, 1});

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));
}


TEST_F(FollowerAnyBefore, RemovalOnAbort)
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

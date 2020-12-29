/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 */
#ifndef DOXYGEN_SKIP

// C++ Standard Library
#include <cstdint>
#include <iterator>
#include <tuple>
#include <vector>

// GTest
#include <gtest/gtest.h>

// Flow
#include <flow/captor/nolock.hpp>
#include <flow/follower/ranged.hpp>

using namespace flow;
using namespace flow::follower;


struct FollowerRangedTest : ::testing::TestWithParam<::std::tuple<int>>
{
  using CaptorType = Ranged<Dispatch<int, int>, NoLock>;

  void SetUp() final { p_delay = std::get<0>(GetParam()); }

  int p_delay = 0;
};


TEST_P(FollowerRangedTest, CaptureRetryOnEmpty)
{
  CaptorType captor{p_delay};

  std::vector<Dispatch<int, int>> data;

  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::RETRY, captor.capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 0UL);
}


TEST_P(FollowerRangedTest, CaptureAbortOnTooFewBeforeZeroRange)
{
  CaptorType captor{p_delay};

  std::vector<Dispatch<int, int>> data;

  captor.inject(-p_delay + 1, 100);
  captor.inject(-p_delay + 2, 100);

  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::ABORT, captor.capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 0UL);
}


TEST_P(FollowerRangedTest, CaptureAbortOnTooFewBeforeNonZeroRange)
{
  CaptorType captor{p_delay};

  std::vector<Dispatch<int, int>> data;

  captor.inject(-p_delay + 1, 100);
  captor.inject(-p_delay + 2, 100);

  CaptureRange<int> t_range{0, 1};
  ASSERT_EQ(State::ABORT, captor.capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 0UL);
}


TEST_P(FollowerRangedTest, CaptureRetryOnNoneAfterZeroRange)
{
  CaptorType captor{p_delay};

  std::vector<Dispatch<int, int>> data;

  captor.inject(-p_delay - 1, 100);
  captor.inject(-p_delay - 2, 100);

  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::RETRY, captor.capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 0UL);
}


TEST_P(FollowerRangedTest, CaptureRetryOnNoneAfterNonZeroRange)
{
  CaptorType captor{p_delay};

  std::vector<Dispatch<int, int>> data;

  captor.inject(-p_delay - 1, 100);
  captor.inject(-p_delay + 1, 100);

  CaptureRange<int> t_range{0, 1};
  ASSERT_EQ(State::RETRY, captor.capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 0UL);
}


TEST_P(FollowerRangedTest, CaptureOnZeroRange)
{
  CaptorType captor{p_delay};

  std::vector<Dispatch<int, int>> data;

  captor.inject(-p_delay - 1, 100);
  captor.inject(-p_delay + 1, 100);

  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::PRIMED, captor.capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 2UL);
}


TEST_P(FollowerRangedTest, CaptureOnZeroRangeWithIntermediate)
{
  CaptorType captor{p_delay};

  std::vector<Dispatch<int, int>> data;

  captor.inject(-p_delay - 1, 100);
  captor.inject(-p_delay, 100);
  captor.inject(-p_delay + 1, 100);

  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::PRIMED, captor.capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 3UL);
}


TEST_P(FollowerRangedTest, CaptureOnNonZeroRange)
{
  CaptorType captor{p_delay};

  std::vector<Dispatch<int, int>> data;

  captor.inject(-p_delay - 1, 100);
  captor.inject(-p_delay + 2, 100);

  CaptureRange<int> t_range{0, 1};
  ASSERT_EQ(State::PRIMED, captor.capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 2UL);
}


TEST_P(FollowerRangedTest, CaptureOnNonZeroRangeWithIntermediate)
{
  CaptorType captor{p_delay};

  std::vector<Dispatch<int, int>> data;

  captor.inject(-p_delay - 1, 100);
  captor.inject(-p_delay, 100);
  captor.inject(-p_delay + 1, 100);
  captor.inject(-p_delay + 2, 100);

  CaptureRange<int> t_range{0, 1};
  ASSERT_EQ(State::PRIMED, captor.capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 4UL);
}


TEST_P(FollowerRangedTest, DryCaptureRetryOnEmpty)
{
  CaptorType captor{p_delay};

  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::RETRY, captor.dry_capture(t_range));
}


TEST_P(FollowerRangedTest, DryCaptureAbortOnTooFewBeforeZeroRange)
{
  CaptorType captor{p_delay};

  captor.inject(-p_delay + 1, 100);
  captor.inject(-p_delay + 2, 100);

  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::ABORT, captor.dry_capture(t_range));
}


TEST_P(FollowerRangedTest, DryCaptureAbortOnTooFewBeforeNonZeroRange)
{
  CaptorType captor{p_delay};

  captor.inject(-p_delay + 1, 100);
  captor.inject(-p_delay + 2, 100);

  CaptureRange<int> t_range{0, 1};
  ASSERT_EQ(State::ABORT, captor.dry_capture(t_range));
}


TEST_P(FollowerRangedTest, DryCaptureRetryOnNoneAfterZeroRange)
{
  CaptorType captor{p_delay};

  captor.inject(-p_delay - 1, 100);
  captor.inject(-p_delay - 2, 100);

  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::RETRY, captor.dry_capture(t_range));
}


TEST_P(FollowerRangedTest, DryCaptureRetryOnNoneAfterNonZeroRange)
{
  CaptorType captor{p_delay};

  captor.inject(-p_delay - 1, 100);
  captor.inject(-p_delay + 1, 100);

  CaptureRange<int> t_range{0, 1};
  ASSERT_EQ(State::RETRY, captor.dry_capture(t_range));
}


TEST_P(FollowerRangedTest, DryCaptureOnZeroRange)
{
  CaptorType captor{p_delay};

  captor.inject(-p_delay - 1, 100);
  captor.inject(-p_delay + 1, 100);

  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::PRIMED, captor.dry_capture(t_range));
}


TEST_P(FollowerRangedTest, DryCaptureOnZeroRangeWithIntermediate)
{
  CaptorType captor{p_delay};

  captor.inject(-p_delay - 1, 100);
  captor.inject(-p_delay, 100);
  captor.inject(-p_delay + 1, 100);

  CaptureRange<int> t_range{0, 0};
  ASSERT_EQ(State::PRIMED, captor.dry_capture(t_range));
}


TEST_P(FollowerRangedTest, DryCaptureOnNonZeroRange)
{
  CaptorType captor{p_delay};

  captor.inject(-p_delay - 1, 100);
  captor.inject(-p_delay + 2, 100);

  CaptureRange<int> t_range{0, 1};
  ASSERT_EQ(State::PRIMED, captor.dry_capture(t_range));
}


TEST_P(FollowerRangedTest, DryCaptureOnNonZeroRangeWithIntermediate)
{
  CaptorType captor{p_delay};

  captor.inject(-p_delay - 1, 100);
  captor.inject(-p_delay, 100);
  captor.inject(-p_delay + 1, 100);
  captor.inject(-p_delay + 2, 100);

  CaptureRange<int> t_range{0, 1};
  ASSERT_EQ(State::PRIMED, captor.dry_capture(t_range));
}


TEST_P(FollowerRangedTest, RemovalOnAbort)
{
  CaptorType captor{p_delay};

  // Start injecting data
  const int t0 = 0;
  int t = t0;
  int N = 10;
  while (N--)
  {
    captor.inject(Dispatch<int, int>{t, 1});
    t += 1;
  }

  captor.abort(2);

  ASSERT_EQ(captor.size(), static_cast<std::size_t>(10));
}


INSTANTIATE_TEST_SUITE_P(
  FollowerRangedTestSweep,
  FollowerRangedTest,
  testing::Combine(testing::ValuesIn(std::vector<int>{0, 1, 2})  // delay
                   ));

#endif  // DOXYGEN_SKIP

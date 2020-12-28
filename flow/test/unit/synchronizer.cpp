/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 */
#ifndef DOXYGEN_SKIP

// C++ Standard Library
#include <deque>
#include <iterator>
#include <list>
#include <tuple>
#include <utility>
#include <vector>

// GTest
#include <gtest/gtest.h>

// Flow
#include <flow/captor/nolock.h>
#include <flow/captor_state_ostream.h>
#include <flow/drivers.h>
#include <flow/followers.h>
#include <flow/synchronizer.h>

using namespace flow;

/// Tests the Synchronizer class
class SynchronizerTestSuiteST : public ::testing::Test
{
public:
  using Driver = driver::Next<Dispatch<int, int>, NoLock>;
  using Follower1 = follower::ClosestBefore<Dispatch<int, double>, NoLock>;
  using Follower2 = follower::Before<Dispatch<int, std::string>, NoLock>;

  /// Persistent capture buffer resources
  std::shared_ptr<Driver> driver;
  std::shared_ptr<Follower1> follower1;
  std::shared_ptr<Follower2> follower2;

  /// Constructor
  SynchronizerTestSuiteST() {}

  /**
   * @brief Test initialization
   */
  void SetUp() final
  {
    driver = std::make_shared<Driver>();
    follower1 = std::make_shared<Follower1>(5, 0);
    follower2 = std::make_shared<Follower2>(0);
  }

  /**
   * @brief Test cleanup
   */
  void TearDown() final {}
};


TEST_F(SynchronizerTestSuiteST, Reset) { Synchronizer::reset(std::forward_as_tuple(*driver, *follower1, *follower2)); }


TEST_F(SynchronizerTestSuiteST, CaptureCannotPrimeRetry)
{
  std::vector<Dispatch<int, int>> driver_output_data;
  std::vector<Dispatch<int, double>> follower1_output_data;
  std::vector<Dispatch<int, std::string>> follower2_output_data;

  const auto result = Synchronizer::capture(
    std::forward_as_tuple(*driver, *follower1, *follower2),
    std::forward_as_tuple(
      std::back_inserter(driver_output_data),
      std::back_inserter(follower1_output_data),
      std::back_inserter(follower2_output_data)),
    0);

  ASSERT_FALSE(result);
  ASSERT_EQ(result.state, State::RETRY);
}


TEST_F(SynchronizerTestSuiteST, CaptureCannotPrimeAbort)
{
  driver->inject(Dispatch<int, int>{1, 1});
  follower1->inject(Dispatch<int, double>{7, 2.0});
  follower2->inject(Dispatch<int, std::string>{0, "ok"});

  std::vector<Dispatch<int, int>> driver_output_data;
  std::vector<Dispatch<int, double>> follower1_output_data;
  std::vector<Dispatch<int, std::string>> follower2_output_data;

  const auto result = Synchronizer::capture(
    std::forward_as_tuple(*driver, *follower1, *follower2),
    std::forward_as_tuple(
      std::back_inserter(driver_output_data),
      std::back_inserter(follower1_output_data),
      std::back_inserter(follower2_output_data)),
    0);

  ASSERT_FALSE(result);
  ASSERT_EQ(result.state, State::ABORT);
}


TEST_F(SynchronizerTestSuiteST, CaptureCanPrime)
{
  driver->inject(Dispatch<int, int>{10, 10});
  follower1->inject(Dispatch<int, double>{0, 2.0});
  follower1->inject(Dispatch<int, double>{9, 2.0});
  follower2->inject(Dispatch<int, std::string>{20, "ok"});

  std::vector<Dispatch<int, int>> driver_output_data;
  std::vector<Dispatch<int, double>> follower1_output_data;
  std::vector<Dispatch<int, std::string>> follower2_output_data;

  const auto result = Synchronizer::capture(
    std::forward_as_tuple(*driver, *follower1, *follower2),
    std::forward_as_tuple(
      std::back_inserter(driver_output_data),
      std::back_inserter(follower1_output_data),
      std::back_inserter(follower2_output_data)),
    0);

  ASSERT_TRUE(result);
  ASSERT_EQ(result.state, State::PRIMED);

  ASSERT_FALSE(driver_output_data.empty());
  ASSERT_FALSE(follower1_output_data.empty());
  ASSERT_TRUE(follower2_output_data.empty());
}


TEST_F(SynchronizerTestSuiteST, CaptureCanPrimeNoCapture)
{
  driver->inject(Dispatch<int, int>{10, 10});
  follower1->inject(Dispatch<int, double>{0, 2.0});
  follower1->inject(Dispatch<int, double>{9, 2.0});
  follower2->inject(Dispatch<int, std::string>{20, "ok"});

  const auto result = Synchronizer::capture(
    std::forward_as_tuple(*driver, *follower1, *follower2),
    std::forward_as_tuple(NoCapture{}, NoCapture{}, NoCapture{}),
    0);

  ASSERT_TRUE(result);
  ASSERT_EQ(result.state, State::PRIMED);
}


TEST_F(SynchronizerTestSuiteST, CaptureDirectCaptureRange)
{
  follower1->inject(Dispatch<int, double>{0, 2.0});
  follower1->inject(Dispatch<int, double>{9, 2.0});
  follower2->inject(Dispatch<int, std::string>{20, "ok"});

  std::vector<Dispatch<int, double>> follower1_output_data;
  std::vector<Dispatch<int, std::string>> follower2_output_data;

  const auto result = Synchronizer::capture(
    std::forward_as_tuple(CaptureRange<int>{10, 10}, *follower1, *follower2),
    std::forward_as_tuple(
      NoCapture{}, std::back_inserter(follower1_output_data), std::back_inserter(follower2_output_data)),
    0);

  ASSERT_TRUE(result);
  ASSERT_EQ(result.state, State::PRIMED);

  ASSERT_FALSE(follower1_output_data.empty());
  ASSERT_TRUE(follower2_output_data.empty());
}


TEST_F(SynchronizerTestSuiteST, CaptureErrorTimeGuard)
{
  driver->inject(Dispatch<int, int>{10, 10});
  follower1->inject(Dispatch<int, double>{0, 2.0});
  follower1->inject(Dispatch<int, double>{9, 2.0});
  follower2->inject(Dispatch<int, std::string>{20, "ok"});

  std::vector<Dispatch<int, int>> driver_output_data;
  std::vector<Dispatch<int, double>> follower1_output_data;
  std::vector<Dispatch<int, std::string>> follower2_output_data;

  const auto result = Synchronizer::capture(
    std::forward_as_tuple(*driver, *follower1, *follower2),
    std::forward_as_tuple(
      std::back_inserter(driver_output_data),
      std::back_inserter(follower1_output_data),
      std::back_inserter(follower2_output_data)),
    100 /*guard*/);

  ASSERT_FALSE(result);
  ASSERT_EQ(result.state, State::ERROR_DRIVER_LOWER_BOUND_EXCEEDED);

  // Inputs may be captured, but capture is aborted after
  ASSERT_FALSE(driver_output_data.empty());
  ASSERT_TRUE(follower1_output_data.empty());
  ASSERT_TRUE(follower2_output_data.empty());
}


TEST_F(SynchronizerTestSuiteST, DryCaptureCannotPrimeRetry)
{
  const auto result = Synchronizer::dry_capture(std::forward_as_tuple(*driver, *follower1, *follower2), 0);

  ASSERT_FALSE(result);
  ASSERT_EQ(result.state, State::RETRY);
}


TEST_F(SynchronizerTestSuiteST, DryCaptureCannotPrimeAbort)
{
  driver->inject(Dispatch<int, int>{1, 1});
  follower1->inject(Dispatch<int, double>{7, 2.0});
  follower2->inject(Dispatch<int, std::string>{0, "ok"});

  const auto result = Synchronizer::dry_capture(std::forward_as_tuple(*driver, *follower1, *follower2), 0);

  ASSERT_FALSE(result);
  ASSERT_EQ(result.state, State::ABORT);
}


TEST_F(SynchronizerTestSuiteST, DryCaptureCanPrime)
{
  driver->inject(Dispatch<int, int>{10, 10});
  follower1->inject(Dispatch<int, double>{0, 2.0});
  follower1->inject(Dispatch<int, double>{9, 2.0});
  follower2->inject(Dispatch<int, std::string>{20, "ok"});

  const auto result = Synchronizer::dry_capture(std::forward_as_tuple(*driver, *follower1, *follower2), 0);

  ASSERT_TRUE(result);
  ASSERT_EQ(result.state, State::PRIMED);
}


TEST_F(SynchronizerTestSuiteST, DryCaptureDirectCaptureRange)
{
  follower1->inject(Dispatch<int, double>{0, 2.0});
  follower1->inject(Dispatch<int, double>{9, 2.0});
  follower2->inject(Dispatch<int, std::string>{20, "ok"});

  const auto result =
    Synchronizer::dry_capture(std::forward_as_tuple(CaptureRange<int>{10, 10}, *follower1, *follower2), 0);

  ASSERT_TRUE(result);
  ASSERT_EQ(result.state, State::PRIMED);
}

#endif  // DOXYGEN_SKIP

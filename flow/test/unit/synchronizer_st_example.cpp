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
#include <flow/drivers.h>
#include <flow/followers.h>
#include <flow/synchronizer.h>

using namespace flow;


// Type we will use to as a data sequencing stamp
using sequencing_type = int;


// Sequencing stamp + data
template <typename T> using MyDispatch = Dispatch<sequencing_type, T>;


TEST(Synchronizer, UsageExampleExampleSingleThreaded)
{
  using namespace flow;

  // Type aliases for captors
  using NextType = driver::Next<MyDispatch<int>, NoLock>;
  using ClosestType = follower::ClosestBefore<MyDispatch<double>, NoLock>;
  using BeforeType = follower::Before<MyDispatch<std::string>, NoLock>;

  // Setup driving captor
  NextType next_driver;

  // Setup first following input captor
  ClosestType closest_follower{1 /*period*/, 0 /*delay offset*/};

  // Setup second following input captor
  BeforeType before_follower{1 /*delay offset*/};

  // Add some data
  for (int n = 0; n < 20; ++n)
  {
    next_driver.inject(n, n);
    closest_follower.inject(n - 2, static_cast<double>(n) + 0.1234);
    before_follower.inject(n - 4, "flow" + std::to_string(n));
  }

  static constexpr std::size_t EXPECTED_SYNC_COUNT = 17;
  std::size_t sync_count = 0;

  // Synchronize
  bool working = true;
  while (working)
  {
    // Any iterable container can be used to capture data (even raw buffers!). Capturing through
    // iterators allows the end user to specify their own memory handling
    std::vector<MyDispatch<int>> next_driver_data;
    std::list<MyDispatch<double>> closest_follower_data;
    std::deque<MyDispatch<std::string>> before_follower_data;

    // Run data capture
    const auto result = Synchronizer::capture(
      std::forward_as_tuple(next_driver, closest_follower, before_follower),
      std::forward_as_tuple(
        std::back_inserter(next_driver_data),
        std::back_inserter(closest_follower_data),
        std::back_inserter(before_follower_data)));

    switch (result.state)
    {
    case State::PRIMED:
      // do stuff with synchronized data
      ++sync_count;
      break;
    case State::ABORT:
      // could not synchronize with current data
      break;
    case State::RETRY:
      // need newer data to synchronize
      working = false;
      break;
    case State::TIMEOUT:  // multithreading only
      break;
    default:
      break;
    }
  }

  // Start a clean slate, if you need to
  Synchronizer::reset(std::forward_as_tuple(next_driver, closest_follower, before_follower));

  ASSERT_EQ(next_driver.size(), 0UL);
  ASSERT_EQ(closest_follower.size(), 0UL);
  ASSERT_EQ(before_follower.size(), 0UL);

  ASSERT_EQ(sync_count, EXPECTED_SYNC_COUNT);
}

#endif  // DOXYGEN_SKIP

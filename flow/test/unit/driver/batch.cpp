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
#include <flow/captor/nolock.hpp>
#include <flow/driver/batch.hpp>

using namespace flow;
using namespace flow::driver;


struct DriverBatch : ::testing::Test, Batch<Dispatch<int, int>, NoLock>
{
  static constexpr std::size_t CHUNK_SIZE = 10;

  DriverBatch() : Batch<Dispatch<int, int>, NoLock>{CHUNK_SIZE} {}

  void SetUp() final { this->reset(); }
};
constexpr std::size_t DriverBatch::CHUNK_SIZE;


TEST_F(DriverBatch, CaptureRetryOnEmpty)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range;

  ASSERT_EQ(State::RETRY, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 0U);
}


TEST_F(DriverBatch, CaptureContinueLTBatchSize)
{
  // Start injecting data
  const int t0 = 0;
  int t = t0;
  int N = CHUNK_SIZE / 2;
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, 1});
    t += 1;
  }

  ASSERT_EQ(this->size(), CHUNK_SIZE / 2);

  // Start processing
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range;

  ASSERT_EQ(State::RETRY, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 0U);
}


TEST_F(DriverBatch, CapturePrimedEQBatchSize)
{
  // Start injecting data
  const int t0 = 0;
  int t = t0;
  int N = CHUNK_SIZE;
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, 1});
    t += 1;
  }


  // Start processing
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range;

  ASSERT_EQ(this->size(), CHUNK_SIZE);
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), CHUNK_SIZE - 1);

  ASSERT_EQ(data.size(), CHUNK_SIZE);
  EXPECT_EQ(t_range.lower_stamp, static_cast<int>(t0));
  EXPECT_EQ(t_range.upper_stamp, static_cast<int>(t0 + CHUNK_SIZE - 1));
}


TEST_F(DriverBatch, CapturePrimedGTBatchSize)
{
  // Start injecting data
  const int t0 = 0;
  int t = t0;
  int N = CHUNK_SIZE + CHUNK_SIZE / 2;
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, 1});
    t += 1;
  }

  // Start processing
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range;

  ASSERT_EQ(this->size(), CHUNK_SIZE + CHUNK_SIZE / 2);
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), CHUNK_SIZE + CHUNK_SIZE / 2 - 1);

  ASSERT_EQ(data.size(), CHUNK_SIZE);
  EXPECT_EQ(t_range.lower_stamp, static_cast<int>(t0));
  EXPECT_EQ(t_range.upper_stamp, static_cast<int>(t0 + CHUNK_SIZE - 1));
}


TEST_F(DriverBatch, DryCaptureRetryOnEmpty)
{
  CaptureRange<int> t_range;

  ASSERT_EQ(State::RETRY, this->dry_capture(t_range));
}


TEST_F(DriverBatch, DryCaptureContinueLTBatchSize)
{
  // Start injecting data
  const int t0 = 0;
  int t = t0;
  int N = CHUNK_SIZE / 2;
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, 1});
    t += 1;
  }

  ASSERT_EQ(this->size(), CHUNK_SIZE / 2);

  // Start processing
  CaptureRange<int> t_range;

  ASSERT_EQ(State::RETRY, this->dry_capture(t_range));
}


TEST_F(DriverBatch, DryCapturePrimedEQBatchSize)
{
  // Start injecting data
  const int t0 = 0;
  int t = t0;
  int N = CHUNK_SIZE;
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, 1});
    t += 1;
  }

  // Start processing
  CaptureRange<int> t_range;

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));

  EXPECT_EQ(t_range.lower_stamp, static_cast<int>(t0));
  EXPECT_EQ(t_range.upper_stamp, static_cast<int>(t0 + CHUNK_SIZE - 1));
}


TEST_F(DriverBatch, DryCapturePrimedGTBatchSize)
{
  // Start injecting data
  const int t0 = 0;
  int t = t0;
  int N = CHUNK_SIZE + CHUNK_SIZE / 2;
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, 1});
    t += 1;
  }

  // Start processing
  CaptureRange<int> t_range;

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));

  EXPECT_EQ(t_range.lower_stamp, static_cast<int>(t0));
  EXPECT_EQ(t_range.upper_stamp, static_cast<int>(t0 + CHUNK_SIZE - 1));
}


TEST_F(DriverBatch, RemovalOnAbort)
{
  // Start injecting data
  const int t0 = 0;
  int t = t0;
  int N = CHUNK_SIZE + 1;
  while (N--)
  {
    this->inject(Dispatch<int, int>{t, 1});
    t += 1;
  }

  this->abort(CHUNK_SIZE);

  ASSERT_EQ(this->size(), 1UL);
}

#endif  // DOXYGEN_SKIP
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
#include <flow/driver/chunk.hpp>

using namespace flow;
using namespace flow::driver;


struct DriverChunk : ::testing::Test, Chunk<Dispatch<int, int>, NoLock>
{
  static constexpr std::size_t CHUNK_SIZE = 10;

  DriverChunk() : Chunk<Dispatch<int, int>, NoLock>{CHUNK_SIZE} {}

  void SetUp() final { this->reset(); }
};
constexpr std::size_t DriverChunk::CHUNK_SIZE;


TEST_F(DriverChunk, CaptureRetryOnEmpty)
{
  std::vector<Dispatch<int, int>> data;
  CaptureRange<int> t_range{0, 0};

  ASSERT_EQ(State::RETRY, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 0U);
}


TEST_F(DriverChunk, CaptureContinueLTChunkSize)
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
  CaptureRange<int> t_range{0, 0};

  ASSERT_EQ(State::RETRY, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(data.size(), 0U);
}


TEST_F(DriverChunk, CapturePrimedEQChunkSize)
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
  CaptureRange<int> t_range{0, 0};

  ASSERT_EQ(this->size(), CHUNK_SIZE);
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), 0U);

  ASSERT_EQ(data.size(), CHUNK_SIZE);
  EXPECT_EQ(t_range.lower_stamp, static_cast<int>(t0));
  EXPECT_EQ(t_range.upper_stamp, static_cast<int>(t0 + CHUNK_SIZE - 1));
}


TEST_F(DriverChunk, CapturePrimedGTChunkSize)
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
  CaptureRange<int> t_range{0, 0};

  ASSERT_EQ(this->size(), CHUNK_SIZE + CHUNK_SIZE / 2);
  ASSERT_EQ(State::PRIMED, this->capture(std::back_inserter(data), t_range));
  ASSERT_EQ(this->size(), CHUNK_SIZE / 2);

  ASSERT_EQ(data.size(), CHUNK_SIZE);
  EXPECT_EQ(t_range.lower_stamp, static_cast<int>(t0));
  EXPECT_EQ(t_range.upper_stamp, static_cast<int>(t0 + CHUNK_SIZE - 1));
}


TEST_F(DriverChunk, DryCaptureRetryOnEmpty)
{
  CaptureRange<int> t_range{0, 0};

  ASSERT_EQ(State::RETRY, this->dry_capture(t_range));
}


TEST_F(DriverChunk, DryCaptureContinueLTChunkSize)
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
  CaptureRange<int> t_range{0, 0};

  ASSERT_EQ(State::RETRY, this->dry_capture(t_range));
}


TEST_F(DriverChunk, DryCapturePrimedEQChunkSize)
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
  CaptureRange<int> t_range{0, 0};

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));

  EXPECT_EQ(t_range.lower_stamp, static_cast<int>(t0));
  EXPECT_EQ(t_range.upper_stamp, static_cast<int>(t0 + CHUNK_SIZE - 1));
}


TEST_F(DriverChunk, DryCapturePrimedGTChunkSize)
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
  CaptureRange<int> t_range{0, 0};

  ASSERT_EQ(State::PRIMED, this->dry_capture(t_range));

  EXPECT_EQ(t_range.lower_stamp, static_cast<int>(t0));
  EXPECT_EQ(t_range.upper_stamp, static_cast<int>(t0 + CHUNK_SIZE - 1));
}


TEST_F(DriverChunk, RemovalOnAbort)
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

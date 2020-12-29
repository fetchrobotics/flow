/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 */
#ifndef DOXYGEN_SKIP

// C++ Standard Library
#include <deque>

// GTest
#include <gtest/gtest.h>

// Flow
#include <flow/dispatch_queue.h>

using namespace flow;


TEST(DispatchQueue, DefaultIsEmpty)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  EXPECT_TRUE(queue.empty());
}

TEST(DispatchQueue, DefaultIteratorsEqual)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  ASSERT_TRUE(queue.empty());
  EXPECT_TRUE(queue.begin() == queue.end());
}


TEST(DispatchQueue, Insert)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  const int t0 = 0;

  queue.insert(DispatchType{t0, 1});

  ASSERT_FALSE(queue.empty());
  EXPECT_EQ(queue.size(), 1u);
}


TEST(DispatchQueue, InsertDispatchType)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  const int t0 = 0;

  queue.insert(DispatchType{t0, 1});

  ASSERT_FALSE(queue.empty());
  EXPECT_EQ(queue.size(), 1u);
}


TEST(DispatchQueue, OldestStamp)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  const int t_oldest{0};

  queue.insert(DispatchType{t_oldest, 1});
  queue.insert(DispatchType{t_oldest + 1, 2});
  queue.insert(DispatchType{t_oldest + 2, 3});

  ASSERT_FALSE(queue.empty());
  EXPECT_EQ(queue.oldest_stamp(), t_oldest);
}


TEST(DispatchQueue, NewestStamp)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  const int t_oldest{0};

  queue.insert(DispatchType{t_oldest, 1});
  queue.insert(DispatchType{t_oldest + 1, 2});
  queue.insert(DispatchType{t_oldest + 2, 3});

  ASSERT_FALSE(queue.empty());
  EXPECT_EQ(queue.newest_stamp(), t_oldest + 2);
}


TEST(DispatchQueue, ShrinkToFitSmallerThanOriginal)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  const int t0 = 0;
  const int t1 = t0 + 1;
  const int t2 = t1 + 1;

  queue.insert(DispatchType{t0, 1});
  queue.insert(DispatchType{t1, 1});
  queue.insert(DispatchType{t2, 1});

  queue.shrink_to_fit(1);

  EXPECT_EQ(queue.size(), 1u);
}


TEST(DispatchQueue, ShrinkToFitLargerThanOriginal)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  const int t0 = 0;
  const int t1 = t0 + 1;
  const int t2 = t1 + 1;

  queue.insert(DispatchType{t0, 1});
  queue.insert(DispatchType{t1, 1});
  queue.insert(DispatchType{t2, 1});

  queue.shrink_to_fit(4);

  EXPECT_EQ(queue.size(), 3u);
}


TEST(DispatchQueue, RemoveNone)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  const int t0 = 0;
  const int t1 = t0 + 1;
  const int t2 = t1 + 1;

  queue.insert(DispatchType{t0, 1});
  queue.insert(DispatchType{t1, 1});
  queue.insert(DispatchType{t2, 1});

  queue.remove_before(t0);

  EXPECT_EQ(queue.size(), 3u);
}


TEST(DispatchQueue, RemoveAll)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  const int t0 = 0;
  const int t1 = t0 + 1;
  const int t2 = t1 + 1;

  queue.insert(DispatchType{t0, 1});
  queue.insert(DispatchType{t1, 1});
  queue.insert(DispatchType{t2, 1});

  queue.remove_before(t2 + 1);

  EXPECT_TRUE(queue.empty());
}


TEST(DispatchQueue, InsertDuplicateTimeBehavior)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  const int t0 = 0;

  queue.insert(DispatchType{t0, 1});
  queue.insert(DispatchType{t0, 2});
  queue.insert(DispatchType{t0, 3});

  ASSERT_EQ(queue.size(), 1u);

  const auto p0 = queue.pop();

  EXPECT_EQ(get_stamp(p0), t0);
  EXPECT_EQ(get_value(p0), 1);
}


TEST(DispatchQueue, InsertOrdered)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  const int t0 = 0;
  const int t1 = t0 + 1;
  const int t2 = t1 + 1;

  queue.insert(DispatchType{t0, 1});
  queue.insert(DispatchType{t1, 1});
  queue.insert(DispatchType{t2, 1});

  EXPECT_EQ(queue.size(), 3u);

  const auto p0 = queue.pop();
  const auto p1 = queue.pop();
  const auto p2 = queue.pop();

  EXPECT_LT(get_stamp(p0), get_stamp(p1));
  EXPECT_LT(get_stamp(p1), get_stamp(p2));
}


TEST(DispatchQueue, InsertUnordered)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  const int t0 = 0;
  const int t1 = t0 + 1;
  const int t2 = t1 + 1;

  queue.insert(DispatchType{t0, 1});
  queue.insert(DispatchType{t2, 1});
  queue.insert(DispatchType{t1, 1});

  EXPECT_EQ(queue.size(), 3u);

  const auto p0 = queue.pop();
  const auto p1 = queue.pop();
  const auto p2 = queue.pop();

  EXPECT_LT(get_stamp(p0), get_stamp(p1));
  EXPECT_LT(get_stamp(p1), get_stamp(p2));
}


TEST(DispatchQueue, BeforeItrEmpty)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  ASSERT_EQ(queue.before(6), queue.end());
}


TEST(DispatchQueue, BeforeItr)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  queue.insert(DispatchType{5, 8});
  queue.insert(DispatchType{6, 9});
  queue.insert(DispatchType{7, 10});

  ASSERT_EQ(queue.before(6)->stamp, 5);
  ASSERT_EQ(queue.before(7)->stamp, 6);
}


TEST(DispatchQueue, ReverseBeforeItrEmpty)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  ASSERT_EQ(queue.rbefore(6), queue.rend());
}


TEST(DispatchQueue, ReverseBeforeItr)
{
  using DispatchType = Dispatch<int, int>;

  DispatchQueue<DispatchType, std::deque<DispatchType>> queue;

  queue.insert(DispatchType{5, 8});
  queue.insert(DispatchType{6, 9});
  queue.insert(DispatchType{7, 10});

  ASSERT_EQ(queue.rbefore(6)->stamp, 5);
  ASSERT_EQ(queue.rbefore(7)->stamp, 6);
}

#endif  // DOXYGEN_SKIP

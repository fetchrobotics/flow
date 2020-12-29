/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 */
#ifndef FLOW_UTILITY_STATIC_ASSERT_HPP
#define FLOW_UTILITY_STATIC_ASSERT_HPP

/**
 * @brief Macro which wrapper static_assert with extra formatting
 *
 * @param condition  static condition
 * @param message  message to print on condition failure
 */
#define FLOW_STATIC_ASSERT(condition, message) static_assert(condition, "\n\n--->\n\n" message "\n\n<---\n\n")

#endif  // FLOW_UTILITY_STATIC_ASSERT_HPP

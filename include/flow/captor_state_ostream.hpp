/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 */
#ifndef FLOW_CAPTOR_STATE_OSTREAM_HPP
#define FLOW_CAPTOR_STATE_OSTREAM_HPP

// C++ Standard Library
#include <ostream>

// Flow
#include <flow/captor_state.hpp>

namespace flow
{

/**
 * @brief Output stream overload for <code>State</code> codes
 * @param[in,out] os  output stream
 * @param state  state code
 * @return os
 */
inline std::ostream& operator<<(std::ostream& os, const State state)
{
  switch (state)
  {
  case State::RETRY:
    return os << "RETRY";
  case State::PRIMED:
    return os << "PRIMED";
  case State::ABORT:
    return os << "ABORT";
  case State::TIMEOUT:
    return os << "TIMEOUT";
  case State::ERROR_DRIVER_LOWER_BOUND_EXCEEDED:
    return os << "ERROR_DRIVER_LOWER_BOUND_EXCEEDED";
  case State::SKIP_FRAME_QUEUE_PRECONDITION:
    return os << "SKIP_FRAME_QUEUE_PRECONDITION";
  default:
    return os;
  }
}

}  // namespace flow

#endif  // FLOW_CAPTOR_STATE_OSTREAM_HPP

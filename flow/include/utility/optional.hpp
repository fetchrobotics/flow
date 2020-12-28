/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 *
 * @file optional.hpp
 */

#ifndef FLOW_UTILITY_OPTIONAL_HPP
#define FLOW_UTILITY_OPTIONAL_HPP

// C++ Standard Library
#include <type_traits>
#include <utility>

namespace flow
{

/**
 * @brief The class template std::optional manages an optional contained value, i.e. a value that may or may not be
 * present
 *
 *        See: https://en.cppreference.com/w/cpp/utility/optional
 * \n
 *        Partial implementation drop-in of C++17 <code>std::optional</code>
 *
 * @tparam T  value type of object to hold. <code>T</code> may be
 *            <code>const</code> or non-defaultable
 */
template <typename T> class optional
{
  /// T with const qualifier removed
  using T_no_const = std::remove_const_t<T>;

public:
  /**
   * @brief default constructor
   *
   * @note invalid by default
   */
  optional() : valid_{false} {}

  /**
   * @brief value constructor
   *
   *        automatically sets optional to valid
   *
   * @tparam ReqArgT   argument to construct <code>T</code>
   * @tparam ArgsPack..other arguments to construct <code>T</code>
   *
   * @throws anything that <code>T::T</code> can throw
   */
  template <typename ReqArgT, typename... ArgsPack> optional(ReqArgT&& arg, ArgsPack&&... other_args) : valid_{true}
  {
    new (vptr()) T(std::forward<ReqArgT>(arg), std::forward<ArgsPack>(other_args)...);
  }

  /**
   * @brief copy constructor for optional
   * @throws anything that <code>T::T</code> can throw
   */
  optional(const optional<T>& other) : valid_{other.valid_}
  {
    if (other)
    {
      new (vptr()) T(other.value());
    }
  }

  /**
   * @brief move constructor for optional
   *
   *       sets other optional to invalid state
   *
   * @throws anything that <code>T::T</code> can throw
   */
  optional(optional<T>&& other) : valid_{other.valid_}
  {
    if (other)
    {
      new (vptr()) T(std::move(other.value()));

      // Destroy other
      other.reset();
    }
  }

  ~optional() { destroy(); }

  /**
   * @brief move/value assignment
   *
   *        automatically sets optional to valid
   *
   * @tparam AssignT...  (deduced) type which is assignable to <code>T</code>
   *
   * @throws anything that <code>T::T</code> can throw
   */
  template <typename AssignT> inline optional& operator=(AssignT&& other)
  {
    // Clean up old value
    destroy();

    // Reconstruct self with new value(s)
    new (this) optional<T>(std::forward<AssignT>(other));

    return *this;
  }

  /**
   * @brief copy assignment
   *
   *        automatically sets optional to valid
   *
   * @tparam CopyT...  (deduced) type which is copyable into <code>T</code>
   *
   * @throws anything that <code>T::T</code> can throw
   */
  inline optional& operator=(const optional<T>& other)
  {
    // Clean up old value
    destroy();

    // Reconstruct self with values copied from other
    new (this) optional<T>(other);

    return *this;
  }

  /**
   * @brief allows conversion to bool for conditional checks
   *
   * @note cannot be used in arithmetic operations
   */
  inline explicit operator bool() const { return valid_; }

  /**
   * @brief Pointer-like access to underlying value
   */
  inline T& operator*() { return value(); }

  /**
   * @brief Pointer-like access to underlying value (const)
   */
  inline const T& operator*() const { return value(); }

  /**
   * @brief Resets optional to no-value
   *
   * @note Does not call underlying destructor
   */
  inline void reset()
  {
    destroy();
    valid_ = false;
  }

private:
  /**
   * @brief Calls deconstructor if optional object is set
   *
   * @note participates in overload resolution if <code>T</code> can be trivially destroyed,
   *       e.g. held object is a POD type
   */
  template <bool U = std::is_trivially_destructible<T>::value> constexpr std::enable_if_t<U> destroy() {}

  /**
   * @brief Calls deconstructor if optional object is set
   *
   * @note participates in overload resolution if <code>T</code> cannot be trivially destroyed
   */
  template <bool U = std::is_trivially_destructible<T>::value> inline std::enable_if_t<!U> destroy()
  {
    if (valid_)
    {
      vptr()->~T();
    }
  }

  /**
   * @brief Returns pointer to value memory
   */
  inline const T_no_const* vptr() const { return reinterpret_cast<const T_no_const*>(&storage_); }

  /**
   * @brief Returns pointer to value memory
   */
  inline T_no_const* vptr() { return reinterpret_cast<T_no_const*>(&storage_); }

  /**
   * @brief Returns reference to value
   */
  inline const T& value() const { return *vptr(); }

  /**
   * @brief Returns reference to value
   */
  inline T& value() { return *vptr(); }

  /// Whether the stored value is valid
  bool valid_;

  /// The stored possibly valid value
  std::aligned_storage_t<sizeof(T), alignof(T)> storage_;
};

}  // namespace flow

#endif  // FLOW_UTILITY_OPTIONAL_HPP

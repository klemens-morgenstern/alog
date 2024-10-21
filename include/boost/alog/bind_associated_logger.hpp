//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ALOG_BIND_ASSOCIATED_LOGGER_HPP
#define BOOST_ALOG_BIND_ASSOCIATED_LOGGER_HPP

#include <boost/asio/associator.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/detail/initiation_base.hpp>

namespace boost
{
namespace alog
{


template <typename T, typename Logger>
class logger_binder
{
  Logger logger_;
  T target_;
 public:

  typedef T target_type;
  typedef Logger logger_type;


  template <typename U>
  logger_binder(const logger_type& l, U&& u)
      : logger_(l),
        target_(static_cast<U&&>(u))
  {
  }

  /// Obtain a reference to the target object.
  target_type& get() noexcept
  {
    return target_;
  }

  /// Obtain a reference to the target object.
  const target_type& get() const noexcept
  {
    return target_;
  }

  /// Obtain the associated logger.
  logger_type get_logger() const noexcept
  {
    return logger_;
  }

  /// Forwarding function call operator.
  template <typename... Args>
  boost::asio::result_of_t<T(Args...)> operator()(Args&&... args)
  {
    return target_(static_cast<Args&&>(args)...);
  }

  /// Forwarding function call operator.
  template <typename... Args>
  boost::asio::result_of_t<T(Args...)> operator()(Args&&... args) const
  {
    return target_(static_cast<Args&&>(args)...);
  }

};

template<typename Logger, typename T>
inline logger_binder<std::decay_t<T>, Logger>
bind_logger(const Logger & log, T && t)
{
  return logger_binder< std::decay_t<T>, Logger>(log, std::forward<T>(t));
}


template <typename T, typename Logger, typename Logger1>
struct associated_logger<logger_binder<T, Logger>, Logger1>
{
  typedef Logger type;

  static auto get(const logger_binder<T, Logger>& b,
                  const Logger1& = Logger1()) noexcept
  -> decltype(b.get_logger())
  {
    return b.get_logger();
  }
};


namespace detail {

template <typename TargetAsyncResult, typename Logger, typename = void>
class logger_binder_completion_handler_async_result
{
 public:
  template <typename T>
  explicit logger_binder_completion_handler_async_result(T&)
  {
  }
};

template <typename TargetAsyncResult, typename Logger>
class logger_binder_completion_handler_async_result<
    TargetAsyncResult, Logger,
    void_t<typename TargetAsyncResult::completion_handler_type>>
{
 private:
  TargetAsyncResult target_;

 public:
  typedef logger_binder<
      typename TargetAsyncResult::completion_handler_type, Logger>
      completion_handler_type;

  explicit logger_binder_completion_handler_async_result(
      typename TargetAsyncResult::completion_handler_type& handler)
      : target_(handler)
  {
  }

  auto get() -> decltype(target_.get())
  {
    return target_.get();
  }
};

template <typename TargetAsyncResult, typename = void>
struct logger_binder_async_result_return_type
{
};

template <typename TargetAsyncResult>
struct logger_binder_async_result_return_type<
    TargetAsyncResult, boost::asio::void_type<typename TargetAsyncResult::return_type>>
{
  typedef typename TargetAsyncResult::return_type return_type;
};

} // namespace detail

}


namespace asio
{


template <template <typename, typename> class Associator,
    typename T, typename Logger, typename DefaultCandidate>
struct associator<Associator,  ::boost::alog::logger_binder<T, Logger>, DefaultCandidate>
: Associator<T, DefaultCandidate>
{
static typename Associator<T, DefaultCandidate>::type get(
    const  ::boost::alog::logger_binder<T, Logger>& b) noexcept
{
  return Associator<T, DefaultCandidate>::get(b.get());
}

static auto get(const ::boost::alog::logger_binder<T, Logger>& b,
                const DefaultCandidate& c) noexcept
-> decltype(Associator<T, DefaultCandidate>::get(b.get(), c))
{
  return Associator<T, DefaultCandidate>::get(b.get(), c);
}
};

template <typename T, typename Logger, typename Signature>
class async_result<::boost::alog::logger_binder<T, Logger>, Signature> :
 public ::boost::alog::detail::logger_binder_completion_handler_async_result<
     async_result<T, Signature>, Logger>,
  public ::boost::alog::detail::logger_binder_async_result_return_type<
     async_result<T, Signature>>
{
  public:
  explicit async_result( ::boost::alog::logger_binder<T, Logger>& b)
  :  ::boost::alog::detail::logger_binder_completion_handler_async_result<
      boost::asio::async_result<T, Signature>, Logger>(b.get())
  {
  }

  template <typename Initiation>
  struct init_wrapper : ::boost::asio::detail::initiation_base<Initiation>
  {
    using detail::initiation_base<Initiation>::initiation_base;

    template <typename Handler, typename... Args>
    void operator()(Handler&& handler, const Logger& a, Args&&... args) &&
    {
      static_cast<Initiation&&>(*this)(
          ::boost::alog::logger_binder<decay_t<Handler>, Logger>(
              a, static_cast<Handler&&>(handler)),
          static_cast<Args&&>(args)...);
    }

    template <typename Handler, typename... Args>
    void operator()(Handler&& handler,
                    const Logger& a, Args&&... args) const &
    {
      static_cast<const Initiation&>(*this)(
          ::boost::alog::logger_binder<decay_t<Handler>, Logger>(
              a, static_cast<Handler&&>(handler)),
          static_cast<Args&&>(args)...);
    }
  };

  template <typename Initiation, typename RawCompletionToken, typename... Args>
  static auto initiate(Initiation&& initiation,
                       RawCompletionToken&& token, Args&&... args)
  -> decltype(
  async_initiate<
      conditional_t<
          is_const<remove_reference_t<RawCompletionToken>>::value, const T, T>,
      Signature>(
      declval<init_wrapper<decay_t<Initiation>>>(),
      token.get(), token.get_logger(), static_cast<Args&&>(args)...))
  {
    return async_initiate<
        conditional_t<
            is_const<remove_reference_t<RawCompletionToken>>::value, const T, T>,
        Signature>(
        init_wrapper<decay_t<Initiation>>(
        static_cast<Initiation&&>(initiation)),
        token.get(), token.get_logger(), static_cast<Args&&>(args)...);
  }

 private:
  async_result(const async_result&) = delete;
  async_result& operator=(const async_result&) = delete;

  async_result<T, Signature> target_;
};


}
}

#endif //BOOST_ALOG_BIND_ASSOCIATED_LOGGER_HPP

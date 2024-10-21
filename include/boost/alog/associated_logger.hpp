//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ALOG_ASSOCIATED_LOGGER_HPP
#define BOOST_ALOG_ASSOCIATED_LOGGER_HPP

#include <boost/alog/null_logger.hpp>


#include <boost/asio/associator.hpp>

namespace boost
{
namespace alog
{


template <typename T, typename logger>
struct associated_logger;



template <typename T, typename logger>
struct associated_logger;


namespace detail {

template <typename T, typename = void>
struct has_logger_type : std::false_type
{
};

template<typename >
using void_t = void;

template <typename T>
struct has_logger_type<T, void_t<typename T::logger_type>>
    : std::true_type
{
};

template <typename T, typename E, typename = void, typename = void>
struct associated_logger_impl
{
  typedef void ssh_asio_associated_logger_is_unspecialised;

  typedef E type;

  static type get(const T&) noexcept
  {
    return type();
  }

  static const type& get(const T&, const E& e) noexcept
  {
    return e;
  }

};

template <typename T, typename E>
struct associated_logger_impl<T, E, void_t<typename T::logger_type>>
{
  typedef typename T::logger_type type;

  static auto get(const T& t) noexcept
  -> decltype(t.get_logger())
  {
    return t.get_logger();
  }

  static auto get(const T& t, const E&) noexcept
  -> decltype(t.get_logger())
  {
    return t.get_logger();
  }

};

template <typename T, typename E>
struct associated_logger_impl<T, E,
    std::enable_if_t<
        !has_logger_type<T>::value
    >,
    void_t<
        typename boost::asio::associator<associated_logger_impl, T, E>::type
    >> : boost::asio::associator<associated_logger_impl, T, E>
{

};

} // namespace detail


template <typename T, typename Logger = null_logger>
struct associated_logger
    : ::boost::alog::detail::associated_logger_impl<T, Logger>
{
};

template <typename T>
[[nodiscard]] inline typename associated_logger<T>::type
get_associated_logger(const T& t) noexcept
{
  return associated_logger<T>::get(t);
}

template <typename T, typename logger>
[[nodiscard]] inline auto get_associated_logger(
    const T& t, const logger& ex) noexcept
-> decltype(associated_logger<T, logger>::get(t, ex))
{
  return associated_logger<T, logger>::get(t, ex);
}

template <typename T, typename Executor = null_logger>
using associated_logger_t = typename associated_logger<T, Executor>::type;

template<typename T>
using has_associated_logger = typename associated_logger_t<T>::has_associated_logger;


}
}

#endif //BOOST_ALOG_ASSOCIATED_LOGGER_HPP

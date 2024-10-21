//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ALOG_NULL_LOGGER_HPP
#define BOOST_ALOG_NULL_LOGGER_HPP

#include <chrono>
#include <string_view>

#include <boost/assert/source_location.hpp>

namespace boost
{
namespace alog
{

struct null_logger
{
  enum level {none = 0, error = 0, trace = 0, warn = 0, info = 0, debug = 0, critical = 0};

  constexpr static level level() {return none;}
  // only her if we don't have if constexpr
  void log(enum level, std::string_view message, std::chrono::steady_clock::time_point = std::chrono::steady_clock::now(), const boost::source_location & loc = BOOST_CURRENT_LOCATION) {}
};

}
}

#endif //BOOST_ALOG_NULL_LOGGER_HPP

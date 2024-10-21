//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ALOG_PRINTF_LOGGER_HPP
#define BOOST_ALOG_PRINTF_LOGGER_HPP

#include <chrono>
#include <string_view>

#include <boost/assert/source_location.hpp>

namespace boost
{
namespace alog
{

struct printf_logger
{
  FILE* out = stderr;


  enum level  {none = 0, trace, debug, info, warn,  error, critical, };

  level current_level{warn};
  level level() const {return current_level;}

  void log    (enum level lvl, std::string_view message, std::chrono::system_clock::time_point tp = std::chrono::system_clock::now(), const boost::source_location & loc = BOOST_CURRENT_LOCATION)
  {

    std::time_t ttp = std::chrono::system_clock::to_time_t(tp);
    char buffer[26];
    strftime(buffer, sizeof(buffer), "%F %T", localtime(&ttp));

    const char * lv[] = {"none", "trace", "debug", "info", "warn", "error", "critical"};
    fprintf(out, "[%s] [%s] %s(%d): %.*s\n",
            buffer, lv[lvl], loc.file_name(), loc.line(),
            (int)message.size(), message.data());
  }
};

}
}


#endif //BOOST_ALOG_PRINTF_LOGGER_HPP

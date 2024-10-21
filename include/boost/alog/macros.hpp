//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ALOG_MACROS_HPP
#define BOOST_ALOG_MACROS_HPP

#include <boost/config.hpp>


#define BOOST_ALOG(Handler, Level, Message) \
do {                                                                                \
    using _alog_logger_type = typename ::boost::alog::associated_logger<std::decay_t<decltype(Handler)>>::type;  \
    BOOST_IF_CONSTEXPR (_alog_logger_type::Level != _alog_logger_type::none)        \
    {                                                                               \
      auto _alogger = ::boost::alog::get_associated_logger(Handler);                \
      if (_alog_logger_type::Level >= _alogger.level())                     \
        _alogger.log(_alog_logger_type::Level, Message);                            \
    } \
} \
while (false)

#endif //BOOST_ALOG_MACROS_HPP

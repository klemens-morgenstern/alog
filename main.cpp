#include <iostream>

#include <boost/alog.hpp>

#include <boost/asio/detached.hpp>
#include <boost/asio/cancel_after.hpp>
#include <boost/asio/compose.hpp>
#include <boost/asio/steady_timer.hpp>

struct async_foo_implementation
{
  boost::asio::steady_timer & timer;

  template <typename Self>
  void operator()(Self& self)
  {
    BOOST_ALOG(self, info, "starting");
    timer.async_wait(std::move(self));
  }

  template <typename Self>
  void operator()(Self & self, boost::system::error_code ec)
  {
    if (ec)
      BOOST_ALOG(self, error, ec.message());
    else
      BOOST_ALOG(self, info, "success");
  }


};

template <typename CompletionToken>
auto async_foo(boost::asio::steady_timer & timer, CompletionToken&& token)
{
  return boost::asio::async_compose<CompletionToken,
      void(boost::system::error_code)>(
      async_foo_implementation{timer},
      token, timer);
}


int main()
{
  namespace asio = boost::asio;

  asio::io_context ctx;

  asio::steady_timer timer{ctx};
  async_foo(timer, boost::alog::bind_logger(boost::alog::printf_logger {stderr, boost::alog::printf_logger::trace}, asio::detached));

  asio::steady_timer timer2{ctx, std::chrono::steady_clock::time_point::max()};
  async_foo(timer2,

            boost::asio::cancel_after(
                std::chrono::seconds(1),
                boost::alog::bind_logger(
                    boost::alog::printf_logger{stdout, boost::alog::printf_logger::trace},
                    asio::detached)
            ));

  ctx.run();

  return 0;
}

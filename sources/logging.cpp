// Copyright 2022 cadeusept abobus1488822@yandex.ru


#include "logging.hpp"

void logs::logInFile() {
  logging::add_file_log
      (
          keywords::file_name = "/log/info.log",
          keywords::rotation_size = 256 * 1024 * 1024,
          keywords::time_based_rotation =
              sinks::file::rotation_at_time_point(0, 0, 0),
          keywords::filter =
              logging::trivial::severity
              >= logging::trivial::info,
          keywords::format =
              (
                  expr::stream
                  << boost::posix_time
                  ::second_clock::local_time()
                  << " : <" << logging
                  ::trivial::severity
                  << "> " << expr::smessage));

  logging::add_file_log
      (
          keywords::file_name = "/log/trace.log",
          keywords::rotation_size = 256 * 1024 * 1024,
          keywords::time_based_rotation = sinks::file
          ::rotation_at_time_point(0, 0, 0),
          keywords::filter = logging::trivial::severity
                             >= logging::trivial::trace,
          keywords::format =
              (
                  expr::stream
                  << boost::posix_time
                  ::second_clock::local_time()
                  << " : <" << logging::
                                   trivial::severity
                  << "> " << expr::smessage));
}

void logs::logInfo(const std::string &key, const std::string &hash) {
  BOOST_LOG_TRIVIAL(info) << "Thread with ID: "
                          << boost::this_thread::get_id() << " Key: "
                          << key << " Hash: " << hash << std::endl;
}

void logs::logTrace(const std::string &key, const std::string &value) {
  BOOST_LOG_TRIVIAL(trace) << "Thread with ID: "
                           << boost::this_thread::get_id() << " Key: "
                           << key << " Value: " << value << std::endl;
}

// Copyright 2022 cadeusept abobus1488822@yandex.ru

#ifndef INCLUDE_LOGGING_HPP_
#define INCLUDE_LOGGING_HPP_

#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks.hpp>
#include <string>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;

namespace logs{
    void logInFile();
    void logInfo(const std::string &key, const std::string &hash);
    void logTrace(const std::string &key, const std::string &value);
}

#endif  // INCLUDE_LOGGING_HPP_

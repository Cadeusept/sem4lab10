// Copyright 2021 Your Name <your_email>

#ifndef INCLUDE_HASHING_DB_HPP_
#define INCLUDE_HASHING_DB_HPP_

#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/slice.h>
#include <boost/filesystem.hpp>
#include <boost/unordered_map.hpp>

#include <cassert>
#include <list>
#include <mutex>
#include <thread>
#include <iostream>

using FamContainer =
    std::list<std::unique_ptr<rocksdb::ColumnFamilyHandle>>;
using FamDescContainer =
    std::vector<rocksdb::ColumnFamilyDescriptor>;
using FamHandlerContainer =
    std::list<std::unique_ptr<rocksdb::ColumnFamilyHandle>>;
using StrContainer = boost::unordered_map<std::string, std::string>;

void copyDB(std::string source_path, std::string dest_path);

class DBhasher {
 private:
  std::string _path;
  std::string _logLVL;
  std::unique_ptr<rocksdb::DB> _db;
  std::size_t _threadCountHash = std::thread::hardware_concurrency();
  std::mutex _mutex;
 public:
  DBhasher(std::string path,
           std::size_t threadCount,
           std::string logLVL) :
           _path(path),
           _logLVL(logLVL),
           _threadCountHash(threadCount){}

  FamHandlerContainer openDB(const FamDescContainer &descriptors);

  FamDescContainer getFamilyDescriptors();

  StrContainer getStrs(rocksdb::ColumnFamilyHandle *);

  void setupHash(FamHandlerContainer *, std::list<StrContainer> *);

  void writeHash(rocksdb::ColumnFamilyHandle *, StrContainer);

  void startThreads();
};

#endif // INCLUDE_HASHING_DB_HPP_

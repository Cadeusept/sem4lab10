// Copyright 2021 Your Name <your_email>

#ifndef INCLUDE_HASHING_DB_HPP_
#define INCLUDE_HASHING_DB_HPP_

#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/slice.h>
#include <boost/unordered_map.h>

#include <cassert>
#include <list>
#include <mutex>
#include <thread>

using FamContainer =
    std::list<std::unique_ptr<rocksdb::ColumnFamilyHandle>>;
using FamDescContainer =
    std::vector<rocksdb::ColumnFamilyDescriptor>;
using FamHandlerContainer =
    std::list<std::unique_ptr<rocksdb::ColumnFamilyHandle>>;
using StrContainer = boost::unordered_map<std::string, std::string>;

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

  FamDescContainer getFamilyDescriptors();

  FamHandlerContainer openDB(const FamDescContainer &descriptors);

  StrContainer getStrs(rocksdb::ColumnFamilyHandle *);

  void getHash(rocksdb::ColumnFamilyHandle *, StrContainer);

  void startHash(FamHandlerContainer *, std::list<StrContainer> *);

  void startThreads();
};

#endif // INCLUDE_HASHING_DB_HPP_

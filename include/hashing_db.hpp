// Copyright 2021 Your Name <your_email>

#ifndef INCLUDE_HASHING_DB_HPP_
#define INCLUDE_HASHING_DB_HPP_

#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/slice.h>
#include <boost/filesystem.hpp>
#include <boost/unordered_map.hpp>
#include "../third-party/ThreadPool/ThreadPool.h"

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

class DBhasher {
 public:
  std::string _path;
  std::string _logLVL;
  std::unique_ptr<rocksdb::DB> _db;
  std::size_t _threadCount = std::thread::hardware_concurrency();
 public:
  DBhasher(std::string path,
           std::size_t threadCount,
           std::string logLVL) :
           _path(path),
           _logLVL(logLVL),
           _threadCount(threadCount){}
};

FamDescContainer getFamilyDescriptors(std::string path);

FamHandlerContainer openDB(const FamDescContainer &descriptors,
                           std::string path,
                           std::unique_ptr<rocksdb::DB>& db);

FamDescContainer getFamilyDescriptors(std::string path);

StrContainer getStrs(rocksdb::ColumnFamilyHandle *family,
                     std::unique_ptr<rocksdb::DB>& db);

void setupHash
    (FamHandlerContainer *handlers,
     std::list <StrContainer> *StrContainerList,
     std::unique_ptr<rocksdb::DB>& db,
     std::unique_ptr<std::mutex>& thread_mutex);

void writeHash
    (rocksdb::ColumnFamilyHandle *family, StrContainer strContainer,
     std::unique_ptr<rocksdb::DB>& db);

void startThreads(std::string path, std::unique_ptr<rocksdb::DB>& db,
                  std::size_t threadCount,
                  std::unique_ptr<std::mutex>& thread_mutex);

void copyDB(DBhasher& source_db, DBhasher& dest_db,
            std::unique_ptr<std::mutex>& thread_mutex);

#endif // INCLUDE_HASHING_DB_HPP_

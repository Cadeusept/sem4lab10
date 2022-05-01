// Copyright 2021 Your Name <your_email>

#include <hashing_db.hpp>
#include <logging.hpp>

#include "../third-party/picosha2.h"

std::mutex console_mutex;

FamHandlerContainer openDB(const FamDescContainer &descriptors,
                                     std::string path,
                                     std::unique_ptr<rocksdb::DB>& db) {
    FamHandlerContainer handlers;
    std::vector < rocksdb::ColumnFamilyHandle * > newHandles;
    rocksdb::DB *dbStrPtr;

    rocksdb::Status status =
    rocksdb::DB::Open(
          rocksdb::DBOptions(),
          path,
          descriptors,
          &newHandles,
          &dbStrPtr);

    assert(status.ok());

    db.reset(dbStrPtr);

    for (rocksdb::ColumnFamilyHandle *ptr : newHandles) {
        handlers.emplace_back(ptr);
    }

    return handlers;
}

FamDescContainer getFamilyDescriptors(std::string path) {
    rocksdb::Options options;

    std::vector <std::string> family;
    FamDescContainer descriptors;
    rocksdb::Status status =
        rocksdb::DB::ListColumnFamilies(rocksdb::DBOptions(),
                                        path,
                                        &family);
    assert(status.ok());

  for (const std::string &familyName : family) {
    descriptors.emplace_back(familyName,
                             rocksdb::ColumnFamilyOptions());
  }
  return descriptors;
}

StrContainer getStrs(rocksdb::ColumnFamilyHandle *family,
                     std::unique_ptr<rocksdb::DB>& db) {
  boost::unordered_map <std::string, std::string> dbCase;
  std::unique_ptr <rocksdb::Iterator>
      it(db->NewIterator(rocksdb::ReadOptions(),
                             family));

  for (it->SeekToFirst(); it->Valid(); it->Next()) {
      std::string key = it->key().ToString();
      std::string value = it->value().ToString();
      dbCase[key] = value;
  }

  return dbCase;
}

void setupHash
    (FamHandlerContainer *handlers,
     std::list <StrContainer> *StrContainerList,
     std::unique_ptr<rocksdb::DB>& db,
     std::unique_ptr<std::mutex>& thread_mutex) {
    while (!handlers->empty()) {
        thread_mutex->lock();

        if (handlers->empty()) {
            thread_mutex->unlock();
            continue;
        }

        auto &family = handlers->front();
        handlers->pop_front();

        StrContainer str_container = StrContainerList->front();
        StrContainerList->pop_front();

        thread_mutex->unlock();

        writeHash(family.get(), str_container, db);
    }
}

void writeHash
        (rocksdb::ColumnFamilyHandle *family, StrContainer strContainer,
          std::unique_ptr<rocksdb::DB>& db) {
    for (auto it = strContainer.begin(); it != strContainer.end(); ++it)
    {
        std::string hash = picosha2::hash256_hex_string(it->first +
                                                            it->second);
        console_mutex.lock();
        std::cout << "key: " << it->first << " hash: " << hash << std::endl;
        console_mutex.unlock();

        logs::logInfo(it->first, hash);
        rocksdb::Status status = db->Put(rocksdb::WriteOptions(),
                                          family,
                                          it->first,
                                          hash);
        assert(status.ok());
    }
}

void startThreads(std::string path, std::unique_ptr<rocksdb::DB> db,
                  std::size_t threadCount,
                  std::unique_ptr<std::mutex> thread_mutex) {
    auto descriptors = getFamilyDescriptors(path);
    auto handlers = openDB(descriptors, path, db);

    std::list <StrContainer> StrContainerList;

    for (auto &family : handlers) {
        StrContainerList.push_back(
          getStrs(family.get(), db));
    }

    ThreadPool threads(threadCount);
    for (size_t i = 0; i < threadCount; ++i) {
        threads.enqueue([&handlers, &StrContainerList,
                       &db, &thread_mutex] {
        setupHash(&handlers, &StrContainerList,
                  db, thread_mutex);
        });
    }
}

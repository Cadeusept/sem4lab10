// Copyright 2021 Your Name <your_email>

#include <hashing_db.hpp>

#include "../third-party/picosha2.h"

std::mutex console_mutex;

void copyDB(std::string source_path, std::string dest_path) {
  `
}

FamHandlerContainer DBhasher::openDB(const FamDescContainer &descriptors) {
    FamHandlerContainer handlers;
    std::vector < rocksdb::ColumnFamilyHandle * > newHandles;
    rocksdb::DB *dbStrPtr;

    rocksdb::Status status =
    rocksdb::DB::Open(
          rocksdb::DBOptions(),
          _path,
          descriptors,
          &newHandles,
          &dbStrPtr);

    assert(status.ok());

    _db.reset(dbStrPtr);

    for (rocksdb::ColumnFamilyHandle *ptr : newHandles) {
        handlers.emplace_back(ptr);
    }

    return handlers;
}

FamDescContainer DBhasher::getFamilyDescriptors() {
    rocksdb::Options options;

    std::vector <std::string> family;
    FamDescContainer descriptors;
    rocksdb::Status status =
        rocksdb::DB::ListColumnFamilies(rocksdb::DBOptions(),
                                        _path,
                                        &family);
    assert(status.ok());

  for (const std::string &familyName : family) {
    descriptors.emplace_back(familyName,
                             rocksdb::ColumnFamilyOptions());
  }
  return descriptors;
}

StrContainer DBhasher::getStrs(rocksdb::ColumnFamilyHandle *family) {
  boost::unordered_map <std::string, std::string> dbCase;
  std::unique_ptr <rocksdb::Iterator>
      it(_db->NewIterator(rocksdb::ReadOptions(),
                             family));

  for (it->SeekToFirst(); it->Valid(); it->Next()) {
      std::string key = it->key().ToString();
      std::string value = it->value().ToString();
      dbCase[key] = value;
  }

  return dbCase;
}

void DBhasher::setupHash
    (FamHandlerContainer *handlers,
     std::list <StrContainer> *StrContainerList) {
    while (!handlers->empty()) {
        _mutex.lock();

        if (handlers->empty()) {
            _mutex.unlock();
            continue;
        }

        auto &family = handlers->front();
        handlers->pop_front();

        StrContainer str_container = StrContainerList->front();
        StrContainerList->pop_front();

        _mutex.unlock();

        writeHash(family.get(), str_container);
    }
}

void DBhasher::writeHash
        (rocksdb::ColumnFamilyHandle *family, StrContainer strContainer) {
    for (auto it = strContainer.begin(); it != strContainer.end(); ++it)
    {
        std::string hash = picosha2::hash256_hex_string(it->first +
                                                            it->second);
        console_mutex.lock();
        std::cout << "key: " << it->first << " hash: " << hash << std::endl;
        console_mutex.unlock();

        //logs::logInfo(it->first, hash);
        rocksdb::Status status = _db->Put(rocksdb::WriteOptions(),
                                          family,
                                          it->first,
                                          hash);
        assert(status.ok());
    }
}

void DBhasher::startThreads() {
    auto descriptors = getFamilyDescriptors();
    auto handlers = openDB(descriptors);

    std::list <StrContainer> StrContainerList;

    for (auto &family : handlers) {
        StrContainerList.push_back(
          getStrs(family.get()));
    }

    std::vector <std::thread> threads;
    threads.reserve(_threadCountHash);
    for (size_t i = 0; i < _threadCountHash; ++i) {
        threads.emplace_back(std::thread(&DBhasher::setupHash, this,
                                       &handlers, &StrContainerList));
    }

    for (auto &th : threads) {
        th.join();
    }
}

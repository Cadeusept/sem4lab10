// Copyright 2021 Your Name <your_email>

#include <hashing_db.hpp>

#include "../third-party/picosha2.h"

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


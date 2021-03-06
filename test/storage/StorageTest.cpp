#include "gtest/gtest.h"
#include <iostream>
#include <set>
#include <vector>

#include <storage/MapBasedGlobalLockImpl.h>
#include <afina/execute/Get.h>
#include <afina/execute/Set.h>
#include <afina/execute/Add.h>
#include <afina/execute/Append.h>
#include <afina/execute/Delete.h>

using namespace Afina::Backend;
using namespace Afina::Execute;
using namespace std;
//

TEST(StorageTest, PutGet) {
    MapBasedGlobalLockImpl storage;

    storage.Put("KEY1", "val1");
    storage.Put("KEY2", "val2");

    std::string value;
    EXPECT_TRUE(storage.Get("KEY1", value));
    EXPECT_TRUE(value == "val1");

    EXPECT_TRUE(storage.Get("KEY2", value));
    EXPECT_TRUE(value == "val2");
}

TEST(StorageTest, PutOverwrite) {
    MapBasedGlobalLockImpl storage;

    storage.Put("KEY1", "val1");
    storage.Put("KEY1", "val2");

    std::string value;
    EXPECT_TRUE(storage.Get("KEY1", value));
    EXPECT_TRUE(value == "val2");
}

TEST(StorageTest, PutIfAbsent) {
    MapBasedGlobalLockImpl storage;

    storage.Put("KEY1", "val1");
    storage.PutIfAbsent("KEY1", "val2");

    std::string value;
    EXPECT_TRUE(storage.Get("KEY1", value));
    EXPECT_TRUE(value == "val1");
}

TEST(StorageTest, BigTest)  {
    const long SIZE = 100000;
    long cur_size = 0;

    MapBasedGlobalLockImpl storage(SIZE);

    std::stringstream ss;
    long i = 0;

    while (true) {
        ss << "Key" << i;
        std::string key = ss.str();
        ss.str("");
        ss << "Val" << i;
        std::string val = ss.str();
        ss.str("");

        cur_size += key.size() + val.size();
        if (cur_size > SIZE) {
            i--;
            break;
        }
        i++;
        storage.Put(key, val);
    }

    for ( ; i >=0; i--) {
        ss << "Key" << i;
        std::string key = ss.str();
        ss.str("");
        ss << "Val" << i;
        std::string val = ss.str();
        ss.str("");

        std::string res;
        storage.Get(key, res);

        EXPECT_TRUE(val == res);
    }
}

TEST(StorageTest, MaxTest) {
    const long SIZE = 7 * 8; //len(key0+val0)==8
    long cur_size = 0;

    MapBasedGlobalLockImpl storage(SIZE);

    std::stringstream ss;

    for (int i = 0; i < 10; i++) {
        ss << "Key" << i;
        std::string key = ss.str();
        ss.str("");
        ss << "Val" << i;
        std::string val = ss.str();
        ss.str("");
        storage.Put(key, val);
    }

    for (int i = 3; i < 10; i++) {
        ss << "Key" << i;
        std::string key = ss.str();
        ss.str("");
        ss << "Val" << i;
        std::string val = ss.str();
        ss.str("");

        std::string res;
        storage.Get(key, res);

        EXPECT_TRUE(val == res);
    }

    for (int i = 0; i < 3; i++) {
        ss << "Key" << i;
        std::string key = ss.str();
        ss.str("");

        std::string res;
        EXPECT_FALSE(storage.Get(key, res));
    }
}

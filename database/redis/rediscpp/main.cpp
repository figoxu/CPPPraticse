#include <iostream>
#include <redis_connection.hpp>

int main() {
    std::cout << "Hello, World!" << std::endl;


    redis::connection conn;

    conn.set("key1", "value1");
    conn.hset("hash1", "field1", "value1");
    conn.sadd("set1", "member1");
    conn.zadd("zset1", 0.9, "member2");

    std::cout << "Key1:" << conn.get("value1") << std::endl;

    return 0;
}
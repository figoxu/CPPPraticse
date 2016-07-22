# redis-cpp
Redis C++ Client.

# Attention
## It's still under developing.

### Functions below has tested pass on Unbuntu 14.04/redis_version:2.8.4/g++ 4.8.2.

* list
* set

### TODO:

* Connections pool
* hash
* sorted set
* hash
* Windows and OSX support.

# Example
<pre><code>
#include < iostream >
#include "redis_connection.hpp"

int main(int argc, char *argv[])
{
	redis::connection conn;

	conn.set("key1", "value1");
	conn.hset("hash1", "field1", "value1");
	conn.sadd("set1", "member1");
	conn.zadd("zset1", 0.9, "member2");

	std::cout << "Key1:" << conn.get("value1") << std::endl;

    return 0;
}
</code></pre>

# Thanks
This soft is based on [jrk's redis-cplusplus-client](https://github.com/jrk/redis-cplusplus-client).

# License
This client is licensed under the same license as redis.

# Author
Wu Hong

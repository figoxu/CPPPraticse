redis_test:redis_error.o redis_connection.o redis_pool.o redis_command.o anet.o redis_utils.o test/redis_test.o test/redis_test_string.o test/redis_test_list.o test/redis_test_set.o test/redis_test_hash.o test/redis_test_zset.o
	g++ -o $@ $^ -lpthread -lcppunit

redis_example:redis_error.o redis_connection.o redis_pool.o redis_command.o anet.o redis_utils.o redis_example.o
	g++ -o $@ $^ -lpthread -lcppunit

%.o:%.cpp
	g++ -c -g $< -o $@ -I. -Itest
%.o:%.c
	gcc -c -g $< -o $@

clean:
	rm -rf *.o

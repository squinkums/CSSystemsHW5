#include <iostream>
#include "fifo_evictor.hh"
#include <cassert>
#include <chrono>
#include <thread>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
#include "cache.hh"

size_t liam_hash(key_type key)
{
	// A hash function that forces "ab" and "ba" to collide in the hashtable
	int result = 0;
	for (auto x : key)
	{
		int mod = x % 10;
		result += mod;
	}
	return result;
}

void basic_test(Cache* my_cache)
{
	//A basic test that tests for basic set(), get(), and reset() functionality
	// We cannot test anyone of these independently of each other because those are the prerequisites for a cache to even make sense.
	key_type const my_key1 = "hello";
	auto const my_value1 = new char[3];
	my_value1[0] = 'a';
	my_value1[1] = 'b';
	my_value1[2] = 'c';
	uint32_t my_size = 2;
	//Test set
	my_cache->set(my_key1, my_value1, 3);
	//Test get
	auto get_back = my_cache->get(my_key1, my_size);
	assert(get_back[0] == 'a');
	assert(get_back[1] == 'b');
	// We have only gotten back 2 bytes, the third byte should not match
	assert(get_back[2] != 'c');
	assert(my_cache->space_used() == 3);
	delete[] get_back;

	delete[] my_value1;
	my_cache->reset();
	assert(my_cache->space_used() == 0);
	get_back = my_cache->get(my_key1, my_size);
	assert(get_back == nullptr);
	return;
}

void deletion_test(Cache* my_cache)
{
	// Try to delete an element
	key_type const my_key1 = "hello";
	auto const my_value1 = new char[3];
	my_value1[0] = 'a';
	my_value1[1] = 'b';
	my_value1[2] = 'c';
	uint32_t my_size = 3;
	my_cache->set(my_key1, my_value1, 3);
	auto res = my_cache->del(my_key1);
	assert(res == true);
	auto const get_back = my_cache->get(my_key1, my_size);
	assert(get_back == nullptr);
	assert(my_cache->del(my_key1) == false);
	delete[] get_back;
	delete[] my_value1;
	my_cache->reset();
	return;

}

void overwrite_test(Cache* my_cache)
{
	key_type const my_key1 = "hello";
	auto const my_value1 = new char[3];
	my_value1[0] = 'a';
	my_value1[1] = 'b';
	my_value1[2] = 'c';
	auto const my_value2 = new char[3];
	my_value2[0] = 'd';
	my_value2[1] = 'e';
	my_value2[2] = 'f';
	my_cache->set(my_key1, my_value1, 3);
	uint32_t my_size = 2;
	my_cache->set(my_key1, my_value2, 3);
	auto const get_back = my_cache->get(my_key1, my_size);
	assert(get_back[0] == 'd');
	delete[] get_back;
	delete[] my_value1;
	delete[] my_value2;
	my_cache->reset();
	return;
}

void null_evictor_test(Cache* my_cache)
{
	//Test if the  when we exceed the capacity of the cache without assigning an evictor
	for (auto i = 0; i < 50; i++)
	{
		key_type const my_key1 = std::to_string(i);
		auto const my_value1 = new char[3];
		if(i<33)
		{
			my_value1[0] = 'a';
			my_value1[1] = 'b';
			my_value1[2] = 'c';

		}else
		{
			my_value1[0] = 'd';
			my_value1[1] = 'e';
			my_value1[2] = 'f';
		}
		my_cache->set(my_key1, my_value1, 3);
		delete[] my_value1;
	}
	assert(my_cache->space_used() == 99);
	uint32_t my_size = 3;
	auto const get_zero = my_cache->get(std::to_string(0), my_size);
	auto const get_32 = my_cache->get(std::to_string(32), my_size);
	assert(get_zero[0] == 'a');
	assert(get_32[0] == 'a');
	assert(my_cache->get(std::to_string(33), my_size) == nullptr);
	my_cache->reset();
	delete[] get_zero;
	delete[] get_32;
	return;
}

void deep_copy_test(Cache* my_cache)
{
	// Test if the set and get values are deep copied
	key_type const my_key1 = "hello";
	auto const my_value1 = new char[3];
	my_value1[0] = 'a';
	my_value1[1] = 'b';
	my_value1[2] = 'c';
	uint32_t my_size = 2;
	my_cache->set(my_key1, my_value1, 3);
	//The value is deleted
	delete[] my_value1;
	//Since the values are deep copied, we should still be able to get back our values.
	auto const get_back = my_cache->get(my_key1, my_size);
	assert(get_back[0] == 'a');
	my_cache->reset();
	// Since the return value of get is deep-copied, we should still be able to access it
	return;

}

//void resize_test()
//{
	//This is implementation-specific but still useful 
//	auto my_cache = new Cache(400);
//	for (auto i = 0; i < 100; i++)
//	{
//		key_type const my_key1 = std::to_string(i);
//		auto const my_value1 = new char[3];
//		my_value1[0] = 'a';
//		my_value1[1] = 'b';
//		my_value1[2] = 'c';
//		my_cache->set(my_key1, my_value1, 3);
//		delete[] my_value1;
//	}
//	assert(my_cache->space_used() == 300);
//	delete my_cache;

//}

void Collision_test(Cache* my_cache)
{
	// Map two keys into the same hashtable entry and see if we can get both of them back
	key_type const my_key1 = "ab";
	key_type const my_key2 = "ba";
	auto const my_value1 = new char[3];
	my_value1[0] = 'a';
	my_value1[1] = 'b';
	my_value1[2] = 'c';
	auto const my_value2 = new char[3];
	my_value2[0] = 'd';
	my_value2[1] = 'e';
	my_value2[2] = 'f';
	uint32_t my_size = 3;
	my_cache->set(my_key1, my_value1, 3);
	my_cache->set(my_key2, my_value2, 3);
	delete[] my_value1;
	delete[] my_value2;
	const auto get1 = my_cache->get(my_key1, my_size);
	const auto get2 = my_cache->get(my_key2, my_size);
	assert(get1[0] == 'a');
	assert(get2[0] == 'd');
	delete[] get1;
	delete[] get2;
	my_cache->reset();
	return;
}

void fifo_evictor_test(Cache* my_cache)
{
	uint32_t my_size = 3;
	// Insert 50 key-val pairs to fill in the cache 
	for (auto i = 0; i < 50; i++)
	{
		key_type const my_key1 = std::to_string(i);
		auto const my_value1 = new char[my_size];
		my_value1[0] = 'a';
		my_value1[1] = 'b';
		my_value1[2] = 'c';
		my_cache->set(my_key1, my_value1, my_size);
		delete[] my_value1;
	}
	// Insert a new key 50
	key_type const my_key1 = std::to_string(50);
	auto const my_value1 = new char[3];
	my_value1[0] = 'd';
	my_value1[1] = 'e';
	my_value1[2] = 'f';
	my_cache->set(my_key1, my_value1, 3);
	// Because cache is full, 0 should be evicted
	auto const get0 = my_cache->get(std::to_string(0), my_size);
	assert(get0 == nullptr);
	auto const get50 = my_cache->get(my_key1, my_size);
	assert(get50[0] == 'd');
	assert(my_cache->space_used() == 150);
	// Before inserting a new key 51, we modify key 1 first. So the key evicted when inserting 51 should be key 2. 
	key_type const my_key2 = std::to_string(51);
	my_cache->set(std::to_string(1), my_value1, 3);
	my_cache->set(my_key2, my_value1, 3);
	auto const get2 = my_cache->get(std::to_string(2), my_size);
	assert(get2 == nullptr);
	auto const get1 = my_cache->get(std::to_string(1), my_size);
	assert(get1[0] == 'd');
	assert(my_cache->space_used() == 150);
	// Before inserting yet another new key 52, we get value from key 3 first. So the key evicted should be key 4.
	key_type const my_key3 = std::to_string(52);
	delete[] my_cache->get(std::to_string(3), my_size);
	my_cache->set(my_key3, my_value1, 3);
	auto const get3 = my_cache->get(std::to_string(3), my_size);
	assert(get3[0] == 'a');
	delete[] get0;
	delete[] get50;
	delete[] get2;
	delete[] get1;
	delete[] get3;
	delete[] my_value1;
	my_cache->reset();
	return;


}

int main()
{
	//Note: we cannot just change the constructor function because with client construction we cannot specify maxmem, evictor etc. We could only pass one test at a time and then go on to restart the server.
	std::string address = "http://127.0.0.1:"; // Make sure address and port align with server
	std::string port = "34568";
	Cache* my_cache = new Cache(address, port);
	// The following tests are based on a server cache of maxmem = 100, evictor = nullptr, hashfunc = default, load_factor = 0.75
	//Basic test
	std::cout << "---------------------------------basic test started---------------------------------------" << '\n' << '\n';
	basic_test(my_cache);
	std::cout << "basic test passed!" << '\n';

	//Now that we have the basic functions tested, we can test each feature of the cache independently.

	// Test for deletion
	std::cout << "---------------------------------deletion test started---------------------------------------" << '\n' << '\n';
	deletion_test(my_cache);
	std::cout << "deletion test passed!" << '\n';

	// Test for overwrite
	std::cout << "---------------------------------overwrite test started---------------------------------------" << '\n' << '\n';
	overwrite_test(my_cache);
	std::cout << "overwrite test passed!" << '\n';
	
	//Test for null_evictor i.e. when the evictor parameter takes in a nullptr
	std::cout << "---------------------------------null evictor test started---------------------------------------" << '\n' << '\n';
	null_evictor_test(my_cache);
	std::cout << "null evictor test passed!" << '\n';

	//we test for deep copy last 
	std::cout << "---------------------------------deep copy test started---------------------------------------" << '\n' << '\n';
	deep_copy_test(my_cache);
	std::cout << "deep copy test passed!" << '\n';
	delete my_cache;

	


	std::cout << "---------------------------------all tests passed :>--------------------------------------" << '\n' << '\n';



	return 0;
}


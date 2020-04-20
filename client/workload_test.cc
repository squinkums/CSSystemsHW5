#include <iostream>
#include <cassert>
#include <chrono>
#include <thread>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include "cache.hh"
#include "fifo_evictor.hh"
#include <random>
#include <string>
#include <vector>
#include <algorithim>

using namespace utility;                                                                                                                                                                          
using namespace web;                                                                                                                                                                                           
using namespace web::http;                                                                                                                                                                                       
using namespace web::http::client;          
using namespace concurrency::streams;

//Credit to inversepalindrome.com for the function to create random strings.                                                                                                                                                                 
std::string create_random_string(int length) {
  const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  std::string result;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, characters.size() - 1);
  for(int i = 0; i < length; i++) {
    result += characters[dis(gen)];
  }
  return result;
}

void etc_workload(Cache* my_cache, int limit, std::vector<std::string>& keys_a, std::vector<std::string>& keys_b, int& hits, int& misses) {
  
  //random number for key pool                                                                                                                                                                                                         
  std::random_device rd_1;                                                                                                                                                                                                                   
  std::mt19937 gen_1(rd_1());                                                                                                                                                                                                                
  std::uniform_int_distribution<> dis_1(1, 100);

  //random number for request type
  std::random_device rd_2;                                                                                                                                                                                                                   
  std::mt19937 gen_2(rd_2());                                                                                                                                                                                                                
  std::uniform_int_distribution<> dis_2(1, 30);

  //variable for chosen vector of keys
  std::vector<std::string> keys;

  //begin workload
  for(int i = 0; i < limit; i++) {

    //decide what pool of keys to choose from
    int m = dis_1(gen_1);                                                                                                                                                                                                                    
    if(m == 100) {                                                                                                                                                                                                                           
      keys = keys_b;                                                                                                                                                                                                                         
    }                                                                                                                                                                                                                                        
    else {                                                                                                                                                                                                                                   
      keys = keys_a;                                                                                                                                                                                                                         
    }

    //variable for amount of keys in pool
    int keys_length = keys.size();

    //random number for key from pool
    std::random_device rd_k;                                                                                                                                                                                                                 
    std::mt19937 gen_k(rd_k());                                                                                                                                                                                                              
    std::uniform_int_distribution<> dis_k(0, keys_length - 1);

    //choose a key from the pool
    int k = dis_k(gen_k);                                                                                                                                                                                                                    
    std::string key = keys[k];

    //decide and perform request
    int n = dis_2(gen_2);                                                                                                                                                                                                                    
    if(n < 21) {                                                                                                                                                                                                                             
      n = 1;                                                                                                                                                                                                                                 
    }                                                                                                                                                                                                                                        
    else if(n > 1 && n < 29) {                                                                                                                                                                                                               
      n = 2;                                                                                                                                                                                                                                 
    }                                                                                                                                                                                                                                        
    else if(n == 30) {                                                                                                                                                                                                                       
      n = 3;                                                                                                                                                                                                                                 
    }                                                                                                                                                                                                                                        
    switch(n) {                                                                                                                                                                                                                              
      case 1: auto get = my_cache->get(key, 3);
	      if(get == nullptr) {
		misses += 1;
	      }
	      else {
		hits += 1;
	      }
	      delete[] get;
	      break;                                                                                                                                                                         
      case 2: auto del = my_cache->del(key); delete[] del; break;                                                                                                                                                                            
      case 3: std::random_device rd_v;
	      std::mt19937 gen_v(rd_v());
	      std::uniform_int_distribution dis_v(1, 10);
	      int r = dis_v(gen_v);
	      int v = dis_v(gen_v);
	      if(r > 4) {
		v *= 100;
	      }
	      auto const value = new char[v]
	      my_cache->set(key, value, v);                                                                                                                                                                                               
    }                                                                                                                                                                                                                                        
  }
  my_cache->reset();                                                                                                                                                                                                                         
}

int main() {
  std::string address = "http://127.0.0.1";
  std::string port = "34568";
  Cache* my_cache = new Cache(address, port);
  int a = 1;
  int b = 2;
  std::vector<std::string> set_a;
  std::vector<std::string> set_b;
  for(int i = 0; i < 5; i++) {
    for(int j = 0; j < 10; j++) {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<> dis(1, 10);
      int r = dis(gen);
      int x;
      int y;
      if(r < 7) {
        x = a + 20;
        y = b + 20;
      }
      else if(r < 9){
        x = a + 40;
        y = b + 40;
      }
      else {
        x = a + 60;
        y = b + 60;
      }
      set_a.push_back(create_random_string(x));
      set_b.push_back(create_random_string(y));
    }
    a += 2;
    b += 2;
  }
  std::random_shuffle(set_a.begin(), set_a.end());
  std::random_shuffle(set_b.begin(), set_b.end());
  for(int k = 0; k < 50; k++) {
    std::random_device rd_a;
    std::mt19937 gen_a(rd_a());
    std::uniform_int_distribution dis_a(1, 10);
    int va = dis_a(gen_a);
    int vb = dis_a(gen_a);
    int ra = dis_a(gen_a);
    int rb = dis_a(gen_a);
    if(ra > 4) {
      va *= 100;
    }
    if(rb > 4) {
      vb *= 100;
    }
    auto const val_a = new char[va];
    auto const val_b = new char[vb];
    my_cache->set(set_a[k], val_a, va);
    my_cache->set(set_b[k], val_b, vb);
  }
  int hits = 0;
  int misses = 0;
  etc_workload(my_cache, 100, set_a, set_b, hits, misses);
  int total = hits + misses;
  int hit_rate = static_cast<float>(hits) / static_cast<float>(total);
  return 0;
}


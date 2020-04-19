#include <iostream>
#include <random>
#include <string>
#include <vector>

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

void workload_test(int limit, std::vector<std::string>& set_a, std::vector<std::string>& set_b) {
  std::random_device rd_1;
  std::mt19937 gen_1(rd_1());
  std::uniform_int_distribution<> dis_1(1, 100);
  std::vector<std::string> set;
  std::string set_title;
  std::random_device rd_2;
  std::mt19937 gen_2(rd_2());
  std::uniform_int_distribution<> dis_2(1, 30);
  for(int i = 0; i < limit; i++) {
    int m = dis_1(gen_1);
    if(m == 100) {
      set = set_b;
      set_title = "set_b";
    }
    else {
      set = set_a;
      set_title = "set_a";
    }
    std::cout << "M = " << m << "     Set = " << set_title << "     ||";
    std::random_device rd_k;
    std::mt19937 gen_k(rd_k());
    std::uniform_int_distribution<> dis_k(0, set.size() - 1);
    int k = dis_k(gen_k);
    std::cout << "     " << k << "     ";
    std::string key = set[k];
    int n = dis_2(gen_2);
    std::cout << n;
    if(n < 21) {
      n = 1;
    }
    else if(n > 1 && n < 30) {
      n = 2;
    }
    else if(n == 30) {
      n = 3;
    }
    std::cout << "     " << n;
    std::string request;
    switch(n) {
    case 1: request = "GET"; break;
    case 2: request = "DELETE"; break;
    case 3: request = "SET"; break;
    }
    std::cout << "     " << request << " " << key << std::endl;
  }
}

int main() {
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
  std::cout << std::endl;
  workload_test(100, set_a, set_b);
  return 0;
}

/*
void etc_workload(Cache* my_cache, int limit, std::vector<std::string>& keys_a, std::vector<std::string>& keys_b) {
  auto const my_value = new char[3];
  std::random_device rd_1;
  std::mt19937 gen_1(rd_1());
  std::uniform_int_distribution<> dis_1(1, 100);
  std::random_device rd_2;
  std::mt19937 gen_2(rd_2());
  std::uniform_int_distribution<> dis_2(1, 30);
  std::vector<std::string> keys;
  for(int i = 0; i < limit; i++) {
    int m = dis_1(gen_1);
    if(m == 100) {
      keys = keys_b;
    }
    else {
      keys = keys_a;
    }
    int keys_length = keys.size();
    std::random_device rd_k;
    std::mt19937 gen_k(rd_k());
    std::uniform_int_distribution<> dis_k(0, keys_length - 1);
    int k = dis_k(gen_k);
    std::string key = keys[k];
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
      case 1: auto get = my_cache->get(key, 3); delete[] get; break;
      case 2; auto del = my_cache->del(key); delete[] del; break;
      case 3; my_cache->set(key, my_value, 3);
    }
  }
  delete[] my_value;
  my_cache->reset();
}
*/

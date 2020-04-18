#include <iostream>
#include <random>
#include <string>

int main(){
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, 30);
  for(int i = 0; i < 100; i++) {
    int n = dis(gen);
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
    std::cout << "     " << request << std::endl;
  }
  return 0;
}

//void etc_workload(Cache* my_cache, int limit) {
//  auto const my_value = new char[3];
//  my_value[0] = 'a';
//  my_value[1] = 'b';
//  my_value[2] = 'c';
//  std::random_device rd;
//  std::mt19937 gen(rd());
//  std::uniform_int_distribution<> dis(1, 30);
//  for(int i = 0; i < limit; i++) {
//    int n = dis(gen);
//    if(n < 21) {
//      n = 1;
//    }
//   else if(n > 1 && n < 29) {
//      n = 2;
//    }
//    else if(n == 30) {
//      n = 3;
//    }
//    switch(n) {
//      case 1: auto get = my_cache->get(key, 3); delete[] get; break;
//      case 2; auto del = my_cache->del(key); delete[] del; break;
//      case 3; my_cache->set(key, my_value, 3);
//    }
//  }
//  delete[] my_value;
//  my_cache->reset();
//}

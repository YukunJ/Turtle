/* dummy test for Cache class */
#include <iostream>
#include <vector>

#include "cache.h"
#include "turtle_server.h"

int main() {
  std::vector<unsigned char> data1;
  data1.push_back(static_cast<unsigned char>('a'));
  data1.push_back(static_cast<unsigned char>('b'));
  data1.push_back(static_cast<unsigned char>('c'));
  TURTLE_SERVER::Cache cache(10);
  for (int i = 0; i < 4; i++) {
    std::cout << " i=" << i;
    std::cout << ((cache.TryInsert("sample" + std::to_string(i), data1))
                      ? "True"
                      : "False")
              << std::endl;
  }

  std::vector<unsigned char> data_big;
  for (int i = 0; i < 10; i++) {
    data_big.push_back(static_cast<unsigned char>('a'));
  }
  std::cout << ((cache.TryInsert("sample 10 big", data_big)) ? "True" : "False")
            << std::endl;
  data_big.push_back(static_cast<unsigned char>('a'));
  std::cout << ((cache.TryInsert("sample 11 big", data_big)) ? "True" : "False")
            << std::endl;
  return 0;
}

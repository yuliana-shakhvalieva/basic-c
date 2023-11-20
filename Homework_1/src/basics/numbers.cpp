#include "numbers.h"
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <vector>

namespace mse {
std::size_t sum_digits(std::int32_t number){
  std::string s = std::to_string(number);

  int sum = 0;

  for (auto & c : s) {
    if (c != '-')
      sum += (c - '0');
  }
  return sum;
}

bool is_prime(uint32_t n) {
  for (uint32_t i = 2; i < n; i++) {
    if (n % i == 0) {
      return false;
    }
  }
  return true;
}

std::vector<uint32_t> primes = {2, 3};
std::size_t count_primes(std::uint32_t limit){
  auto a = primes;
  if (limit <= primes[primes.size() - 1]){
    int32_t l = -1;
    int32_t r = primes.size();

    while (r - l > 1){
      int32_t m = (r + l) / 2;
      if (primes[m] >= limit)
        r = m;
      else
        l = m;
    }
    return r;
  }

  uint32_t count = primes.size();
  for (uint32_t i=(primes[primes.size() - 1] + 1); i < limit; i++){
    if (is_prime(i)){
      primes.push_back(i);
      count ++;
    }
  }
  return count;
}


std::int32_t divide(std::int32_t a, std::int32_t b) {
  int64_t result_64 = 0;
  bool positive = true;

  if (((a > 0) && (b < 0)) || ((a < 0) && (b > 0)))
    positive = false;

  auto a_64 = std::int64_t(a);
  auto b_64 = std::int64_t(b);

  std::int64_t a_positive_64 = std::abs(a_64);
  std::int64_t b_positive_64 = std::abs(b_64);

  while (((a_positive_64 - b_positive_64) >= 0) && (a_positive_64 != 0)) {
    a_positive_64 -= b_positive_64;
    result_64 += 1;
  }

  auto result_32 = int32_t(result_64);

  std::int32_t min_int = std::numeric_limits<std::int32_t>::min();
  std::int32_t max_int = std::numeric_limits<std::int32_t>::max();

  if (result_32 == min_int && positive)
    return max_int;

  if (result_32 == max_int && !positive)
    return min_int;

  return positive ? result_32 : - result_32;
}


std::map<std::string, unsigned> converter = {{"I",1},
                                             {"IV",4},
                                             {"V",5},
                                             {"IX",9},
                                             {"X",10},
                                             {"XL",40},
                                             {"L",50},
                                             {"XC",90},
                                             {"C",100},
                                             {"CD", 400},
                                             {"D", 500},
                                             {"CM", 900},
                                             {"M",1000}};

std::vector<std::string> roman_values = {"M", "CM", "D", "CD", "C", "XC",
                                         "L", "XL", "X", "IX", "V", "IV", "I"};


std::int32_t roman_to_integer(const std::string &number){
  int32_t result = 0;

  std::string roman_number = number;

  for (auto & c : roman_values){
    while (roman_number.rfind(c, 0) == 0){
      result += converter[c];
      roman_number = roman_number.substr(c.size(), roman_number.size()-c.size());
    }
  }
  return result;
}
} // namespace mse

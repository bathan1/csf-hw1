#include <cassert>
#include "bigint.h"
#include <sstream>
#include <iomanip>

BigInt::BigInt() : magnitude({}), negative(false) {}

BigInt::BigInt(uint64_t val, bool negative) : negative(negative) 
{
  magnitude.push_back(val);
}

BigInt::BigInt(std::initializer_list<uint64_t> vals, bool negative) : magnitude(vals), negative(negative) {}

BigInt::BigInt(const BigInt &other) : magnitude(other.magnitude), negative(other.negative) {}

BigInt::~BigInt() {}

BigInt &BigInt::operator=(const BigInt &rhs) {}

bool BigInt::is_negative() const
{
  return negative;
}

const std::vector<uint64_t> &BigInt::get_bit_vector() const 
{
  return magnitude;
}

uint64_t BigInt::get_bits(unsigned index) const
{
  if (index >= this->magnitude.size()) 
  {
    return 0;
  }

  return this->magnitude[index];
}

BigInt BigInt::operator+(const BigInt &rhs) const
{
  // TODO: implement
}

BigInt BigInt::operator-(const BigInt &rhs) const
{
  // TODO: implement
  // Hint: a - b could be computed as a + -b
}

BigInt BigInt::operator-() const
{
  // TODO: implement
}

bool BigInt::is_bit_set(unsigned n) const
{
  // TODO: implement
}

BigInt BigInt::operator<<(unsigned n) const
{
  // TODO: implement
}

BigInt BigInt::operator*(const BigInt &rhs) const
{
  // TODO: implement
}

BigInt BigInt::operator/(const BigInt &rhs) const
{
  // TODO: implement
}

int BigInt::compare(const BigInt &rhs) const
{
  // TODO: implement
}

std::string BigInt::to_hex() const // Convert the magnitude (stored as a vector of unint64_t) to hexdec string
{
  // Check is magnitude vector is empty OR if it has one element 0
  if (magnitude.empty() || (magnitude.size() == 1 && magnitude[0] == 0)) {
    return "0";
  }

  // Loop through mag vec in reverse (store endian little-endian format) to construct hex
  std::stringstream ss;
  for (auto i = magnitude.rbegin(); i != magnitude.rend(); ++i) {
    if (i != magnitude.rbegin()) {
      ss << std::setfill('0') << std::setw(16);
    }
    ss << std::hex << *i;
  }

  std::string result = ss.str();
  result.erase(0, result.find_first_not_of('0')); // Remove leading 0's

  if (negative && result != "0") {
    result = "-" + result;
  }

  return result;
}

std::string BigInt::to_dec() const
{
  // TODO: implement
}


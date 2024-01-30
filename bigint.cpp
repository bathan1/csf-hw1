#include <cassert>
#include "bigint.h"

BigInt::BigInt() : magnitude(0), negative(false) {}

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

std::string BigInt::to_hex() const
{
  // TODO: implement
}

std::string BigInt::to_dec() const
{
  // TODO: implement
}


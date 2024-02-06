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
   BigInt result;
  // Handles cases where operands have different signs
  if (this->negative != rhs.negative) {
    if (this->compareMagnitudes(rhs) >= 0) {
            // If *this larger or equal magnitude, subtract rhs magnitude from *this magnitude
            result = this->subtractMagnitudes(rhs);
            result.negative = this->negative; 
        } else {
            // If rhs larger magnitude, subtract *this magnitude from rhs magnitude
            result = rhs.subtractMagnitudes(*this);
            result.negative = rhs.negative; 
        }
        return result;
    }

    // Same sign --> perform addition of magnitudes
    result.negative = this->negative; 
    result = this->addMagnitudes(rhs); 
    return result;
}

// Helper function using the “grade school” algorithm for operator+
BigInt BigInt::addMagnitudes(const BigInt &rhs) const {
    BigInt result;
    uint64_t carry = 0;
    size_t maxLength = std::max(this->magnitude.size(), rhs.magnitude.size());

    for (size_t i = 0; i < maxLength || carry; ++i) {
        uint64_t a = this->get_bits(i);
        uint64_t b = rhs.get_bits(i);
        uint64_t sum = a + b + carry;

        //Detect addition overflow
        if (sum < a || sum < b) carry = 1;
        else carry = 0;

        // Appends modulo 2^64 sum to the result magnitude
        result.magnitude.push_back(sum);
    }
    return result;
}

BigInt BigInt::operator-(const BigInt &rhs) const
{
  // TODO: implement
  // Hint: a - b could be computed as a + -b
  //Uses the negation of rhs and calls operator+
    BigInt negatedRhs = -rhs;
    return *this + negatedRhs;
}

// Helper function for operator-
// Assumes that lhs >= rhs for magnitude
BigInt BigInt::subtractMagnitudes(const BigInt &rhs) const {
    BigInt result;
    bool borrow = false; // Boolean to see if needs to borrow from next column

    // Loop range is determined by lhs since we assume lhs is greater
    for (size_t i = 0; i < lhs.magnitude.size(); ++i) {
        uint64_t lhs_digit = lhs.get_bits(i);
        uint64_t rhs_digit = rhs.get_bits(i);
        if (borrow) {
            if (lhs_digit <= rhs_digit) {
                lhs_digit += (1ULL << 64) - 1; 
            } else {
                lhs_digit -= 1;
                borrow = false;
            }
        }
        // Check if borrow needed for current digit's subtraction
        if (lhs_digit < rhs_digit) borrow = true;
        uint64_t diff = lhs_digit - rhs_digit;
        result.magnitude.push_back(diff);
    }

    // Remove trailing zeros
    while (!result.magnitude.empty() && result.magnitude.back() == 0) {
        result.magnitude.pop_back();
    }

    return result;
}

BigInt BigInt::operator-() const
{
  // TODO: implement

  BigInt result(*this); //Make a copy of the current BigInt
  if (!(magnitude.empty() || (magnitude.size() == 1 && magnitude[0] == 0 ))) 
  {
    // Negative sign if num not zero
    result.negative = !negative;
  }
  return result;
}

bool BigInt::is_bit_set(unsigned n) const
{
    int num_bits = this->magnitude.size() * 64;
    // Check if the bit is stored in the vector
    if (n >= num_bits)
    {
       return false; 
    }

    // Next we figure out which index in the vector
    // holds the nth bit and get the decimal value at that index.
    int vector_index = n / 64;
    uint64_t dec_value = this->magnitude[vector_index];
    int binary_index = n % 64;
    uint64_t compare_bitstring = 1ULL << binary_index;

    return (dec_value & compare_bitstring) != 0;
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
  // Check the sign
  if (this->negative != rhs.negative) {
        return this->negative ? -1 : 1;
    }

  bool reverse = this->negative; // If both negative, use inverted logic

  return this->compareMagnitudes(rhs) * (reverse ? -1 : 1); // Compare magnitudes
}

// Helper function for compareMagnitude
// Returns -1 if *this is smaller, 1 if *this is larger, and 0 if equal
int BigInt::compareMagnitudes(const BigInt &rhs) const 
{
    if (this->magnitude.size() != rhs.magnitude.size()) {
        return this->magnitude.size() < rhs.magnitude.size() ? -1 : 1;
    }
    for (int i = this->magnitude.size() - 1; i >= 0; --i) {
        if (this->magnitude[i] != rhs.magnitude[i]) {
            return this->magnitude[i] < rhs.magnitude[i] ? -1 : 1;
        }
    }
    return 0;
}

std::string BigInt::to_hex() const // Convert the magnitude (stored as a vector of unint64_t) to hexdec string
{
  // Check is magnitude vector is empty OR if it has one element 0
  if (magnitude.empty() || (magnitude.size() == 1 && magnitude[0] == 0)) {
    return "0";
  }

  // Loop through mag vec in reverse (store endian little-endian format) to construct hex
  std::stringstream ss;
  for (auto i = magnitude.rbegin(); i != magnitude.rend(); ++i) 
  {
    if (i != magnitude.rbegin()) 
    {
      ss << std::setfill('0') << std::setw(16);
    }
    ss << std::hex << *i;
  }

  std::string result = ss.str();
  result.erase(0, result.find_first_not_of('0')); // Remove leading 0's

  if (negative && result != "0") 
  {
    result = "-" + result;
  }

  return result;
}

std::string BigInt::to_dec() const
{
  // TODO: implement
}


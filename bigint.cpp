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

BigInt &BigInt::operator=(const BigInt &rhs) 
{
    this->negative = rhs.negative;
    this->magnitude = rhs.magnitude;
    return *this;
}

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
    if (this->negative != rhs.negative) 
    {
        if (this->compare_magnitudes(rhs) >= 0) 
        {
            // If *this larger or equal magnitude, subtract rhs magnitude from *this magnitude
            result = this->subtract_magnitudes(rhs);
            result.negative = this->negative; 
        } 
        else 
        {
            // If rhs larger magnitude, subtract *this magnitude from rhs magnitude
            result = rhs.subtract_magnitudes(*this);
            result.negative = rhs.negative; 
        }
        return result;
    }

    // Same sign --> perform addition of magnitudes
    result.negative = this->negative; 
    result = this->add_magnitudes(rhs); 
    return result;
}

// Helper function using the “grade school” algorithm for operator+
BigInt BigInt::add_magnitudes(const BigInt &rhs) const 
{
    BigInt result;
    uint64_t carry = 0;
    size_t max_length = std::max(this->magnitude.size(), rhs.magnitude.size());

    for (size_t i = 0; i < max_length || carry; ++i)
    {
        uint64_t lhs_chunk = this->get_bits(i);
        uint64_t rhs_chunk = rhs.get_bits(i);
        uint64_t sum = lhs_chunk + rhs_chunk + carry;

        //Detect addition overflow
        if (sum < lhs_chunk || sum < rhs_chunk) carry = 1;
        else carry = 0;

        // Appends modulo 2^64 sum to the result magnitude
        result.magnitude.push_back(sum);
    }
    return result;
}

BigInt BigInt::operator-(const BigInt &rhs) const
{
    BigInt negatedRhs = -rhs;
    return *this + negatedRhs;
}

// Helper function for operator-
// Assumes that lhs >= rhs for magnitude
BigInt BigInt::subtract_magnitudes(const BigInt &rhs) const 
{
    BigInt result;
    bool borrow = false; // Boolean to see if needs to borrow from next column

    // Loop range is determined by obejcts since we assume lhs is greater
    for (size_t i = 0; i < this->magnitude.size(); ++i) 
    {
        uint64_t this_digit = this->get_bits(i);
        uint64_t rhs_digit = rhs.get_bits(i);
        if (borrow) 
        {
            if (this_digit == 0) 
            {   // If 0, borrow makes this_digit uint64_max
                this_digit = UINT64_MAX;
            } 
            else 
            {
                this_digit -= 1;
                borrow = false;
            }
        }
        // Check if borrow needed for current digit's subtraction
        if (this_digit < rhs_digit)
        {
          borrow = true;
          this_digit += (1ULL << 63);
        } 

        uint64_t diff = this_digit - rhs_digit; // Calculate the diff
        result.magnitude.push_back(diff);
    }

    // Remove trailing zeros
    while (!result.magnitude.empty() && result.magnitude.back() == 0) 
    {
        result.magnitude.pop_back();
    }

    return result;
}

BigInt BigInt::operator-() const
{
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
    if (negative) throw std::invalid_argument("Cannot left shift a negative BigInt");

    BigInt result;
    unsigned shift_chunks = n / 64;
    unsigned shift_bits = n % 64;

    result.magnitude.resize(magnitude.size() + shift_chunks, 0);
    // If we have a remainder resulting from n / 64, we have to include an additional index for the chunk.
    if (shift_bits > 0) result.magnitude.push_back(0);

    for (size_t i = 0; i < magnitude.size(); ++i) 
    {
        uint64_t chunk = magnitude[i];
        if (shift_bits > 0 && i + shift_chunks + 1 < result.magnitude.size()) 
        {
            // If we have an intra-chunk due to shift_bits > 0, we have to right shift it so that the
            // succeeding chunk has the overflown bits.
            result.magnitude[i + shift_chunks + 1] |= (chunk >> (64 - shift_bits));
        }
        // Now assign the original chunk left shifted to its new position in the result vector.
        result.magnitude[i + shift_chunks] |= (chunk << shift_bits);
    }

    // Remove leading zeros
    while (!result.magnitude.empty() && result.magnitude.back() == 0) 
    {
        result.magnitude.pop_back();
    }

    return result; 
}

BigInt BigInt::operator*(const BigInt &rhs) const
{
    BigInt product = BigInt();
    product.negative = this->negative == rhs.negative ? false : true;    
    
    BigInt pos_lhs = BigInt(*this);
    BigInt pos_rhs = BigInt(rhs);
    for (int i = 0; i < pos_rhs.magnitude.size() * 64; i++)
    {
        if (pos_rhs.is_bit_set(i))
        {
           product = product + (pos_lhs << i); 
        }
    }
    return product;
}

BigInt BigInt::operator/(const BigInt &rhs) const
{
    if (rhs.magnitude.empty())
    {
        throw std::invalid_argument("Can't divide by 0!");
    }

    // If the divisor > dividend, then we simply return 0.
    if (rhs > *this)
    {
        return BigInt();    
    }

    BigInt high = *this;
    BigInt low = BigInt();
    BigInt mid = high.divide_by_two();

    while (low < high)
    {
        BigInt product = mid * rhs;  
        if (product == *this)
        {
            return mid;
        }

        if (product > *this)
        {
            high = mid - BigInt(1);
            mid = (low + high).divide_by_two();
        }
        else 
        {
            low = mid + BigInt(1);
            mid = (low + high).divide_by_two();
        }
    }
    return low;
}

BigInt BigInt::divide_by_two() const
{
    BigInt result = BigInt();
    result.negative = this->negative;

    uint64_t carry = 0;
    for (int i = this->magnitude.size() - 1; i > -1; i--)
    {
        uint64_t chunk = this->magnitude[i];
        uint64_t new_carry = chunk & 1UL;
        chunk >>= 1UL;
        chunk |= carry << 63; // Now carry over the previous carry with an OR operator
        result.magnitude.push_back(chunk);
        carry = new_carry;
    }

    while (result.magnitude.size() > 1 && result.magnitude.back() == 0) result.magnitude.pop_back();

    return result;
}

int BigInt::compare(const BigInt &rhs) const
{
    // Check the sign
    if (this->negative != rhs.negative) 
    {
        return this->negative ? -1 : 1;
    }

    bool reverse = this->negative; // If both negative, use inverted logic

    return this->compare_magnitudes(rhs) * (reverse ? -1 : 1); // Compare magnitudes
}

// Helper function for compareMagnitude
// Returns -1 if *this is smaller, 1 if *this is larger, and 0 if equal
int BigInt::compare_magnitudes(const BigInt &rhs) const 
{
    if (this->magnitude.size() != rhs.magnitude.size()) 
    {
        return this->magnitude.size() < rhs.magnitude.size() ? -1 : 1;
    }
    for (int i = this->magnitude.size() - 1; i >= 0; --i) 
    {
        if (this->magnitude[i] != rhs.magnitude[i]) 
        {
            return this->magnitude[i] < rhs.magnitude[i] ? -1 : 1;
        }
    }
    return 0;
}

std::string BigInt::to_hex() const // Convert the magnitude (stored as a vector of unint64_t) to hexdec string
{
    // Check is magnitude vector is empty OR if it has one element 0
    if (magnitude.empty() || (magnitude.size() == 1 && magnitude[0] == 0)) 
    {
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


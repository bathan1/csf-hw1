TODO: list team members, document who did what, discuss
anything interesting about your implementation.


Team Members: Winston Li (wli111) and Nathan Oh (noh8)

Nathan: Constructor/Destructor, getters, addition, subtraction, division, left shift and unit tests for respective functions
Winston: to_hex(), operator-(),  for unary operator, to_dec, addition, subtraction and unit tests for respective functions

Implementation notes:
Since the hexadecimal representation requires the big endian format, we had to use rbegin() and rend() 
when iterating through our uint64_t vector which represents the magnitude in little endian format.

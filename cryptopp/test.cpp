#include <iostream>
using std::cout;
using std::endl;

#include "cryptopp/integer.h"
using CryptoPP::Integer;

int main( int, char** ) {

  Integer j("100000000000000000000000000000000");
  j %= 1999;

  cout << "j: " << j << endl;

  return 0;
}

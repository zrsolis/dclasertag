#ifndef __BITARRAY_H_
#define __BITARRAY_H_

#include "simpletools.h"

class BitArray {
public:
  static volatile bool bitArraySame(volatile bool source[], volatile bool dest[], int length);
  static volatile bool * ToBool(int input, int length);
  static volatile bool * ToBool(int input, int start_addr, int length);
  static volatile int ToInt(bool array[], int length);
  static volatile void Print(bool array[], int length); 
};

#endif

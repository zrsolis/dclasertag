#include "BitArray.h"

volatile int BitArray::ToInt(bool array[], int length)
{
  int int_value = 0;
  int power = 0; 

  for(int i=length-1; i >= 0; i--)
  {
    if(array[i] == 1)
    {
      power++; 
      int_value += 2^i;
    }
  }

  return int_value;
}

volatile bool BitArray::bitArraySame(volatile bool source[], volatile bool dest[], int length) 
{
  for (int i = 0; i < length; i++)
  {
    if(source[i] != dest[i])
      return false;
  }

  return true;
}

volatile bool * BitArray::ToBool(int input, int length)
{
   int m_input = input;
   bool * ret = new bool[length];
   for (int i = 0; i < length; ++i)
   {
      ret[i] = m_input & 1;
      m_input /= 2;
   }

   return ret;
}

volatile bool * BitArray::ToBool(int input, int start_addr, int length)
{
   int m_input = input;
   bool * ret = new bool[length-start_addr];
   for (int i = 0; i < length; ++i)
   {
      if (i >= start_addr)
        ret[i-start_addr] = m_input & 1;
      m_input /= 2;
   }

   return ret;
}

//delete array after use


volatile void BitArray::Print(bool array[], int length) 
{
  for (int i = 0; i < length; i++)
  {
    print("%d", array[i] & 0x1);
  }
}

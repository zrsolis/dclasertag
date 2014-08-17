#ifndef __Goon_Serial_H
#define __Goon_Serial_H

#include "fdserial.h"

// #define TX_PIN 27 // different for goon and player
// #define RX_PIN 26 // different for goon and player
#define MODE 0 // Mode bits (add):  1 = invert rx,  2 = invert tx,  4 = open-drain/source tx,  8 = ignore tx echo on rx
#define BAUDRATE 115200
#define RX_TIMEOUT_MS 1000   // 1 s


class Goon_Serial
{

private:

   static fdserial* term;

public:

   // Initialize connection. Uses defines for settings. Returns 1 if successful, 0 if not.
   static volatile int Init(int rx_pin, int tx_pin);

   // Transmit data.
   static volatile int Tx(int data);

   // Receive data. Blocks until byte is received. Returns received byte.
   static volatile int Rx_block();

   // Receive data. Blocks until byte is received or timeout is reached. Returns received byte, or 0xff or -1 if timed out.
   static volatile int Rx_timeout();

   // Receive data. Blocks until byte is received or timeout is reached. Returns received byte, or 0xff or -1 if timed out.
   static volatile int Rx_timeout(int ms);

   // Receive data. Does not block. Returns byte in receive buffer, or 0xff or -1 if buffer empty.
   static volatile int Rx_check();

   // Flush rx and tx buffers, then close the connection
   static volatile void Close();

   //tx string or array?

};


#endif 
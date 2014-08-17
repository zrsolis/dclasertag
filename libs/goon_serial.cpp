#include "goon_serial.h"

fdserial* Goon_Serial::term;

// Initialize connection. Uses defines for settings. Returns 1 if successful, 0 if not.
volatile int Goon_Serial::Init(int rx_pin, int tx_pin)
{
   term = fdserial_open(rx_pin,tx_pin,MODE,BAUDRATE);
   if (term)
      return 1;
   else
      return 0;
}

// Transmit data.
volatile int Goon_Serial::Tx(int data)
{
   return fdserial_txChar(term, data);
}

// Receive data. Blocks until byte is received. Returns received byte.
volatile int Goon_Serial::Rx_block()
{
   return fdserial_rxChar(term);
}

// Receive data. Blocks until byte is received or timeout is reached. Returns received byte, or 0xff or -1 if timed out.
volatile int Goon_Serial::Rx_timeout()
{
   return fdserial_rxTime(term, RX_TIMEOUT_MS);
}

// Receive data. Blocks until byte is received or timeout is reached. Returns received byte, or 0xff or -1 if timed out.
volatile int Goon_Serial::Rx_timeout(int ms)
{
   return fdserial_rxTime(term, ms);
}

// Receive data. Does not block. Returns byte in receive buffer, or 0xff or -1 if buffer empty.
volatile int Goon_Serial::Rx_check()
{
   return fdserial_rxCheck(term);
}

// Flush rx and tx buffers, then close the connection
volatile void Goon_Serial::Close()
{
   fdserial_close(term);
}
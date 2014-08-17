#ifndef BadgeIO_Class_Defined__
#define BadgeIO_Class_Defined__

#include "Simple_Serial.h"
#include "fdserial.h"
#include "../libs/screen.h"
#include "../libs/Simple_Numbers.h"
#include "../libs/goon_serial.h"


#define RX_PIN 4 // 27 PS2 // 31 USB
#define TX_PIN 2 // 26 PS2 // 30 USB
#define NEW_GAME_PIN 0
#define IO_LIGHT_PIN 16

class BadgeIO
{
  public:
    static const int _Clkmode = 1032;
    static const int _Xinfreq = 5000000;
    BadgeIO();
    void Init();
    int* WaitAndRead(screenSpin SC);
    void GenerateDataToSend();
    void NewGame();
    void Close();
  private:
    Goon_Serial goon_serial;
    // Simple_Numbers SN; //for outputting numbers
    int* ReadUntilEnd(screenSpin SC);
    void TransmitAllClear();
    uint32_t _currentGame;
    uint32_t _currentPlayer;
};


#endif

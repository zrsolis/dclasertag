#include <propeller.h>
#include "BadgeIO.h"
#include "Simple_Serial.h"
#include "settings.h"
#include "simpletools.h"
#include "../libs/BitArray.h"
#include "../libs/player.h"


BadgeIO::BadgeIO()
{
  _currentGame=0;
  _currentPlayer=0b000000001;
}

void BadgeIO::Init()
{
  goon_serial.Init(RX_PIN, TX_PIN);
}

int* BadgeIO::WaitAndRead(screenSpin SC)
{
  Init();

  int message;
  int connected = 0;
  while (!connected)
  {
    goon_serial.Tx(0xDC);
    // TransmitAllClear();
    pause(10);
    message = goon_serial.Rx_timeout();
    if (message == 0xDC)
      connected = 1;
    //else
      //pause(100);
  }

  return ReadUntilEnd(SC);
}


void BadgeIO::GenerateDataToSend()
{
  _currentPlayer++;
  high(IO_LIGHT_PIN);
  goon_serial.Tx(_currentGame);
  goon_serial.Tx(_currentPlayer);
  low(IO_LIGHT_PIN);
}

void BadgeIO::NewGame()
{
  _currentGame++;
}

void BadgeIO::Close()
{
  goon_serial.Close();
}

/*********************************PRIVATE*************************/

int* BadgeIO::ReadUntilEnd(screenSpin SC)
{
  //receive 16 bits: 3 bits for game id, 2 hit type, 9 player id, 1 checksum, 1 who cares ignore it like the orphan child it is bit.
  high(IO_LIGHT_PIN);
  int data = goon_serial.Rx_block();
  low(IO_LIGHT_PIN);
  Close();

  // Print data
  SC.Init();
  SC.Clear();
  SC.Position(0,0);
  if (data == 0)
    SC.Print("data Zero");
  else
    SC.Print("data Nonzero");
  // SC.Close();


  bool* gameID = (bool*)BitArray::ToBool(data, 0, NUM_GAME_BITS);
  bool* hit = (bool*)BitArray::ToBool(data, NUM_GAME_BITS, NUM_HIT_TYPE_BITS);
  bool* teamID = (bool*)BitArray::ToBool(data, NUM_GAME_BITS+NUM_HIT_TYPE_BITS, NUM_TEAM_BITS);
  bool* playerID = (bool*)BitArray::ToBool(data, NUM_GAME_BITS+NUM_HIT_TYPE_BITS+NUM_TEAM_BITS, NUM_PLAYERID_BITS);
  bool* checkSum = (bool*)BitArray::ToBool(data, NUM_GAME_BITS+NUM_HIT_TYPE_BITS+NUM_TEAM_BITS+NUM_PLAYERID_BITS, NUM_CHECKSUM_BITS);
  SC.Position(1,0);
  if (Player::GetChecksum(gameID,hit,teamID,playerID) != checkSum[0])
  {
    SC.Print("Failed error check");
  }
  else
  {
    SC.Print("Passed error check");
  }
  pause(1000);
/*
  //check for errors
  SC.Position(1,0);
  if ( (data%2 == 0) && ((data & 0x02<<8)!=2) ) // even
  {
    return 0;
    SC.Print("Failed error check");
    pause(1000);
  }
  else if ( (data%2 == 1) && ((data & 0x2<<8)!=0) ) // odd
  {
    return 0;
    SC.Print("Failed error check");
    pause(1000);
  }
*/

  SC.Close();

  int parsedData[3];
  parsedData[0]=data >> 13; //Game Id
  parsedData[1]=(data >>11) & 0b00000011; //Hit Type
  parsedData[2]=(data >>2) & 0b00000111111111; //Player Id

  // TransmitAllClear();
  
  return parsedData;
  
}


void BadgeIO::TransmitAllClear()
{
  goon_serial.Tx(0xFF);
}


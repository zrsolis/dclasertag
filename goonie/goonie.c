#include "../libs/screen.h"
#include "simpletools.h"
#include "../libs/BadgeIO.h"
#include "simpletext.h"
#include "simplei2c.h"

void SaveData(int,screenSpin);
void ReadData(screenSpin SC);



int main()
{

  pause(2000);
  screenSpin SC;
  SC.Init();
  SC.Clear();
  SC.Mediainit();
  SC.Setbyteaddr(0,0,0,0);
  SC.Displayimage();
  pause(1000); 
  SC.Fadeout();
  SC.Fadein();
  SC.Clear();  
  SC.Position(0,3);
  SC.Fontsize(2);
  SC.Print("DC530");
  SC.Position(1,0);
  SC.Print("LazorTag");
  SC.Position(2,2);
  SC.Print("Beta");
  SC.Position(3,3);
  SC.Print("1.1");
  SC.Position(0,0);
  pause(3000);
  SC.SetsectorScores();
  SC.Clear();

  SC.SetsectorScores();

  BadgeIO comms;

  comms.NewGame();
  while(!input(0))//button press for new game
  {

    SC.Clear();
    SC.Position(0,0);
    SC.Print("Please plug in");
    SC.Position(1,0);
    SC.Print("Badge now:");
    SC.Position(2,0);
    SC.Close();

    // get player information
    int* data=comms.WaitAndRead(SC);
    
    if(data[2]!=0) //if player id != 0
    {
      //save
      //comms.Close();
      SC.Init();
      SaveData(*data,SC);
      SC.Savestr("$");
    }
    SC.Clear();
    SC.Position(0,0);
    SC.Print("Saved");
    SC.Close();

    //high(15);
    comms.Init();
    comms.GenerateDataToSend();
    comms.Close();
    
    SC.Init();
    SC.Clear();
    SC.Position(0,0);
    SC.Print("Data Sent");


    while (!input(RX_PIN)); // wait until disconnected
    //SC.Init();
    SC.Clear();
    //SC.Position(0,0);
    //SC.Print("Game id: ");
    //SC.PrintInt(data[0]);
    SC.Position(1,0);
    SC.Print("Hit type: ");
    SC.PrintInt(data[1]);
    SC.Position(2,0);
    SC.Print("Player id: ");
    SC.PrintInt(data[2]);
    
    
  }

  //loop end, flush SD card to signify game end
  SC.Flush();
  SC.SetsectorScores();
  ReadData(SC);
  return 0;
}


void SaveData(int playerData, screenSpin SC)
{
  //16 bytes: 3 bits for game id, 2 hit type, 9 player id, 1 checksum, 1 who cares ignore it like the orphan child it is bit.
  SC.Init();
  short part=playerData >>8;
  SC.Writebyte(part);
  part=playerData<<8;
  part=part>>8;
  SC.Writebyte(part);
}

void ReadData(screenSpin SC)
{
  SC.Init();
  int reads=0x00;
  int linePos=0;
  while(reads != 0xFF)  
  {
    reads=SC.Readbyte();
    if(reads=='$')
    {
      //new record!
      linePos++;
      SC.Position(linePos,0);
    }
    SC.PrintInt(reads);
  }
}
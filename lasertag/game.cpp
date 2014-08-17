//
// DC530
// Lazer tag game
//
#include "fdserial.h"
#include "simpletools.h"
#include "simpletext.h"
#include "simplei2c.h"

#define salem_is_a_dick 1
#define salem_is_holy 0

#include "game.h"
//#include "../libs/Simple_Serial.h"
#include "../libs/BitArray.h"
#include "../libs/goon_serial.h"

//cog functions
void task_record_hits(void* arg);
void task_led_control(void* arg);
void task_record_hits(void* arg);

//helper functions
void IR_out(bool ir_code[]);
void UpdateIrCode(bool ir_code[]);
//void watch_usb(); //no longer used?
void goon_comm(bool * ir_code, Goon_Serial goon_serial);



///////////////////////////////////////////////////////////////////////////////
//
// main function
//
int cogID_led=0;
int cogID_hits=0;

int main()
{
   low(IR_OUT_PIN);
   //Allows print() from cogs
   extern text_t *dport_ptr; // default debug port pointer gets reassigned to fdserial.
   simpleterm_close();
   dport_ptr = (fdserial_open(31,30,0,115200));

   Goon_Serial goon_serial; // serial object to communicate with goon badge

   //for testing
   //bool gameID[NUM_GAME_BITS] = {0, 1, 0};
   //bool playerID[NUM_PLAYERID_BITS] = {1, 0, 0, 1, 1, 0, 0, 1};
   //bool teamID[NUM_TEAM_BITS] = {0};

   //Only one of these should be uncommented.
   //Player::Init(gameID, playerID, teamID);    //For debugging
   Player::Init();                          //For gameplay

   print("Game ID: ");
   BitArray::Print((bool*)Player::PlayerGameID(), NUM_GAME_BITS);
   print("\nPlayer ID: ");
   BitArray::Print((bool*)Player::PlayerID(), NUM_PLAYERID_BITS);
   print("\nTeam ID: ");
   BitArray::Print((bool*)Player::TeamID(), NUM_TEAM_BITS);
   print("\nHit Type: ");
   BitArray::Print((bool*)Player::HitType(), NUM_HIT_TYPE_BITS);
   print("\nChecksum: %d\n", Player::GetChecksum((bool*)Player::PlayerGameID(), (bool*)Player::HitType(), (bool*)Player::TeamID(), (bool*)Player::PlayerID()));
   print("Health: %d\n", Player::GetHealth() );

   //initialize variables
   hit_flag = salem_is_holy;   //flag from main to team led to start blinking when shot
   hit_blink_flag = salem_is_holy;

   //static int ir_in_cog_stack[STACKSIZE];
   //static int led_control_cog_stack[2000];
   //static int record_hits_cog_stack[2000];

   static int led_control_cog_stack = 500;
   int *led_stack = (int*) malloc(led_control_cog_stack);

   static int record_hits_cog_stack = 500;
   int *hits_stack = (int*) malloc(record_hits_cog_stack);

   int arg = 0;

   //cogstart(&task_record_hits, (void*) arg, ir_in_cog_stack, sizeof(ir_in_cog_stack));
   cogID_led = cogstart(&task_led_control, (void*) arg, led_stack, led_control_cog_stack);
   cogID_hits = cogstart(&task_record_hits, (void*) arg, hits_stack, record_hits_cog_stack);

   //16 bytes: 3 bits for game id, 2 hit type, 9 player id, 1 checksum, 1 who cares ignore
   bool ir_code[16];

   UpdateIrCode(ir_code);

   goon_serial.Init(RX_PIN, TX_PIN); // int32_t Rxpin, int32_t Txpin, int32_t Baud

   //main game loop
   while(salem_is_a_dick)
   {

      //low(GOON_TX_PIN); // Keep low, turn high to signal connection

      //We decided to use the main thread for processing the IR out.
      //This is mainly to reduce the amount of cogs being used.
      //However, this main loop was just empty cycles anyways.
      //print("IR out\n");
      IR_out((bool*)ir_code);

      
      //We can throw the usb in this loop because it takes hardly any time to check if the USB is connected.
      //If the USB is connected, we are going to block all threads anyways.
      //print("Starting recieve code\n");
      int recvd_code = goon_serial.Rx_check();
      //print("Finished recieve code\n");
      if (recvd_code == 0xDC) // need to disable after done until connected. debounce?
      {
         //stop the cogs while doing this processing
         cogstop(cogID_led);
         cogstop(cogID_hits);

         print("Starting goon communication\n");
         goon_comm(ir_code, goon_serial);

         //We need to update the ir code after goon communications
         //There might be new values for the player/game
         UpdateIrCode(ir_code);
         print("Updated IR code\n");

         //restart the cogs
         cogstart(&task_led_control, (void*) arg, led_stack, led_control_cog_stack);
         cogstart(&task_record_hits, (void*) arg, hits_stack, record_hits_cog_stack);
         print("Started cogs\n");

         goon_serial.Init(RX_PIN, TX_PIN);

      }
      else
      {
        //print("Recieved code: %d\n", recvd_code);
      }
   }

   return 0;

}


///////////////////////////////////////////////////////////////////////////////
//
// helpers
//

void UpdateIrCode(bool ir_code[])
{
   bool one[1] = {1};
   bool checksum[1] = {Player::GetChecksum((bool*)Player::PlayerGameID(),
                                           (bool*)Player::HitType(),
                                           (bool*)Player::TeamID(),
                                           (bool*)Player::PlayerID())};    

   // set game id
   memcpy(ir_code, (bool*)Player::PlayerGameID(), NUM_GAME_BITS);
   // set hit type
   memcpy(ir_code + NUM_GAME_BITS, (bool*)Player::HitType(), NUM_HIT_TYPE_BITS);
   // set team
   memcpy(ir_code + NUM_GAME_BITS + NUM_HIT_TYPE_BITS, (bool*)Player::TeamID(), NUM_TEAM_BITS);
   // set playerID
   memcpy(ir_code + NUM_GAME_BITS + NUM_HIT_TYPE_BITS + NUM_TEAM_BITS, (bool*)Player::PlayerID(), NUM_PLAYERID_BITS);
   // set checksum
   memcpy(ir_code + NUM_GAME_BITS + NUM_HIT_TYPE_BITS + NUM_TEAM_BITS + NUM_PLAYERID_BITS, (bool*)checksum, NUM_CHECKSUM_BITS);
   // set a random bit
   memcpy(ir_code + NUM_GAME_BITS + NUM_HIT_TYPE_BITS + NUM_TEAM_BITS + NUM_PLAYERID_BITS + NUM_CHECKSUM_BITS, (bool*)one, 1);
}

void IR_out(bool ir_code[])
{
    if (!input(TRIGGER_PIN) && Player::IsAlive())
    {

      const int frequency = 38000;

      print("Trigger Pressed\n");

      int loop_size = NUM_TEAM_BITS + NUM_HIT_TYPE_BITS + NUM_PLAYERID_BITS + NUM_GAME_BITS + NUM_CHECKSUM_BITS + NUM_RANDOM_BITS;

      print("Sending IR Code: ");
      BitArray::Print(ir_code, loop_size);
      print("\n");

      //send ir code
      freqout(IR_OUT_PIN, 1, frequency);
      pause(10);

      for(int loop = 0; loop <= loop_size; loop++)
      {
        //print("%d", ir_code[loop]);
        if (ir_code[loop])
        {
			   freqout(IR_OUT_PIN, 1, frequency);
			   pause(3);
        } else {
			   freqout(IR_OUT_PIN, 1, frequency);
			   pause(1);
        }
      }
      low(IR_OUT_PIN);
      //print("Done freqout\n");

      //limit the rate of fire
      pause(RATE_OF_FIRE);
    }
}

void goon_comm(bool * ir_code, Goon_Serial goon_serial)
{
   // send signal to goon badge to get ready to recieve
   // low(GOON_TX_PIN);
   

   // open serial connection
   
   goon_serial.Tx(0xDC);
   pause(10);


   // send my ir code
   print("Sending my IR code: ");
   BitArray::Print(ir_code, 16);
   goon_serial.Tx(BitArray::ToInt(ir_code, 16));
   //goon_serial.Tx(0xFF);
   // recieve 0xFF?


   /*
   // send number of hits (or just transmit all clear after done?)
   int num_hits = Player::GetNumberTimesHit();
   goon_serial.Tx(num_hits);

   // send hit codes (still send if new game?)
   for (int i = 1; i <= num_hits;) // for each hit, send hit code
   {
      int hit_code = BitArray::ToInt((bool*)Player::GetEnemyPlayerID(i), NUM_PLAYERID_BITS);
      goon_serial.Tx(hit_code);
      // recieve 0xFF?
   }
   */
   // transmit 0xFF?

   // receive game id
   int new_game_id = goon_serial.Rx_block();
   if ( (new_game_id == 0xFF) || (new_game_id == -1) )
      print("New game id: Rx timeout");
   else
      print("New game id: %d\n", new_game_id);

   // receive player id
   int new_player_id = goon_serial.Rx_block();
   if ( (new_player_id == 0xFF) || (new_player_id == -1) )
      print("New player id: Rx timeout");
   else
      print("New player id: %d\n", new_player_id);

   goon_serial.Close();

   // convert to bool arrays
   bool * new_game_id_bool;
   bool * new_player_id_bool;
   bool * new_team_id_bool; // team id is part of player id, current code uses least significant digit
   new_game_id_bool = (bool*)BitArray::ToBool(new_game_id, NUM_GAME_BITS);
   new_player_id_bool = (bool*)BitArray::ToBool((new_player_id >> 1), NUM_PLAYERID_BITS);
   new_team_id_bool = (bool*)BitArray::ToBool((new_player_id % 2), NUM_TEAM_BITS);
   
   print("New game id bool: ");
   BitArray::Print(new_game_id_bool, NUM_GAME_BITS);
   print("\nNew player id bool: ");
   BitArray::Print(new_player_id_bool, NUM_PLAYERID_BITS);
   print("\nNew team id bool: ");
   BitArray::Print(new_team_id_bool, NUM_TEAM_BITS);
   print("\n");

   print("Setting new information:\n");
   Player::SetPlayerGameID(new_game_id_bool);
   Player::SetPlayerID(new_player_id_bool);
   Player::SetTeamID(new_team_id_bool);
   Player::RestoreHealth();

   print("   Game id: ");
   BitArray::Print((bool*)Player::PlayerGameID(), NUM_GAME_BITS);
   print("\n   Player id: ");
   BitArray::Print((bool*)Player::PlayerID(), NUM_PLAYERID_BITS);
   print("\n   Team id: ");
   BitArray::Print((bool*)Player::TeamID(), NUM_TEAM_BITS);
   print("\n   Health: %d\n", Player::GetHealth());

   delete new_game_id_bool;
   delete new_player_id_bool;
   delete new_team_id_bool;


   /* For old method using manual setting of pin
   pause(100); // wait 1/10 sec for goon badge to set pin low
   print("Waiting for disconnect\n");
   while (!input(GOON_RX_PIN)); // wait until disconnected
   */

   // TODO: shield for a time after connecting to goon badge?

}

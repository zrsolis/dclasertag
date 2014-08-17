#ifndef IR_IN
#define IR_IN

#include <propeller.h>
#include "simpletools.h"
#include "../settings/settings.h"
#include "../libs/player.h"
#include "../libs/BitArray.h"
#include "pins.h"

//Thread for watching IR (getting hit)
void task_record_hits(void* arg)
{

int a;
//int b; //unused?
int c;
bool d[16];

  while(1) {
         c = 0;
  	while(pulse_in(IR_IN_PIN, 0) != 0) {                           // Check if there is a pulse on the IR
  		//high(16);                                        // DEBUG: Turns light on signaling RCV
  		a = rc_time(IR_IN_PIN, 1);
  		if (a > 8000 && a<12000){			       // Checking if received signal is 10ms ACK signal
  			while(c<16){
  				a = rc_time(IR_IN_PIN, 1);
  				//print("%d\n", a);                  //DEBUG: Print output for timings to console
                pause(1);
  				if (a < 1000 && a>50) {		// If signal received is between 50us and 1000us, this is a logical LOW
  					d[c] = 0;
  					c++;
  				}
  				if (a < 3000 && a>2000) {	       // If signal received is between 2000us and 3000us, this is a logical HIGH
  					d[c] = 1;
  					c++;
  				}
  				if (a >=249987) {			// Sometimes the IR rcv gets stuck in a loop with the maximum value, this breaks the loop
          	c = 16;
  				}
  				pause(1);
  			}

            // Print output of bit pattern to console for verification, will not be needed in final code, for testing only
            print("Received IR code: ");
            BitArray::Print(d, 16);
            print("\n");

  			bool gameid[NUM_GAME_BITS];
  			memcpy(gameid, d, NUM_GAME_BITS);

  			bool hit[NUM_HIT_TYPE_BITS];
  			memcpy(hit, d + NUM_GAME_BITS, NUM_TEAM_BITS);

  			bool team[NUM_TEAM_BITS];
  			memcpy(team, d + NUM_GAME_BITS + NUM_HIT_TYPE_BITS, NUM_TEAM_BITS);

  			//verify same game
			if(BitArray::bitArraySame(gameid, Player::PlayerGameID(), NUM_GAME_BITS))
			{
		    	 bool* playerID[NUM_PLAYERID_BITS];
		    	 memcpy(playerID, d + NUM_GAME_BITS + NUM_HIT_TYPE_BITS + NUM_TEAM_BITS, NUM_PLAYERID_BITS);

           if(Player::GetChecksum(gameid, hit, team, (bool*)playerID) == d[NUM_BITS_FOR_CHECKSUM + 1])
           {
             Player::BadgeHit(team, *playerID);
			    }
			   else
			   {
				   print("Checksum did not match!\n");
			   }
        }
        else
        {
           print("Hit... but not in same game\n");
        }
  		}
  	}
  }
}
#endif

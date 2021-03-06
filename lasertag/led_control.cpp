#ifndef LED_CONTROL
#define LED_CONTROL

#include "game.h"
#include "ws2812_libraries/ws2812.h"
//#include <stdint.h> //What is this used for?
#include <propeller.h>
#include "simpletools.h"
#include "../settings/settings.h"

void blink_shot(int shot_blink_frame_counter);
int blink_white(int error_blink_frame_counter);
void update_health_leds();

//Thread for controlling LEDs
void task_led_control(void* arg)
{
   uint32_t colors[LED_COUNT];
   
   memset(colors, 0, sizeof(colors));

   if (ws2812_init(TEAM_LED_PIN, colors, LED_COUNT) < 0)
      return;

   //blink variables
   int error_blink_frame_counter = 0;        //counts number of frames completed
   int shot_blink_frame_counter = 0;         //counts number of frames completed
   //bool shot_blink = false;

   int team = 0;

   //Is there a better way to add these together?
   bool* team_array = (bool*)Player::TeamID();
   for (unsigned int i = 0; i < sizeof(team_array); i++)
   {
      team = (team << 1) + (int)team_array[i];
   }

   switch(team)
   {
     
      case 0:     // No team (blink white)
         error_blink_frame_counter = blink_white(error_blink_frame_counter); //controls whether blink is on/off, then increments/resets counter by returning
      break;
       
      case 1:     // Red
         led_cur_color = COLOR_RED;
         blink_shot(shot_blink_frame_counter);
      break;
       
      case 2:     // Blue
         led_cur_color = COLOR_BLUE;
         blink_shot(shot_blink_frame_counter);
      break;
       
      case 3:     // Green/Error
         if (NUM_TEAMS >= 3)
         {
            led_cur_color = COLOR_GREEN;
         }
         else     //Error (blink white)
         {
            error_blink_frame_counter = blink_white(error_blink_frame_counter); //controls whether blink is on/off, then increments/resets counter by returning
         }
      break;

      default:    // Error (blink white)
         error_blink_frame_counter = blink_white(error_blink_frame_counter); //controls whether blink is on/off, then increments/resets counter by returning
      break;
   } 

   ws2812_fill(colors, LED_COUNT, led_cur_color);

   while(1)
   {
      update_health_leds();
      pause(1000);
   } // main while loop      
}

int blink_white(int error_blink_frame_counter)
{
   if (error_blink_frame_counter < ERROR_BLINK_FRAMES/2)
      led_cur_color = COLOR_WHITE;
   else
      led_cur_color = COLOR_BLACK;
   return ( (error_blink_frame_counter >= ERROR_BLINK_FRAMES) ? 0 : (error_blink_frame_counter+1) ); //if ebfc >= EBF, set to 0, otherwise increment

}



void blink_shot(int shot_blink_frame_counter)
{
//TODO: Do we want the light to blink when fired? Which light?
/*


   //code goes here
   if (shot_flag) //is true if have been shot but hasn't gone through this code yet
   {
      shot_blink = true;
      shot_blink_frame_counter = 0;
      shot_flag = false;
   }
   if (shot_blink)      //while true, led is blinking
   {

      // blink in first half of blink frames
      if ( (shot_blink_frame_counter % SHOT_BLINK_FRAMES) > (SHOT_BLINK_FRAMES / 2) )
      {
         //blink is off
         led_cur_color = COLOR_BLACK;
      }  //else do nothing (color is already set)


      //if done blinking, turn off flag and reset; if not done, increment counter
      if ( shot_blink_frame_counter >= (SHOT_BLINK_FRAMES * SHOT_NUM_BLINKS) )
      {
         shot_blink_frame_counter = 0;
         shot_blink = false;
      }
      else
      {
         shot_blink_frame_counter++;
      }

   }

*/
}



void update_health_leds()
{
   //8 leds, 0 (low) to 7 (high)
   //HEALTH_LED_7_PIN is the lowest pin number representing our health  
   for (int i = 0; i < MAX_HEALTH; i++)
   {
      if (i < Player::GetHealth())
      {
         //health_led_array[i] = 1;
         high(HEALTH_LED_7_PIN + i);
      }
      else
      {
         low(HEALTH_LED_7_PIN + i);
      }
   }
}

#endif
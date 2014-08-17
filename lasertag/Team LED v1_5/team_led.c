//#include <stdint.h>
#include <propeller.h>
#include "ws2812.h"
#include "team_led.h"

#define LED_PIN     0
#define LED_COUNT   1


uint32_t led_cur_color = COLOR_BLACK;


void team_led_function(void* par)
{


   uint32_t colors[LED_COUNT];
   int cog;
   

   memset(colors, 0, sizeof(colors));

   if ((cog = ws2812_init(LED_PIN, colors, LED_COUNT)) < 0)
      return 1;

   //blink variables
   int error_blink_frame_counter = 0;        //counts number of frames completed
   int shot_blink_frame_counter = 0;         //counts number of frames completed
   bool shot_blink = false;

   while(true)
   {

      switch(team)
      {
       
         case 0:     // No team (blink white)
            error_blink_frame_counter = blink_white(error_blink_frame_counter); //controls whether blink is on/off, then increments/resets counter by returning
         break;
         
         case 1:     // Red
            led_cur_color = COLOR_RED;
            blink_shot(int shot_blink_frame_counter);
         break;
         
         case 2:     // Blue
            led_cur_color = COLOR_BLUE;
            blink_shot(int shot_blink_frame_counter);
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

   } // main while loop

   cogstop(cog);
   return 0;
}






int blink_white(int error_blink_frame_counter)
{
   if (error_blink_frame_counter < ERROR_BLINK_FRAMES/2)
      led_cur_color = COLOR_WHITE;
   else
      led_cur_color = COLOR_BLACK;
   return ( (error_blink_frame_counter >= ERROR_BLINK_FRAMES) ? 0 : (error_blink_frame_counter+1) ); //if ebfc >= EBF, set to 0, otherwise increment

}



void blink_shot(int &shot_blink_frame_counter)
{
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
}
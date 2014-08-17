//
// DC530
// Lazer tag game
// game.h
//

#ifndef __GAME_H__
#define __GAME_H__

///////////////////////////////////////////////////////////////////////////////
//
// include statements
//
#include "fdserial.h"
#include <propeller.h>        // Propeller-specific functions
#include "simpletools.h"

//pins
#include "pins.h"
#include "../settings/settings.h"
#include "ws2812_libraries/ws2812.h"

#include "BadgeType.h"
#include "../libs/player.h"
#include "../libs/BitArray.h"

//#include "ir_in.cogc"
//#include "led_control.cogc"


///////////////////////////////////////////////////////////////////////////////
//
// define statements
//

// game/rules
#define TEAM_HIT_MODIFIER     1        // multiply damage by this if hit by own team
#define SHOT_DELAY            2        // delay before another shot can be fired (in seconds or main loop iterations?)

// neopixel
#define LED_COUNT             0        // pixel number of neopixel team led (should always be 0)
#define BRIGHTNESS            100      // brightness of neopixel (out of 100)
#define ERROR_BLINK_FRAMES    100      // blink speed before team assigned or if team number is not valid
				       //(in number of loops before 1 full blink is completed)
#define SHOT_BLINK_FRAMES     50       // blink speed when player is shot (in number of loops before 1 full blink is completed)
#define SHOT_NUM_BLINKS       5        // number of blinks when player is shot

//#define GOON_BAUD_RATE        9600

/*
In the comments for cogstart from the propeller.h include,
it says the stack size must be enough to hold the struct _thread_state_t,
the initial stack frame and other stack frames used by called functions.

The struct thread_state_t is 148 bytes.
According to the cogstart function, stack size should be > sizeof(thread_state_t) + 3*sizeof(unsigned int).
So, the minimum stack size would be 148+3*4 = 160 bytes. Stack size should also be evenly divisible by 4.
*/

#define STACKSIZE (160*4)/4


///////////////////////////////////////////////////////////////////////////////
//
// structures
//




///////////////////////////////////////////////////////////////////////////////
//
// global variables
//


//communication between cogs
//static volatile hit_code_t hit_codes [HIT_ARRAY_SIZE]; //use functions above
   //static volatile bool shooter_ids [5][16];
   //static volatile uint16_t shooter_ids [5];
static volatile bool hit_flag; //from ir to main
static volatile bool hit_blink_flag;
static volatile uint32_t colors[LED_COUNT]; //array of colors (by pixel) for ws2812 (from led control to ws2812 driver)
static volatile uint32_t led_cur_color = COLOR_BLACK;
static volatile int error_blink_frame_counter = 0;        // blink - counts number of frames completed
static volatile int shot_blink_frame_counter = 0;         // blink - counts number of frames completed
static volatile bool shot_blink = false;                  // blink - turns true while blinking after shot


///////////////////////////////////////////////////////////////////////////////
//
// forward declarations
//

//use .h file(s)? classes?  (these can use #define statements above?)
//#include "ir.h"
//#include "team_led.h"
//#include "base_station.h"
//
/*
void IR_out();
void goon_comm();
void team_led_blink_shot(int &shot_blink_frame_counter);
int team_led_blink_white(int error_blink_frame_counter);
void team_led_control();
void health_led_control();
*/
//void ir_recieve(void* par);

#endif //__GAME_H__

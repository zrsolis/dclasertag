#ifndef __TEAM_LED_H__
#define __TEAM_LED_H__

#include <propeller.h>



//function declarations
int blink_white(int error_blink_frame_counter);
void blink_shot(int &shot_blink_frame_counter);
void team_led_function(void* par);

#endif


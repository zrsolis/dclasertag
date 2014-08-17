#ifndef __PINS_H_
#define __PINS_H_

#define TRIGGER_PIN           24 // IN  from trigger button
#define IR_OUT_PIN            25 // OUT to gun IR (not badge)
#define IR_IN_PIN             12 // IN  from on-badge IR
/* hardcoded for simplicity
#define HEALTH_LED_0_PIN      23 // OUT Health LED bottom
#define HEALTH_LED_1_PIN      22 // OUT Health LED   |
#define HEALTH_LED_2_PIN      21 // OUT Health LED   |
#define HEALTH_LED_3_PIN      20 // OUT Health LED   |
#define HEALTH_LED_4_PIN      19 // OUT Health LED   |
#define HEALTH_LED_5_PIN      18 // OUT Health LED   |
#define HEALTH_LED_6_PIN      17 // OUT Health LED   V
*/
#define HEALTH_LED_7_PIN      16 // OUT Health LED  top
#define TEAM_LED_PIN          0  // OUT to ws2811 team led
#define RX_PIN                26 // PS/2 Data (USB RX = 14)
#define TX_PIN                27 // PS/2 Clk  (USB TX = 15)

#endif
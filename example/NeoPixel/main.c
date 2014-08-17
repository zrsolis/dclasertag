#include <stdint.h>
#include <propeller.h>
#include "ws2812.h"

#define LED_PIN     0
#define LED_COUNT   1

int ticks_per_ms;

void pause(int ms);

int main(void)
{
    uint32_t colors[LED_COUNT];
    int cog, i;
    
    ticks_per_ms = CLKFREQ / 1000;
    
    memset(colors, 0, sizeof(colors));
    
    if ((cog = ws2812_init(LED_PIN, colors, LED_COUNT)) < 0)
        return 1;
        
    for (;;) {
        ws2812_fill(colors, LED_COUNT, COLOR_RED);
        pause(1000);
        ws2812_fill(colors, LED_COUNT, COLOR_GREEN);
        pause(1000);
        ws2812_fill(colors, LED_COUNT, COLOR_BLUE);
        pause(1000);
    }
        
    cogstop(cog);
    
    return 0;
}

void pause(int ms)
{
    waitcnt(CNT + ms * ticks_per_ms);
}
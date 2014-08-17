'' Modified for use with PropGCC by David Betz
''  Derived from:
''   File....... jm_ws2812.spin
''   Purpose.... 800kHz driver for WS2812 LEDs
''   Author..... Jon "JonnyMac" McPhalen
''               Copyright (c) 2013 Jon McPhalen

{{
    // parameter structure
    typedef struct {
        uint32_t pin;
        uint32_t *colors;
        volatile uint32_t count;
    } ws2812_params;
    
    // driver header structure
    typedef struct {
        uint32_t    jmp_inst;
        uint32_t    resettix;
        uint32_t    bit0hi;
        uint32_t    bit0lo;
        uint32_t    bit1hi;
        uint32_t    bit1lo;
    } ws2812_hdr;
}}

pub driver
  return @ws2812

dat
                        org     0

ws2812                  jmp     #ws2812_cont

resettix                long    0                               ' frame reset timing
bit0hi                  long    0                               ' bit0 high timing
bit0lo                  long    0                               ' bit0 low timing
bit1hi                  long    0                               ' bit1 high timing    
bit1lo                  long    0                               ' bit1 low timing     

ws2812_cont             mov     t1, par                         ' get the pin number
                        rdlong  t2, t1
                        mov     txmask, #1                      ' create mask for tx
                        shl     txmask, t2
                        andn    outa, txmask                    ' set to output low
                        or      dira, txmask
                        add     t1, #4
                        rdlong  hubpntr, t1                     ' get the buffer address
                        add     t1, #4
                        rdlong  ledcount, t1                    ' get the led count
                        mov     t2, #0                          ' init is done
                        wrlong  t2, t1
                        
rgbmain                 mov     bittimer, resettix              ' set reset timing  
                        add     bittimer, cnt                   ' sync timer 
                        waitcnt bittimer, #0                    ' let timer expire                             
                        
                        mov     addr, hubpntr                   ' point to rgbbuf[0]
                        mov     nleds, ledcount                 ' set # active leds

frameloop               rdlong  colorbits, addr                 ' read a channel
                        add     addr, #4                        ' point to next

' Shifts long in colorbits to WS2812 chain
'
'  WS2812 Timing (slot is 1.25us for 800kHz)
'
'  0      0.35us / 0.80us
'  1      0.70us / 0.60us
'
'  At least 50us (reset) between frames
'
'  This routine manipulates the value in colorbits so that elements are
'  transmitted GRB as required by the WS2812. Rearrange-on-the-fly code
'  trick by TonyP12 in the Propeller forum.  
'
'  Entry      $00_RR_GG_BB
'  Step 1     $BB_00_RR_GG
'  Step 2-24  $GG_BB_00_RR - when nbits == 24
'             $BB_00_RR_GG - after sending GG
'  Step 2-16  $RR_GG_BB_00 - when nbits == 16
'             $GG_BB_00_RR - after sending RR
'  Step 2-8   $BB_00_RR_GG - when nbits == 8

shiftout                ror     colorbits, #8                   ' {1} to offset the rol 24 below
                        mov     nbits, #24                      ' shift 24 bits (3 x 8) 
                        
:loop                   test    nbits, #%111            wz      ' {2} nbits at 24, 16 or 8?
        if_z            rol     colorbits, nbits                ' if yes, modify colorbits 
                        rol     colorbits, #1           wc      ' msb --> C
        if_c            mov     bittimer, bit1hi                ' set bit timing  
        if_nc           mov     bittimer, bit0hi                
                        or      outa, txmask                    ' tx line 1  
                        add     bittimer, cnt                   ' sync bit timer  
        if_c            waitcnt bittimer, bit1lo                
        if_nc           waitcnt bittimer, bit0lo 
                        andn    outa, txmask                    ' tx line 0             
                        waitcnt bittimer, #0                    ' hold while low
                        djnz    nbits, #:loop                   ' next bit

                        djnz    nleds, #frameloop               ' done with all leds?

                        jmp     #rgbmain                        ' back to top

' --------------------------------------------------------------------------------------------------

hubpntr                 res     1                               ' pointer to rgb array
ledcount                res     1                               ' # of rgb leds in chain

txmask                  res     1                               ' mask for tx output

bittimer                res     1                               ' timer for reset/bit
addr                    res     1                               ' address of current rgb bit
nleds                   res     1                               ' # of channels to process
colorbits               res     1                               ' rgb for current channel
nbits                   res     1                               ' # of bits to process

t1                      res     1                               ' work vars
t2                      res     1

                        fit     496                                    
{{

  Terms of Use: MIT License

  Permission is hereby granted, free of charge, to any person obtaining a copy of this
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify,
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be included in all copies
  or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

}}

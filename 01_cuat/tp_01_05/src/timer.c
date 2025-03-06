#include "../inc/timer.h"
__attribute__((section("._text_timer_init")))void __timer_init()
{
   _timer_t* const TIMER0 = (_timer_t*)TIMER0_ADDR;

   TIMER0->Timer1Load = 0x0010000; // Configuro el registro que va a ser decrementado periodicamente desde el valor 65536 (0x10000)
   TIMER0->Timer1Ctrl = 0x0000002; // Timer 32 bits (Bit 1)
   TIMER0->Timer1Ctrl |=0x0000040; // Timer periodico (Bit 6)
   TIMER0->Timer1Ctrl |=0x0000020; // Interrupccion timer habilitada (Bit 5)
   TIMER0->Timer1Ctrl |=0x0000080; // Habilitacion TIMER (Bit 7)
}
//Extraccion manual ARM
/*
   PERIODIC: The counter operates continuously by reloading from the Load
   Register each time that the counter reaches zero.

   Periodic mode is selected by setting the following bits in the TimerControl Register:
   • set TimerMode   bit to 0   
   • set OneShot     bit to 0.

   The 32-bit or 16-bit counter operation is selected by setting the TimerSize bit
   appropriately in the TimerControl Register.

   In 32-bit mode, the full 32 bits of the counter are decremented and when the count
   reaches zero, 0x00000000 , an interrupt is generated and the counter reloads with the
   value in the TimerXLoad Register. The counter starts to decrement again and this whole
   cycle repeats for as long as the counter is enabled.

   The TimerXLoad Register is a 32-bit register that contains the value from which the
   counter is to decrement

   An interrupt is generated if IntEnable=1 and the counter reaches 0x00000000 in 32-bit

   Programming the timer interval -- See page 31 ARM Dual-Timer Module.pdf
*/

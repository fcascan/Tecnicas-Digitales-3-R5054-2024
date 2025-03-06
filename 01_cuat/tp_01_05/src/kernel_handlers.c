#include "../inc/gic.h"
#include "../inc/timer.h"
#include "../inc/scheduler.h"

#define INT_ID_MASK  0x3FF

extern Task *current_task;

uint8_t ticks;

uint8_t dirty = 1;

__attribute__((section("._kernel_handlers"),interrupt("naked")))void kernel_handler_irq()
{
    _gicc_t*    const GICC0     = (_gicc_t*)    GICC0_ADDR;
    _timer_t*   const TIMER0    = (_timer_t*)   TIMER0_ADDR;
   
    int id = (GICC0->IAR) & INT_ID_MASK;    //Los bits 0 a 9 de la Interrupt Acknowledge determinan el ID de interrupción
    
    switch (id) // Escaneo que fuente me interrumpio
    {
        case GIC_SOURCE_TIMER0: // Interrupcion generada porque la cuenta de TIMER0 llego a 0 
            ticks++;
            TIMER0->Timer1IntClr = 1;       // Borro el flag de interrupccion del timer (Ver pag 38)
            GICC0->EOIR |=  (GICC0->IAR) & id; // Copio la el id de la interrupcion en el registro 'end of interrupt' cuando haya terminado de atenderla
            if (dirty==0) {
                /*
                La primera vez que arranca el sistema pongo a dirty en '1' de esta manera solo se carga el stack inicializado direcatemnte 
                la vez que arranca el sistema
                */
                dirty = 1;
                ticks = 0;
            } 
            else {
                scheduler();    // Llamo al scheduler
            }
            cargar_contexto();
            break;
        default:
            break;
    }
}


__attribute__((section("._kernel_handlers"),interrupt("fiq")))void kernel_handler_fiq(){}
__attribute__((section("._kernel_handlers"),interrupt("undef")))void kernel_handler_undef(){}
__attribute__((section("._kernel_handlers"),interrupt("swi")))void kernel_handler_swi(){}
__attribute__((section("._kernel_handlers"),interrupt("abort")))void kernel_handler_pabt(){}
__attribute__((section("._kernel_handlers"),interrupt("svc")))void kernel_handler_dabt(){}
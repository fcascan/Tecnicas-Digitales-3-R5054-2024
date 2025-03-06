
#include "../inc/gic.h"
#include "../inc/timer.h"
#define INT_ID_MASK  0x3FF

int C_LLAM;         // Varaible declarada en startup.s si agrego un extern tira error 
char INV,MEM,SVC;   // Varaible declarada en startup.s si agrego un extern tira error 

__attribute__((section(".kernel_handlers"),interrupt("undef")))void kernel_handler_undef(){}

__attribute__((section(".kernel_handlers"),interrupt("swi")))void kernel_handler_swi(){}

__attribute__((section(".kernel_handlers"),interrupt("abort")))void kernel_handler_pabt(){}

__attribute__((section(".kernel_handlers"),interrupt("abort")))void kernel_handler_dabt(){}

__attribute__((section(".kernel_handlers"),interrupt("naked")))void kernel_handler_irq()
{
    _gicc_t*    const GICC0     = (_gicc_t*)    GICC0_ADDR;
    _timer_t*   const TIMER0    = (_timer_t*)   TIMER0_ADDR;

    int C_LLAM2;
    int id = (GICC0->IAR) & INT_ID_MASK;

    switch (id) { // Escaneo que fuente me interrumpio
        case GIC_SOURCE_TIMER0: // Interrupccion generada porque la cuenta de TIMER0 llego a 0 
            C_LLAM++;
            asm volatile (
            "ldr %[C_LLAM2], %[C_LLAM]\n"   //Cargo el valor de C_LLAM en C_LLAM2
            "mov r10, %[C_LLAM2]\n"         //Guardo el valor de C_LLAM2 en R10 
            : [C_LLAM2] "=r" (C_LLAM2)      //Output operand C_LLAM2
            : [C_LLAM] "m" (C_LLAM)         //Input operand: C_LLAM
            );
            TIMER0->Timer1IntClr = 1;       // Borro el flag de interrupccion del timer (Ver pag 38)
            break;
        default:
            break;
    }
    /*OJO HACER MASCARA Y NO PISAR EL VALOR EN EL REGISTRO!!*/
    GICC0->EOIR |=  (GICC0->IAR) & id; // Copio la el id de la interrupccion en el registro 'end of interrupt' cuando haya terminado de antenderla
}

/*
GICC0->IAR -> The processor acquires the interrupt number by
reading this register from the interrupting GIC.Pre-empted interrupts are recorded as active.
GICC0->EOIR -> When the interrupt has been completed by the processor, it writes the interrupt number to this
register in the interrupting GIC.
*/
__attribute__((section(".kernel_handlers"),interrupt("fiq")))void kernel_handler_fiq(){}

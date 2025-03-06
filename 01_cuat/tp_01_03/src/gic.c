#include "../inc/gic.h"
__attribute__((section(".text_gic_init")))void __gic_init()
{
    _gicc_t* const GICC0 = (_gicc_t*) GICC0_ADDR;
    _gicd_t* const GICD0 = (_gicd_t*) GICD0_ADDR;

    GICC0->PMR           = 0x000000F0;// Interrupcciones con prioridad mas alta que 0xF0 no estan enmascaradas
    GICD0->ISENABLER[1] |= 0x00000010;// 0b00000000000000000000000010000 Habilito timer 0 y 1
    //GICD0->ISENABLER[1] |= 0x00001000; 
    GICC0->CTLR          = 0x00000001;// Habilito el controlador de int
    GICD0->CTLR          = 0x00000001;// Habilito las interrupcciones para el GIC0
}
/*
    Extraccion del manual 
    Pag 168: 
    GICC0->CTLR
    b0 = disable the CPU interface for this GIC  
    b1 = enable the CPU interface for this GIC
    En nuestro caso habilitado
    Pag 169: 
    GICC0->PMR
    The Priority mask is used to prevent interrupts from
    being sent to the processor. The CPU Interface asserts an
    an interrupt request if the priority of the highest pending
    interrupt sent by the Distributor is greater than the
    priority set in the Priority mask field.
    For example:
    0x0 means all interrupts are masked.
    A Priority mask value of 0xF means interrupts with
    priority 0xF are masked but interrupts with higher
    priority values 0x0 to 0xE are not masked.
    ~~~~Distributor Memory map is on P.176~~~~
    Pag 178:
    GICD0->CTLR
    b0 = interrupts are disable for this GIC
    b1 = interrupts are enabled for this GIC
    Pag 180:
    GICD0->ISENABLER[1]
    Read this register to determine which interrupts are
    enabled. Bits 0 to 31 correspond to PB-A8 interrupt
    input lines 32 to 63 respectively. A bit set to 1 indicates
    an enabled interrupt. Write a 1 to a bit to enable the
    corresponding interrupt. Use Read-Modify-Write to
    maintain reserved interrupt states.
    See GIC interrupt allocation on page 4-62 for details of
    the PB-A8 external interrupt sources
    See page 162 Interrup ID table

*/
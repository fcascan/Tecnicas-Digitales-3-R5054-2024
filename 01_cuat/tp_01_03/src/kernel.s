.code 32
@Constantes 
.EQU IRQ_MODE, 0b10010 
.EQU FIQ_MODE, 0b10001 
.EQU SVC_MODE, 0b10011 
.EQU ABT_MODE, 0b10111 
.EQU UND_MODE, 0b11011 
.EQU SYS_MODE, 0b11111 
.EQU I_BIT, 0b10000000      @Recordar que con '1' deshabilito las interrupcciones por IRQ
.EQU F_BIT, 0b01000000      @Recordar que con '1' deshabilito las interrupcciones por FIQ
.EQU EN_IRQ,0b00010011     @Habilito FIQ & IRQ
@Definicion globales de etiquetas
.GLOBAL _kernel_start
@~~~~~~~~Nueva seccion~~~~~~~~
.SECTION ._kernel_code, "a"

_kernel_start:

    /*Se vuelve al modo SVC HABILITANDO IRQ*/

    MSR cpsr_c, #(SVC_MODE)  

    MRS R0, CPSR          @ Lee el valor actual del registro CPSR en R0
    MOV R1, #0xC0         @ Máscara con los bits 6 y 7 en 1 (11000000 en binario)
    BIC R0, R0, R1        @ Aplica la operación AND bit a bit con la máscara
    MSR CPSR_c, R0        @ Escribe el valor modificado de vuelta en el registro CPSR
    
   
_idle:
    wfi
    SWI 90
    B _idle

.END

@ graph display `x /20xw 0x70030000`

.code 32
@Constantes 
.EQU IRQ_MODE, 0x00000012 
.EQU FIQ_MODE, 0x00000011 
.EQU SVC_MODE, 0x00000013 
.EQU ABT_MODE, 0x00000017 
.EQU UND_MODE, 0x0000001b 
.EQU SYS_MODE, 0x0000001f 
.EQU I_BIT, 0x00000080      @Recordar que con '1' deshabilito las interrupcciones por IRQ
.EQU F_BIT, 0b01000000      @Recordar que con '1' deshabilito las interrupcciones por FIQ
.EQU EN_IRQ,0x00000060      @Habilito FIQ & IRQ


.equ DIR_NO_PAGINADA, 0x90000000
@Definicion globales de etiquetas
.GLOBAL _kernel_start
@~~~~~~~~Nueva seccion~~~~~~~~
.SECTION ._kernel_code, "a"

_kernel_start:
  
_idle:
   @LDR R0, =#0x10
   @SWI 90
    WFI
    
   
@Error tipo data abort: Quiero leer una direccion de memoria no paginada!
@_data_abt: 
   @LDR R0, =DIR_NO_PAGINADA
   @LDR R1, [R0]
   @B _idle
@Error tipo prefectch abort: Quiero saltar a una direccion de memoria no paginada
@_prefectch_abt:  
   @MOV R10, #0x78000000
   @MOV LR,PC
   @BX R10
    B _idle
.END

@ graph display `x /20xw 0x70030000`

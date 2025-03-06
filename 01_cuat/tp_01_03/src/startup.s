.CODE 32

@ Referencias a variables o simbolos externos
.EXTERN _PUBLIC_STACK_INIT
.EXTERN _PUBLIC_RAM_INIT
.EXTERN _PUBLIC_ROM_INIT
.EXTERN _PUBLIC_INT_INIT
.EXTERN __table_size__             @Longitud de la tabla
.EXTERN __text_table_init__        @Posicion de memoria donde arranca la tabla de interrupcciones
.EXTERN __c_functions_size__       @Longitud de la seccion c_functions
.EXTERN __c_functions_ram_init__   @Posicion de inicio en RAM
.EXTERN __c_functions_rom_init__   @Posicion de inicio en ROM
.EXTERN __text_memcopy_size__      @Longitud del memcopy
.EXTERN __text_memcopy_rom_init__  @Donde arranca memcopy en ROM
.EXTERN __handler_code_ram_init__  @Donde arrancan los handlers en RAM
.EXTERN __handler_code_rom_init__  @Donde arrancan los handlers en ROM
.EXTERN __kernel_code_rom_init__   @Donde arrancan el kernel en ROM
.EXTERN __kernel_code_ram_init__   @Donde arrancan el kernel en RAM
.EXTERN __kernel_code_size__       @Longitud del kernel
.EXTERN __irq_stack_top__
.EXTERN __fiq_stack_top__
.EXTERN __svc_stack_top__
.EXTERN __abt_stack_top__
.EXTERN __und_stack_top__
.EXTERN __sys_stack_top__

@Funciones en C
.EXTERN td3_memcpy
.EXTERN __gic_init
.EXTERN __timer_init

@Constantes 
.EQU IRQ_MODE,       0x00000012
.EQU FIQ_MODE,       0x00000011
.EQU SVC_MODE,       0x00000013
.EQU ABT_MODE,       0x00000017
.EQU UND_MODE,       0x0000001b
.EQU SYS_MODE,       0x0000001f
.EQU I_BIT,          0x00000080 @Recordar que con '1' deshabilito las interrupcciones por IRQ
.EQU F_BIT,          0x00000060  @Recordar que con '1' deshabilito las interrupcciones por FIQ

@Etiquetas tabla de interrupcciones
.EXTERN _reset_vector
.EXTERN _undef_handler
.EXTERN _swi_handler
.EXTERN _pabt_handler
.EXTERN _dabt_handler
.EXTERN _reserved_handler
.EXTERN _irq_handler
.EXTERN _fiq_handler

.EXTERN _kernel_start

@Definicion globales de etiquetas
.GLOBAL _start

@~~~~~~~~Nueva seccion~~~~~~~~
.SECTION ._table_section_start
_table_interrupt_start:
    LDR PC, address_reset_vector
    LDR PC, address_undef_vector
    LDR PC, address_swi_vector
    LDR PC, address_pabt_vector
    LDR PC, address_dabt_vector
    LDR PC, address_reserved_vector
    LDR PC, address_irq_vector
    LDR PC, address_fiq_vector

address_reset_vector:       
    .word _reset_vector
address_undef_vector:
    .word _undef_handler
address_swi_vector:
    .word _swi_handler
address_pabt_vector:
    .word _pabt_handler
address_dabt_vector:
    .word _dabt_handler
address_reserved_vector:
    .word _reserved_handler
address_irq_vector:
    .word _irq_handler
address_fiq_vector:
    .word _fiq_handler

@~~~~~~~~Nueva seccion~~~~~~~~
.SECTION .bss
 .EXTERN INV:       .asciz "INV"
 .EXTERN MEM:       .asciz "MEM"
 .EXTERN SVC:       .asciz "SVC"
 .EXTERN C_LLAM:    .word 0

 @~~~~~~~~Nueva seccion~~~~~~~
.SECTION ._start_code
_start:
    LDR SP,=_PUBLIC_STACK_INIT
    @Copio memcopy de rom a ram
    LDR R0,=_PUBLIC_RAM_INIT                    @Destino
    LDR R1,=__text_memcopy_rom_init__           @Origen  
    LDR R2,=__text_memcopy_size__               @Longitud
    LDR R10,=__text_memcopy_rom_init__
    MOV LR, PC
    BX  R10
    @Copio el vector de interrupcciones
    LDR R0,=_PUBLIC_INT_INIT                    @Destino
    LDR R1,=__text_table_init__                 @Origen  
    LDR R2,=__table_size__                      @Longitud
    LDR R10,=td3_memcpy
    MOV LR, PC
    BX  R10
    @Copio los handlers de ROM a RAM 
    LDR R0, =__handler_code_ram_init__          @Destino
    LDR R1, =__handler_code_rom_init__          @Origen
    LDR R2, =__handler_code_size__              @Longitud
    LDR R10, =td3_memcpy
    MOV LR, PC
    BX  R10
    @Copio la funciones en C de ROM a RAM
    LDR R0, =__c_functions_ram_init__           @Destino
    LDR R1, =__c_functions_rom_init__           @Origen  
    LDR R2, =__c_functions_size__               @Longitud
    LDR R10,=td3_memcpy
    MOV LR, PC
    BX  R10
    @Copio kernel en C de ROM a RAM
    LDR R0, =__kernel_code_ram_init__           @Destino
    LDR R1, =__kernel_code_rom_init__           @Origen  
    LDR R2, =__kernel_code_size__               @Longitud
    LDR R10,=td3_memcpy
    MOV LR, PC
    BX  R10
    B _stack_init

    /*~~~~~~~~~~~~~~~~INICIALIZACION STACK Y HARDWARE~~~~~~~~~~~~~~~~~*/

    /* Inicializacion de los stack pointers de los diversos modos */

_stack_init:
    /*
        I_bit = 1: disable irq
        F_bit = 1: disable fiq
    */
    
    MSR cpsr_c, #(IRQ_MODE | I_BIT | F_BIT) /* Entro en modo irq deshabilito fiq e irq */
    LDR SP,=__irq_stack_top__ /* IRQ stack pointer */

    MSR cpsr_c, #(FIQ_MODE | I_BIT | F_BIT)
    LDR SP,=__fiq_stack_top__ /* FIQ stack pointer */

    MSR cpsr_c, #(SVC_MODE | I_BIT | F_BIT)
    LDR SP,=__svc_stack_top__ /* SVC stack pointer */

    MSR cpsr_c, #(ABT_MODE | I_BIT | F_BIT)
    LDR SP,=__abt_stack_top__ /* ABT stack pointer */
    
    MSR cpsr_c, #(UND_MODE | I_BIT | F_BIT)
    LDR SP,=__und_stack_top__ /* UND stack pointer */

    MSR cpsr_c, #(SYS_MODE | I_BIT | F_BIT)
    LDR SP,=__sys_stack_top__ /* SYS stack pointer */

    /* Inicializacion hardware */
    B _hwd_init

_hwd_init:  
    @Llamo a gic_init
    LDR R10,=__timer_init
    MOV LR, PC
    BX  R10
    @Llamo a timer_init
    LDR R10,=__gic_init
    MOV LR, PC
    BX  R10
    
    B _kernel_init

_kernel_init:
    @Brancheo al kernel en RAM
    B _kernel_start
.END

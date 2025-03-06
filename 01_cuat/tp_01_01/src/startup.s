.code 32

/* Referencias a variables o simbolos externos */
.extern _PUBLIC_STACK_INIT
.extern _PUBLIC_RAM_INIT
.extern _PUBLIC_ROM_INIT
.extern __kernel_init__
.extern _kernelSize_
.extern td3_memcpy

.global _start

.section ._start_code

_start:
    LDR SP, =_PUBLIC_STACK_INIT

    LDR R0, =_PUBLIC_RAM_INIT    @Destino
    LDR R1, =__kernel_init__     @Origen  
    LDR R2, =_kernelSize_        @Longitud 

    LDR R10, =td3_memcpy
    MOV LR, PC
    BX R10

    B   _PUBLIC_RAM_INIT    
.end

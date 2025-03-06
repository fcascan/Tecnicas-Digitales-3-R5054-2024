.code 32

.global _undef_handler
.global _swi_handler
.global _pabt_handler
.global _dabt_handler
.global _reserved_handler
.global _irq_handler
.global _fiq_handler

.EQU EN_IRQ, 0b00010011     @Habilito FIQ & IRQ

.section ._handler_code, "a"

_undef_handler:
    B .

_swi_handler:
    stmfd sp!, { r0-r12, lr }   @Guardo los registros de r0 a r12 en la pila, tambien el link register (lr) y luego decremento el stack pointer
                                @Almaceno el contexto. storage multiple
    mov   r7, SP                     
    mrs   r8, spsr              @Muevo el contenido de spsr al registro r8
    push {r7, r8}               @Pongo en la pila SP y spsr a traves de r7 y r8

    mov   r0, sp
    bl    kernel_handler_swi    @identificacion de la irq
    pop {r7, r8}                @Recupero el spsr y el estack pointer
    mov sp, r7
    ldmfd sp!, { r0-r12, pc }^  @La instruccion ldmfd (Load Multiple Decrement Before). Cargo ro a r12 y el pc sacados del stack.
                                @Decrementandose en el numero de registros cargados
                                @ ! hace que el stack pointer sea actualizado una vez ejecutada la instruccion. 
                                @ ^ hace que el SPSR sea copiado en el CPSR
_pabt_handler:
    B .

_dabt_handler:
    B .

_reserved_handler:
    B .

_irq_handler:
    sub   lr, lr, #4            @El pipeline estaba aputando dos instrucciones siguientes que cuando se produjo la interrupcion
                                @Como el link register guarda la direccion donde hay que volver, retorna a la direccion siguiente 
                                @a ejercutarse que cuando se produjo la interrupccion
    stmfd sp!, { r0-r12, lr }   @Guardo los registros de r0 a r12 en la pila, tambien el link register (lr) y luego decremento el stack pointer
                                @Almaceno el contexto. storage multiple
    mov   r7, SP                     
    mrs   r8, spsr              @Muevo el contenido de spsr al registro r8
    push {r7, r8}               @Pongo en la pila SP y spsr a traves de r7 y r8

    mov   r0, sp
    bl    kernel_handler_irq    @identificacion de la irq
    pop {r7, r8}                @Recupero el spsr y el estack pointer
    mov sp, r7
    ldmfd sp!, { r0-r12, pc }^  @La instruccion ldmfd (Load Multiple Decrement Before). Cargo ro a r12 y el pc sacados del stack.
                                @Decrementandose en el numero de registros cargados
                                @ ! hace que el stack pointer sea actualizado una vez ejecutada la instruccion. 
                                @ ^ hace que el SPSR sea copiado en el CPSR

_fiq_handler:
    B .
.end

/*
    ldmfd se utiliza para cargar múltiples registros  
    stmfd se utiliza para almacenar múltiples registros 
*/


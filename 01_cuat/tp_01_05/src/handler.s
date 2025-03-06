.code 32

.global _undef_handler
.global _swi_handler
.global _pabt_handler
.global _dabt_handler
.global _reserved_handler
.global _irq_handler
.global _fiq_handler

@ Etiquetas
.equ IRQ_MODE,        0x00000012
.equ FIQ_MODE,        0x00000011
.equ SVC_MODE,        0x00000013
.equ ABT_MODE,        0x00000017
.equ UND_MODE,        0x0000001b
.equ SYS_MODE,        0x0000001f

.equ I_BIT,           0x00000080    @Recordar que con '1' deshabilito las interrupcciones por IRQ
.equ F_BIT,           0x00000060    @Recordar que con '1' deshabilito las interrupcciones por FIQ


@ Tareas
.extern current_task

@ Variables de c
.extern dirty


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
    /*
        Aca guardo el stack de IRQ
    */

    sub   lr, lr, #4            @ El pipeline estaba aputando dos instrucciones siguientes que cuando se produjo la interrupcion
                                @ Como el link register guarda la direccion donde hay que volver, retorna a la direccion siguiente 
                                @ a ejercutarse que cuando se produjo la interrupccion
    stmfd sp!, { r0-r12, lr }   @ Guardo los registros de r0 a r12 en la pila, tambien el link register (lr) y luego decremento el stack pointer
                                @ Almaceno el contexto. storage multiple
                                
    //  Si dirty esta en 0 quiere decir que es la primera vez que se interrumpe el sistema desde que arranco entonces
    //  tengo que directamente cargar el stack incializado en el startup
    ldr r1,=dirty
    ldr r2, [r1]
    mov r3, #0

    cmp r2, r3
    //Si es 0 salta a 
    beq _1st_time
    //Si no es la primera vez no salta!
    //Salva los stacks de la tarea actual de los distintos modos
    
    /*
        En R0 cargo la direccion base de la tarea actual
        en la que estoy
    */

    LDR R0, =current_task
    ldr r0, [r0]

    /*
        Guardo stack de FIQ
    */

    MSR cpsr_c, #(FIQ_MODE | I_BIT | F_BIT)

    MOV R1, SP
    STR R1, [R0, #4]

    /*
        Guardo stack de SVC
    */

    MSR cpsr_c, #(SVC_MODE | I_BIT | F_BIT)

    MOV R1, SP
    STR R1, [R0, #8]

    /*
        Guardo stack de ABT
    */

    MSR cpsr_c, #(ABT_MODE | I_BIT | F_BIT)
    
    MOV R1, SP
    STR R1, [R0, #12]

    /*
        Guardo stack de UND
    */

    MSR cpsr_c, #(UND_MODE | I_BIT | F_BIT)

    MOV R1, SP
    STR R1, [R0, #16]

    /*
        Guardo stack de SYS
    */

    MSR cpsr_c, #(SYS_MODE | I_BIT | F_BIT)

    MOV R1, SP
    STR R1, [R0, #20]

    //Vuelvo a modo IRQ una vez que salve todos los stacks

    MSR cpsr_c, #(IRQ_MODE | I_BIT | F_BIT)
    
_1st_time:
    bl    kernel_handler_irq    @ Identificacion de la irq
    
    pop {r7, r8}                @ Recupero el spsr y el estack pointer
    mov sp, r7
    ldmfd sp!, { r0-r12, pc }^  @ La instruccion ldmfd (Load Multiple Decrement Before). Cargo ro a r12 y el pc sacados del stack.
                                @ Decrementandose en el numero de registros cargados
                                @ ! hace que el stack pointer sea actualizado una vez ejecutada la instruccion. 
                                @ ^ hace que el SPSR sea copiado en el CPSR

_fiq_handler:
    B .
.end

/*
    ldmfd se utiliza para cargar múltiples registros  
    stmfd se utiliza para almacenar múltiples registros 
*/


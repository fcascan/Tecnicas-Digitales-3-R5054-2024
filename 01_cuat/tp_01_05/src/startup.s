.CODE 32
@Constantes 
.EQU IRQ_MODE,          0x00000012
.EQU FIQ_MODE,          0x00000011
.EQU SVC_MODE,          0x00000013
.EQU ABT_MODE,          0x00000017
.EQU UND_MODE,          0x0000001b
.EQU SYS_MODE,          0x0000001f
.EQU I_BIT,             0x00000080 @Recordar que con '1' deshabilito las interrupcciones por IRQ
.EQU F_BIT,             0x00000060 @Recordar que con '1' deshabilito las interrupcciones por FIQ
.equ DIR_NO_PAGINADA,   0x90000000

@Simbolos del linker
/*Kernel*/
.EXTERN _PUBLIC_RAM_INIT
.EXTERN _PUBLIC_INT_INIT

.EXTERN __kernel_begin__
.EXTERN __kernel_size__

.EXTERN __text_table_init__
.EXTERN __table_size__

.EXTERN __irq_stack_top__
.EXTERN __fiq_stack_top__
.EXTERN __svc_stack_top__
.EXTERN __abt_stack_top__
.EXTERN __und_stack_top__
.EXTERN __sys_stack_top__

.EXTERN _OFFSET_DIR_VIRTUAL_FISICA

/*Tarea 1*/
.EXTERN __irq_stack_top__t1__
.EXTERN __fiq_stack_top__t1__vir__
.EXTERN __svc_stack_top__t1__vir__
.EXTERN __abt_stack_top__t1__vir__
.EXTERN __und_stack_top__t1__vir__
.EXTERN __sys_stack_top__t1__vir__
.EXTERN __sys_stack_top__t1__vir__
.EXTERN __T1_textArea_begin__
.EXTERN __T1_textArea_size__
.EXTERN _T1_TEXT_INIT

/*Tarea 2*/
.EXTERN __irq_stack_top__t2__
.EXTERN __fiq_stack_top__t2__vir__
.EXTERN __svc_stack_top__t2__vir__
.EXTERN __abt_stack_top__t2__vir__
.EXTERN __und_stack_top__t2__vir__
.EXTERN __sys_stack_top__t2__vir__
.EXTERN __sys_stack_top__t2__vir__
.EXTERN __T2_textArea_begin__
.EXTERN __T2_textArea_size__
.EXTERN _T2_TEXT_INIT
.EXTERN _T1_TEXT_INIT_VIR
.EXTERN _T2_TEXT_INIT_VIR

/*Tarea 3*/
.EXTERN __T3_textArea_begin__
.EXTERN __T3_textArea_size__
.EXTERN _T3_TEXT_INIT

@Funciones en C
.EXTERN td3_memcpy
.EXTERN board_init

@Fucniones en C de las tareas
.EXTERN task1_f
.EXTERN task2_f
.EXTERN task_idle_f

@Variables globales en C
.EXTERN t_1_est
.EXTERN t_2_est
.EXTERN t_idle_est

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
@Tabla de interrupccion
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
    
   
_tarea_1_stack_init:
    
    /*~~~Inicializacion STACKS en los distintos modos */
    LDR R0, =t_1_est //Cargo la direccion base de la estructura

    MSR cpsr_c, #(IRQ_MODE | I_BIT | F_BIT) /* Entro en modo irq deshabilito fiq e irq */
    LDR SP,=__irq_stack_top__t1__ /* IRQ stack pointer */

    MOV R1, SP  /*Guardo la La direccion stack irq en la estructura */
    STR R1, [R0, #0]

    MSR cpsr_c, #(FIQ_MODE | I_BIT | F_BIT)
    LDR SP,=__fiq_stack_top__t1__vir__ /* FIQ stack pointer */

    MOV R1, SP
    STR R1, [R0, #4]

    MSR cpsr_c, #(SVC_MODE | I_BIT | F_BIT)
    LDR SP,=__svc_stack_top__t1__vir__ /* SVC stack pointer */

    MOV R1, SP
    STR R1, [R0, #8]

    MSR cpsr_c, #(ABT_MODE | I_BIT | F_BIT)
    LDR SP,=__abt_stack_top__t1__vir__ /* ABT stack pointer */
    
    MOV R1, SP
    STR R1, [R0, #12]

    MSR cpsr_c, #(UND_MODE | I_BIT | F_BIT)
    LDR SP,=__und_stack_top__t1__vir__ /* UND stack pointer */

    MOV R1, SP
    STR R1, [R0, #16]

    MSR cpsr_c, #(SYS_MODE | I_BIT | F_BIT)
    LDR SP,=__sys_stack_top__t1__vir__ /* SYS stack pointer */

    MOV R1, SP
    STR R1, [R0, #20]

    /*Tengo que simular el stack en modo IRQ*/
    MSR cpsr_c, #(IRQ_MODE | I_BIT | F_BIT) /* Entro en modo irq deshabilito fiq e irq */
    
    MOV R0,  #0
    MOV R1,  #0
    MOV R2,  #0
    MOV R3,  #0
    MOV R4,  #0
    MOV R5,  #0
    MOV R6,  #0
    MOV R7,  #0
    MOV R8,  #0
    MOV R9,  #0
    MOV R10, #0
    MOV R11, #0
    MOV R12, #0
    LDR LR,=_T1_TEXT_INIT_VIR
    /*LR apunta al comienzo de la seccion text de T1 -> ojo que es la dir virtual que luego la mmu va a traducir en la dir fisica donde esta el codigo realmente */
    STMFD SP!, { R0-R12, LR } //Pusheo todo al stack ficticio y decremento el SP
    //Guardo el stack pointer actualizado en la estructura de t1. Este es el stack en mdoo irq ya que cuando salte la interrupccion por timer sera en modo irq
    LDR R0, =t_1_est
    MOV R1, SP
    //Una vez que cargue todos los stacks en al memoria fisica hago que en al estructura quede referenciado el stack de la direccion lineal
    //que la mmu despues se encarga de traducir a memoria fisica
    //NOTA: EL RESTO DE LOS STACKS COMO NO HACE FALTA INICIALIZARLOS SON CARGADOS EN LA ESTRUCTURA CON LA DIRECCION LINEAL!!!
    LDR R2, =_OFFSET_DIR_VIRTUAL_FISICA
    SUB R1, R1,R2
    STR R1, [R0, #0]
    
    B _tarea_2_stack_init

    /*Tarea 2*/
_tarea_2_stack_init:  
    // La inicializacion de t2 es identica a la de t1
    /*~~~Inicializacion STACKS en los distintos modos */
    LDR R0, =t_2_est //Cargo la direccion base de la estructura

    MSR cpsr_c, #(IRQ_MODE | I_BIT | F_BIT) /* Entro en modo irq deshabilito fiq e irq */
    LDR SP,=__irq_stack_top__t2__ /* IRQ stack pointer */

    MOV R1, SP  /*Guardo la La direccion stack irq en la estructura */
    STR R1, [R0, #0]

    MSR cpsr_c, #(FIQ_MODE | I_BIT | F_BIT)
    LDR SP,=__fiq_stack_top__t2__vir__ /* FIQ stack pointer */
    @Guardo el valor del stack del modo fiq en la primer variable y asi con todas las tareas en todos los modos
    MOV R1, SP
    STR R1, [R0, #4]

    MSR cpsr_c, #(SVC_MODE | I_BIT | F_BIT)
    LDR SP,=__svc_stack_top__t2__vir__ /* SVC stack pointer */

    MOV R1, SP
    STR R1, [R0, #8]

    MSR cpsr_c, #(ABT_MODE | I_BIT | F_BIT)
    LDR SP,=__abt_stack_top__t2__vir__ /* ABT stack pointer */
    
    MOV R1, SP
    STR R1, [R0, #12]

    MSR cpsr_c, #(UND_MODE | I_BIT | F_BIT)
    LDR SP,=__und_stack_top__t2__vir__ /* UND stack pointer */

    MOV R1, SP
    STR R1, [R0, #16]

    MSR cpsr_c, #(SYS_MODE | I_BIT | F_BIT)
    LDR SP,=__sys_stack_top__t2__vir__ /* SYS stack pointer */

    MOV R1, SP
    STR R1, [R0, #20]

    /*Tengo que simular el stack en modo IRQ*/
    MSR cpsr_c, #(IRQ_MODE | I_BIT | F_BIT) /* Entro en modo irq deshabilito fiq e irq */
    
    MOV R0,  #0
    MOV R1,  #0
    MOV R2,  #0
    MOV R3,  #0
    MOV R4,  #0
    MOV R5,  #0
    MOV R6,  #0
    MOV R7,  #0
    MOV R8,  #0
    MOV R9,  #0
    MOV R10, #0
    MOV R11, #0
    MOV R12, #0
    LDR LR,=_T2_TEXT_INIT_VIR
    
    STMFD SP!, { R0-R12, LR } //Pusheo todo al stack ficticio y decremento el SP
    
    LDR R0, =t_2_est
    MOV R1, SP
    //Aca lo que hago es una vez que cargue todos los stacks en al memoria fisica hago que en al estructura quede referenciado el stack de la direccion lineal
    //que la mmu despuies se encarga de traducir a fisica
    //NOTA: EL RESTO DE LOS STACKS COMO NO HACE FALTA INICIALIZARLOS SON CARGADOS EN LA ESTRUCTURA CON LA DIRECCION LINEAL!!!
    LDR R2, =_OFFSET_DIR_VIRTUAL_FISICA
    SUB R1, R1,R2
    STR R1, [R0, #0]        //Guardo la nueva direccion del SP en  modo irq

_task3_stack_init:   

    /*~~~Inicializacion STACKS en los distintos modos */

    LDR R0, =t_idle_est

    MSR cpsr_c, #(IRQ_MODE | I_BIT | F_BIT) /* Entro en modo irq deshabilito fiq e irq */
    LDR SP,=__irq_stack_top__t3__ /* IRQ stack pointer */

    MOV R1, SP
    STR R1, [R0, #0]

    MSR cpsr_c, #(FIQ_MODE | I_BIT | F_BIT)
    LDR SP,=__fiq_stack_top__t3__ /* FIQ stack pointer */

    MOV R1, SP
    STR R1, [R0, #4]

    MSR cpsr_c, #(SVC_MODE | I_BIT | F_BIT)
    LDR SP,=__svc_stack_top__t3__ /* SVC stack pointer */

    MOV R1, SP
    STR R1, [R0, #8]

    MSR cpsr_c, #(ABT_MODE | I_BIT | F_BIT)
    LDR SP,=__abt_stack_top__t3__ /* ABT stack pointer */
    
    MOV R1, SP
    STR R1, [R0, #12]

    MSR cpsr_c, #(UND_MODE | I_BIT | F_BIT)
    LDR SP,=__und_stack_top__t3__ /* UND stack pointer */

    MOV R1, SP
    STR R1, [R0, #16]

    MSR cpsr_c, #(SYS_MODE | I_BIT | F_BIT)
    LDR SP,=__sys_stack_top__t2__ /* SYS stack pointer */

    MOV R1, SP
    STR R1, [R0, #20]

    /*Tengo que simular el stack en modo IRQ*/
    MSR cpsr_c, #(IRQ_MODE | I_BIT | F_BIT) /* Entro en modo irq deshabilito fiq e irq */
    
    MOV R0,  #0
    MOV R1,  #0
    MOV R2,  #0
    MOV R3,  #0
    MOV R4,  #0
    MOV R5,  #0
    MOV R6,  #0
    MOV R7,  #0
    MOV R8,  #0
    MOV R9,  #0
    MOV R10, #0
    MOV R11, #0
    MOV R12, #0
    LDR LR,= task_idle_f
    STMFD SP!, { R0-R12, LR }
    //Aca como no decia nada el enunciado que no sea identity mapping la direccion de memoria
    //Apunta el llink register al comienzo de la tarea idle
    LDR R0, =t_idle_est
    MOV R1, SP
    STR R1, [R0, #0]

    //Hago estos branch de 1 lineas para poder ver todo mas facil en el ddd
    B _stack_init
_stack_init:
   /*Inicializo los stacks de kernel */
    /*Kernel*/
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
   
    MSR cpsr_c, #(SVC_MODE | I_BIT | F_BIT)

    /* Inicializacion stack pointers para los distintos modos */

    /*
        I_bit = 1: disable irq
        F_bit = 1: disable fiq
    */
    /* Copio toda la seccion vector de interrupciones */
tablass_:
    LDR R0,=_PUBLIC_INT_INIT                @Destino
    LDR R1,=__text_table_init__             @Origen  
    LDR R2,=__table_size__                  @Longitud
    LDR R10,=td3_memcpy
    MOV LR, PC
    BX  R10
rams_:  
    /// Copio toda la seccion text de rom a ram 
    LDR R0,=_PUBLIC_RAM_INIT                 @Destino
    LDR R1,=__kernel_begin__                 @Origen  
    LDR R2,=__kernel_size__                  @Longitud
    LDR R10,=td3_memcpy
    MOV LR, PC
    BX  R10
tareas_1_:
    ///Copio tarea1 de rom a su seccion definida en el linker
    LDR R0,=_T1_TEXT_INIT                     @Destino
    LDR R1,=__T1_textArea_begin__             @Origen  
    LDR R2,=__T1_textArea_size__              @Longitud
    LDR R10,=td3_memcpy
    MOV LR, PC
    BX  R10
tareas_2_:
    ///Copio tarea2 de rom a su seccion definida en el linker
    LDR R0,=_T2_TEXT_INIT                     @Destino
    LDR R1,=__T2_textArea_begin__             @Origen  
    LDR R2,=__T2_textArea_size__              @Longitud
    LDR R10,=td3_memcpy
    MOV LR, PC
    BX  R10
tareas_3_:   
    ///Copio tarea idle de rom a su seccion definida en el linker
    LDR R0,=_T3_TEXT_INIT                     @Destino
    LDR R1,=__T3_textArea_begin__             @Origen  
    LDR R2,=__T3_textArea_size__              @Longitud
    LDR R10,=td3_memcpy
    MOV LR, PC
    BX  R10

    /* Inicializacion BOARD - llamamos a board_init en c*/
    LDR R10,=board_init
    MOV LR, PC
    BX  R10
    
    //Activacion de FIQ y IRQ
    MRS R0, CPSR          @ Copia el valor actual del registro CPSR en R0
    MOV R1, #0xC0         @ Creo Máscara con los bits 6 y 7 en 1 (11000000 en binario)
    BIC R0, R0, R1        @ Aplica la operación AND bit a bit con la máscara
    MSR CPSR_c, R0        @ Escribe en el campo de control del registro CPSR el valor modificado

_board_init:
    B _kernel_start 
.END

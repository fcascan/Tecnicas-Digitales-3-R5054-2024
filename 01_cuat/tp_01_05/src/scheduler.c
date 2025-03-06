#include "../inc/scheduler.h"
#include "../inc/common.h"

/*Variables usadas por scheduler*/
Task	*	 current_task;
Task	*	 next_task;
Task t_1_est,t_2_est,t_idle_est;
uint32_t modos[NUM_MODOS] __attribute__((section("._global_scheduler"))) = {
    IRQ_MODE| I_BIT | F_BIT,
    FIQ_MODE| I_BIT | F_BIT,
    SVC_MODE| I_BIT | F_BIT, 
    ABT_MODE| I_BIT | F_BIT,
    UND_MODE| I_BIT | F_BIT,
    SYS_MODE| I_BIT | F_BIT,
};

extern uint8_t ticks;

int C_LLAM2;

uint32_t link_reg;

extern const void _OFFSET_DIR_VIRTUAL_FISICA;


/*
Inicialización de Tareas
*/
__attribute__((section("._scheduler")))void tasks_init(){
    /*Asigno Time slice*/
    t_1_est.time_slice      = 1;
    t_2_est.time_slice      = 1;
    t_idle_est.time_slice   = 8;

    t_1_est.remaining_time_slice      = 1;
    t_2_est.remaining_time_slice      = 1;
    t_idle_est.remaining_time_slice   = 8;

    /*Asigno TTBR0*/
    t_1_est.current_ttbr0       =(uint32_t)&_SYSTABLES_PHYSICAL_T1;
    t_2_est.current_ttbr0       =(uint32_t)&_SYSTABLES_PHYSICAL_T2;
    t_idle_est.current_ttbr0    =(uint32_t)&_SYSTABLES_PHYSICAL_T3;

    /*Asigno estados*/
    t_1_est.state      = READY;
    t_2_est.state      = READY;
    t_idle_est.state   = READY;   
    
    t_1_est.ident       = 1;
    t_2_est.ident       = 2;
    t_idle_est.ident    = 3; 
    
    current_task        = &t_1_est;
    next_task           = &t_2_est; 
}

/*
Cambia la tarea actual cuando el time_slice expira. 
Actualiza el estado de las tareas y realiza el cambio de contexto, alternando entre las tareas en función de la tarea actual.
*/
__attribute__((section("._scheduler")))void scheduler(void) {
    if(current_task->time_slice == ticks){
        ticks = 0;

        //Cambio de tarea
        current_task->state = READY;
        next_task->state = RUNING;

        //dependiendo de que tarea estoy ejecutando hago que current y next task apuntan a las distitnas tareas
        switch (current_task->ident)
        {
            case 1:     //Cambio de Tarea1 a Tarea2
                current_task = &t_2_est;
                next_task    = &t_idle_est;
                break;
            case 2:     //Cambio de Tarea2 a TareaIdle
                current_task = &t_idle_est;
                next_task    = &t_1_est;
                break;
            case 3:     //Cambio de TareaIdle a Tarea1
                current_task = &t_1_est;
                next_task    = &t_2_est;
                break;
            default:
                break;
        }
   }
}


/*
@deprecated No estaria funcionando. Guarda las direcciones de las pilas para diferentes modos de excepción (IRQ, FIQ, SVC, ABT, UND, SYS) y el registro de enlace (LR) para preservar el contexto antes del cambio de tarea.
*/
__attribute__((section("._scheduler")))void salvar_contexto(void){
   
    //Guardo el LR porque lo pierdo al cambiar de modo
    asm ("STR LR,%0" : "=m"(link_reg));
    
    /************IRQ***********/

    /*Cargo Stack en MODO IRQ*/
    asm ("LDR R0,%0" : "=m"(modos[0]));
    asm ("MSR cpsr_c,R0");
    /*Salvo el stack IRQ en la estructura*/
    asm("mov   r0, sp"); // Muevo el SP a R0
    asm("STR R0,%0" : "=m"(current_task->stack_pointer_irq));

    /************FIQ***********/

    //*Cargo Stack en MODO FIQ*/
    asm ("LDR R0,%0" : "=m"(modos[1]));
    asm ("MSR cpsr_c,R0");

    //*Salvo el stack FIQ en la estructura*/
    asm("mov   r0, sp"); // Muevo el SP a R0
    asm("STR R0,%0" : "=m"(current_task->stack_pointer_fiq));
    
    /************SVC***********/

    //*Cargo Stack en MODO SVC*/
    asm ("LDR R0,%0" : "=m"(modos[2]));
    asm ("MSR cpsr_c,R0");

    //*Salvo el stack SVC en la estructura*/
    asm("mov   r0, sp"); // Muevo el SP a R0
    asm("STR R0,%0" : "=m"(current_task->stack_pointer_svc));

    /************ABT***********/

    //*Cargo Stack en MODO ABT*/
    asm ("LDR R0,%0" : "=m"(modos[3]));
    asm ("MSR cpsr_c,R0");

    //*Salvo el stack ABT en la estructura*/
    asm("mov   r0, sp"); // Muevo el SP a R0
    asm("STR R0,%0" : "=m"(current_task->stack_pointer_abt));

    /************UND***********/

    //*Cargo Stack en MODO UND*/
    asm ("LDR R0,%0" : "=m"(modos[4]));
    asm ("MSR cpsr_c,R0");

    //*Salvo el stack UND en la estructura*/
    asm("mov   r0, sp"); // Muevo el SP a R0
    asm("STR R0,%0" : "=m"(current_task->stack_pointer_und));

    /************SYS***********/

    //*Cargo Stack en MODO SYS*/
    asm ("LDR R0,%0" : "=m"(modos[5]));
    asm ("MSR cpsr_c,R0");

    //*Salvo el stack SYS en la estructura*/
    asm("mov   r0, sp"); // Muevo el SP a R0
    asm("STR R0,%0" : "=m"(current_task->stack_pointer_sys));

    //ATENCION: ME GUARDO EL LR PORQUE LO PIERDO!
    asm ("LDR LR,%0" : "=m"(link_reg));
   
}


/*
Restaura las pilas y el registro de enlace para la tarea a la que se va a cambiar. 
Configura el TTBR0 y carga las pilas para los distintos modos de excepción desde la estructura de tarea actual.
*/
__attribute__((section("._scheduler")))void cargar_contexto(void){

    /*Guardo el LR porque se me pierde al cambiar de modo*/
    asm ("STR LR,%0" : "=m"(link_reg));
    
    //Cambio de ttbr0 de la tarea a entrar
    //MMU_Invalidate_TLB();
    MMU_Set_TTBR0_task(current_task->current_ttbr0);

     /************FIQ***********/

    /*Cargo Stack en MODO FIQ*/
    asm ("LDR R0,%0" : "=m"(modos[1]));
    asm ("MSR cpsr_c,R0");
    
    /*Cargo el stack FIQ de la estructura*/
    asm ("LDR R0,%0" : "=m"(current_task->stack_pointer_fiq));
    asm ("MOV SP, R0");

     /************SVC***********/

    /*Cargo Stack en MODO SVC*/
    asm ("LDR R0,%0" : "=m"(modos[2]));
    asm ("MSR cpsr_c,R0");
   
    /*Cargo el stack SVC de la estructura*/
    asm ("LDR R0,%0" : "=m"(current_task->stack_pointer_svc));
    asm ("MOV SP, R0");

     /************ABT***********/

    /*Cargo Stack en MODO ABT*/
    asm ("LDR R0,%0" : "=m"(modos[3]));
    asm ("MSR cpsr_c,R0");
    
    /*Cargo el stack ABT de la estructura*/
    asm ("LDR R0,%0" : "=m"(current_task->stack_pointer_abt));
    asm ("MOV SP, R0");

    /************UND***********/

    /*Cargo Stack en MODO UND*/
    asm ("LDR R0,%0" : "=m"(modos[4]));
    asm ("MSR cpsr_c,R0");
    
    /*Cargo el stack UND de la estructura*/
    asm ("LDR R0,%0" : "=m"(current_task->stack_pointer_und));
    asm ("MOV SP, R0");

    /************SYS***********/

    /*Cargo Stack en MODO SYS*/
    asm ("LDR R0,%0" : "=m"(modos[5]));
    asm ("MSR cpsr_c,R0");
    
    /*Cargo el stack SYS de la estructura*/
    asm ("LDR R0,%0" : "=m"(current_task->stack_pointer_sys));
    asm ("MOV SP, R0");
    
    /************IRQ***********/

    /*Cargo Stack en MODO IRQ*/
    asm ("LDR R0,%0" : "=m"(modos[0]));
    asm ("MSR cpsr_c,R0");
    
    /*Cargo el stack IRQ de la estructura*/
    asm ("LDR R0,%0" : "=m"(current_task->stack_pointer_irq));
    asm ("MOV SP, R0");

    /**************************/

    /*Carga multiple para restaurar desde la pila*/
    asm (" ldmfd sp!, { r0-r12, pc }^");

    /*Guardo el LR porque se me pierde*/
    asm ("LDR LR,%0" : "=m"(link_reg));
}

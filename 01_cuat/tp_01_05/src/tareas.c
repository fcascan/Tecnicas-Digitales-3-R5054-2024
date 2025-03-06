#include "../inc/scheduler.h"

extern const void _T1_READING_AREA_VIR;
extern const void _T2_READING_AREA_VIR;

/*
    T1: : Debe escribir y luego leer en la RAM el word 0x55AA55AA en el rango 0x70A00000 a 0x70A0FFFF
*/
__attribute__((section("._T1")))void task1_f(void){
    uint32_t dato_a_escribir = 0x55AA55AA;
    uint32_t dato_a_leer;
    uint32_t *direccion;
    
    while(1){
        /*Escribo*/
        for(uint32_t i = (uint32_t) & _T1_READING_AREA_VIR; i <= (uint32_t) & _T1_READING_AREA_VIR+0xffc; i+=4){
            direccion = (uint32_t *)i;
            *direccion = dato_a_escribir;
        }
        /*Leo*/
        for(uint32_t i = (uint32_t) & _T1_READING_AREA_VIR; i <= (uint32_t) & _T1_READING_AREA_VIR+0xffc; i+=4){
            direccion = (uint32_t *)i;
            dato_a_leer = *direccion;
        }
        asm ("WFI");
    }
}

/*
    T2: Debe recorrer el rango de 0x70A10000 ~ 0x70A1FFFF y debe hacer un toggle de lo leido (si lee un bit en 1 lo debe pasar a 0, y viceversa)
*/
__attribute__((section("._T2")))void task2_f(void){
    uint32_t *direccion;
    uint32_t dato;
    while(1){
        // /*Leo*/
        for(uint32_t i = (uint32_t) & _T2_READING_AREA_VIR; i <= (uint32_t) & _T2_READING_AREA_VIR+0xffc; i+=4){
            direccion = (uint32_t *)i;
            dato = *direccion;
            dato = !dato;
            *direccion=dato;
        }
        asm  ("WFI");
    }
}

/*
    T3: Kernel en WFI hasta cumplir con el timeFrame
*/
__attribute__((section("._idle")))void task_idle_f(void){
    while(1){
        asm  ("WFI");
    }
}

/*
    //asm ("LDR R0,%0" : "=m"(modos[0])); // C a ASM

    //asm("STR R0,%0" : "=m"(current_task->stack_pointer_irq)); //ASM a C
*/

#include "../inc/mmu_tools.h"
#include "../inc/low_level_cpu_access.h"
#include "../inc/timer.h"
#include "../inc/gic.h"
#include "../inc/scheduler.h"

//Definiciones de direcciones de memoria las traigo del linker script

//TTBR0 del kernel
extern const void _SYSTABLES_PHYSICAL;

//Definiciones memoria del linker

extern const void _PUBLIC_RAM_INIT;
extern const void _PUBLIC_ROM_INIT;
extern const void _PUBLIC_INT_INIT;
extern const void _PUBLIC_STACK_INIT;
extern const void _PUBLIC_DATA_INIT;
extern const void _PUBLIC_BSS_INIT;

extern const void __data_ram_end__;
extern const void __bss_ram_end__;
extern const void __kernel_ram_end__;
extern const void __stack_start__;
extern const void __stack_end__;
extern const void __text_end__;

//Definiciones de tarea 1

extern const void _T1_READING_AREA;
extern const void _T1_READING_AREA_VIR;
extern const void _T1_TEXT_INIT_VIR;
extern const void _T1_DATA_INIT_VIR;
extern const void _T1_STACK_INIT_VIR;
extern const void _T1_BSS_INIT_VIR;
extern const void _T1_RODATA_INIT_VIR;

extern const void __text_t1_init__;
extern const void __text_t1_end__;

extern const void __stack_start__t1__;
extern const void __stack_end__t1__;

//Definiciones de tarea 2

extern const void _T2_READING_AREA;
extern const void _T2_READING_AREA_VIR;
extern const void _T2_TEXT_INIT_VIR;
extern const void _T2_DATA_INIT_VIR;
extern const void _T2_STACK_INIT_VIR;
extern const void _T2_BSS_INIT_VIR;
extern const void _T2_RODATA_INIT_VIR;

extern const void __text_t2_init__;
extern const void __text_t2_end__;
extern const void __stack_start__t2__;
extern const void __stack_end__t2__;

//Definiciones de tarea idle

extern const void __text_idle_init__;
extern const void __text_idle_end__;
extern const void __stack_start__idle__;
extern const void __stack_end__idle__;

//Definiciones de las direcciones de los registros de los perifericos

extern const void _REG_GIC_ADDR;
extern const void _REG_GIC_ADDR_END;
extern const void _REG_TIMER_ADDR;
extern const void _REG_TIMER_ADDR_END;

//Declaracion externa de las estructuras de las tareas
extern Task t_1_est,t_2_est,t_idle_est;

__attribute__((section("._text"))) void board_init(void){
    //Van a ver que son funciones para `leer` y `escribir` registros necesarios para la MMU.

    // Variables para configuración de MMU
    TTBCR ttbcr;
    TTBR0 ttbr0;
    SCTLR sctlr;
    DACR dacr;
    uint32_t vbar;
    uint32_t i,j;

    // Esta lectura, limpia las variables
    ttbcr   = MMU_Get_TTBCR();
    sctlr   = MMU_Get_SCTLR();
    ttbr0   = MMU_Get_TTBR0();
    dacr    = MMU_Get_DACR();
    vbar    = MMU_Get_VBAR();


    __gic_init();       // Inicializamos el GIC y las interrupciones deseadas
    
    __timer_init();     // Inicializamos y configuramos el Timer
    
    tasks_init();       // Inicializacion de tareas
    
    //------------------------------------------------------------------------------//
    // Configuracion del mapa de memoria
    //------------------------------------------------------------------------------//

    // Configuramos el DMCR (Domain Manager Control Register) - Por ahora, en Manager domain:
    dacr.dacr = 0xFFFFFFFF;
    MMU_Set_DACR(dacr);

    // Configuramos el TTBCR. Usamos la descripción como "short descriptor" por ende, si bien la estructura
    // dice T0SZ, por compatiblidad con el modo largo, esos mismos bits representan a N en el modo corto. Así
    // lo ponemos en cero para que TTBR0 pueda acceder a toda la memoria direccionable disponible (4 GB)
    ttbcr.T0SZ = 0;
    MMU_Set_TTBCR(ttbcr);

    // Configuramos el SCTLR
    sctlr.TRE = 0;
    sctlr.C = 0;
    sctlr.I = 0;
    sctlr.Z = 0;
    sctlr.AFE = AF_NO;
    MMU_Set_SCTLR(sctlr);

    // Invalidamos la TLB, según documentación
    MMU_Invalidate_TLB();

    /*----------------------------------------------------Mapeo Kernel  ------------------------------------------------------------------------------*/
    /*----------------------------------------------------Kernel es identity mapping -----------------------------------------------------------------*/


    //Se coloca TTBR0 del kernel, que como es el primero, lo tomamos como la primer direccion fisica disponible para la tabla de sistemas
    MMU_Set_FirstLevelTranslationTable_PhysicalAddress((uint32_t)&_SYSTABLES_PHYSICAL );

    //Mapeo tabla de interrupcciones
    for(i = (uint32_t)&_PUBLIC_INT_INIT;i<(uint32_t)&_PUBLIC_INT_INIT+0x1000;i+=0x1000)
      MMU_MapNewPage(i,i,_4K,XN_ALLOWEXECUTION,_NORMAL,NONCACHEABLE,_SBIT,PL1);
    //Mapeo la ROM
    for(i = (uint32_t)&_PUBLIC_ROM_INIT;i<(uint32_t)&__text_end__;i+=0x1000)
        MMU_MapNewPage(i,i,_4K,XN_ALLOWEXECUTION,_NORMAL,NONCACHEABLE,_SBIT,PL1);
    //Mapeo KERNEL que esta en RAM
    for(i = (uint32_t)&_PUBLIC_RAM_INIT;i<(uint32_t)&__kernel_ram_end__+0x1000;i+=0x1000)
       MMU_MapNewPage(i,i,_4K,XN_ALLOWEXECUTION,_NORMAL,NONCACHEABLE,_SBIT,PL1);
    //Mapeo el stack
    for(i = (uint32_t)&_PUBLIC_STACK_INIT;i<(uint32_t)&__stack_end__;i+=0x1000)
        MMU_MapNewPage(i,i,_4K,XN_ALLOWEXECUTION,_NORMAL,NONCACHEABLE,_SBIT,PL1);
    //Mapeo bss
    for(i = (uint32_t) &_PUBLIC_BSS_INIT; i < (uint32_t) & __bss_ram_end__; i += 0x1000)
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    //Mapeo data_init
    for(i = (uint32_t) &_PUBLIC_DATA_INIT; i < (uint32_t) & __data_ram_end__; i += 0x1000)
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    //Mapeo perifericos GIC
    for(i = (uint32_t)&_REG_GIC_ADDR;i<(uint32_t)&_REG_GIC_ADDR_END;i+=0x1000)
        MMU_MapNewPage(i,i,_4K,XN_ALLOWEXECUTION,_NORMAL,NONCACHEABLE,_SBIT,PL1);
    //Mapeo periferico timer
    for(i = (uint32_t)&_REG_TIMER_ADDR;i<(uint32_t)&_REG_TIMER_ADDR_END;i+=0x1000)
        MMU_MapNewPage(i,i,_4K,XN_ALLOWEXECUTION,_NORMAL,NONCACHEABLE,_SBIT,PL1);
    
    /*----------------------------------------------------Mapeo Tarea 1------------------------------------------------------------------------------*/
     // Asigno TTBR0 de la tarea 1
    MMU_Set_FirstLevelTranslationTable_PhysicalAddress(t_1_est.current_ttbr0);
  
    //Mapeo tabla de interrupcciones
    for(i = (uint32_t) &_PUBLIC_INT_INIT; i<(uint32_t) &_PUBLIC_INT_INIT + 0x1000; i += 0x1000)
       MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL,NONCACHEABLE, _SBIT, PL1);
    //Mapeo KERNEL que esta en RAM
    for(i = (uint32_t) &_PUBLIC_RAM_INIT; i < (uint32_t) &__kernel_ram_end__+0x1000; i += 0x1000)
       MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    // Mapeo kernel tarea 1
    for(i = (uint32_t) &__text_t1_init__,j = (uint32_t) &_T1_TEXT_INIT_VIR; i < (uint32_t) &__text_t1_end__+0x1000;  i += 0x1000,j += 0x1000)
        MMU_MapNewPage(i, j, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    // Mapeo stack tarea 1
    for(i = (uint32_t) &__stack_start__t1__,j = (uint32_t) &_T1_STACK_INIT_VIR; i < (uint32_t) &__stack_end__t1__; i += 0x1000,j += 0x1000)
        MMU_MapNewPage(i, j, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    //Mapeo perifericos GIC
    for(i = (uint32_t) &_REG_GIC_ADDR; i < (uint32_t) &_REG_GIC_ADDR_END; i += 0x1000)
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    //Mapeo periferico timer
    for(i = (uint32_t) &_REG_TIMER_ADDR; i < (uint32_t) &_REG_TIMER_ADDR_END; i += 0x1000)
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
     //Mapeo bss para poder las estructuras cuando interrumpe el timer desde las distintas tareas
    for(i = (uint32_t) &_PUBLIC_BSS_INIT; i < (uint32_t) & __bss_ram_end__; i += 0x1000)
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    //Mapeo el area de lectura de t1
    for(i = (uint32_t) &_T1_READING_AREA, j = (uint32_t) &_T1_READING_AREA_VIR; i <= (uint32_t) & _T1_READING_AREA+0xffc; i += 0x1000,j += 0x1000 )
        MMU_MapNewPage(i, j, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    //Invalido la tlb
    MMU_Invalidate_TLB();

    /*----------------------------------------------------Mapeo Tarea 2------------------------------------------------------------------------------*/
    // Asigno TTBR0 de la tarea 2
    MMU_Set_FirstLevelTranslationTable_PhysicalAddress(t_2_est.current_ttbr0);

    //Mapeo tabla de interrupcciones
    for(i = (uint32_t) &_PUBLIC_INT_INIT; i<(uint32_t) &_PUBLIC_INT_INIT + 0x1000; i += 0x1000)
       MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL,NONCACHEABLE, _SBIT, PL1);
    //Mapeo KERNEL que esta en RAM
    for(i = (uint32_t) &_PUBLIC_RAM_INIT; i < (uint32_t) &__kernel_ram_end__+0x1000; i += 0x1000)
       MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    // Mapeo kernel tarea 1
    for(i = (uint32_t) &__text_t2_init__,j = (uint32_t) &_T2_TEXT_INIT_VIR; i < (uint32_t) &__text_t2_end__+0x1000;  i += 0x1000,j += 0x1000)
        MMU_MapNewPage(i, j, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    // Mapeo stack tarea 2
    for(i = (uint32_t) &__stack_start__t2__,j = (uint32_t) &_T2_STACK_INIT_VIR; i < (uint32_t) &__stack_end__t2__; i += 0x1000,j += 0x1000)
        MMU_MapNewPage(i, j, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    // Mapeo perifericos GIC
    for(i = (uint32_t) &_REG_GIC_ADDR; i < (uint32_t) &_REG_GIC_ADDR_END; i += 0x1000)
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    // Mapeo perifericos Timer
    for(i = (uint32_t) &_REG_TIMER_ADDR; i < (uint32_t) &_REG_TIMER_ADDR_END; i += 0x1000)
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    // Mapeo bss para poder las estructuras cuando interrumpe el timer desde las distintas tareas
    for(i = (uint32_t) &_PUBLIC_BSS_INIT; i < (uint32_t) & __bss_ram_end__; i += 0x1000)
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    // Mapeo el area de lectura de t2
    for(i = (uint32_t) &_T2_READING_AREA, j = (uint32_t) &_T2_READING_AREA_VIR; i <= (uint32_t) & _T2_READING_AREA+0xffc; i += 0x1000,j += 0x1000 )
        MMU_MapNewPage(i, j, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);

    MMU_Invalidate_TLB();

    /*-------------Mapeo Tarea 3-------------*/
    // Asigno TTBR0 de la tarea 3
    MMU_Set_FirstLevelTranslationTable_PhysicalAddress(t_idle_est.current_ttbr0); 
    
    //Mapeo tabla de interrupcciones
    for(i = (uint32_t) &_PUBLIC_INT_INIT; i<(uint32_t) &_PUBLIC_INT_INIT + 0x1000; i += 0x1000)
       MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL,NONCACHEABLE, _SBIT, PL1);
    //Mapeo KERNEL que esta en RAM
    for(i = (uint32_t) &_PUBLIC_RAM_INIT; i < (uint32_t) &__kernel_ram_end__+0x1000; i += 0x1000)
       MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    // Mapeo kernel tarea idle
    for(i = (uint32_t) &__text_idle_init__; i < (uint32_t) &__text_idle_end__+0x1000; i += 0x1000)
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    // Mapeo stack tarea idle
    for(i = (uint32_t) &__stack_start__idle__; i < (uint32_t) &__stack_end__idle__; i += 0x1000)
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    // Mapeo perifericos GIC
    for(i = (uint32_t) &_REG_GIC_ADDR; i < (uint32_t) &_REG_GIC_ADDR_END; i += 0x1000)
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    // Mapeo perifericos Timer
    for(i = (uint32_t) &_REG_TIMER_ADDR; i < (uint32_t) &_REG_TIMER_ADDR_END; i += 0x1000)
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    // Mapeo bss para poder las estructuras cuando interrumpe el timer desde las distintas tareas
    for(i = (uint32_t) &_PUBLIC_BSS_INIT; i < (uint32_t) & __bss_ram_end__; i += 0x1000)
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);

    MMU_Invalidate_TLB();

    // Vuelvo a setear el TTBR0 al kernel

    MMU_Set_FirstLevelTranslationTable_PhysicalAddress((uint32_t)&_SYSTABLES_PHYSICAL );
    
    //Activo la mmu
    MMU_Invalidate_TLB();
    asm("MRC p15, 0,R1, c1, c0, 0 ");
    asm(" ORR R1, R1, #0x1 ");
    asm(" MCR p15, 0, R1, c1, c0, 0");
    asm("DSB");
    MMU_Invalidate_TLB();
}

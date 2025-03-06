#include "../inc/mmu_tools.h"
#include "../inc/low_level_cpu_access.h"
#include "../inc/timer.h"
#include "../inc/gic.h"

//Definiciones de direcciones de memoria las traigo del linker script
extern const void _SYSTABLES_VMA;
extern const void _SYSTABLES_PHYSICAL;
extern const void _PUBLIC_RAM_INIT;
extern const void _PUBLIC_ROM_INIT;
extern const void _PUBLIC_INT_INIT;
//Definiciones de las direcciones de los registros de los perifericos
extern const void _REG_GIC_ADDR;
extern const void _REG_GIC_ADDR_END;
extern const void _REG_TIMER_ADDR;
extern const void _REG_TIMER_ADDR_END;
//Simbolos dinamicos del linkeru
extern const void __kernel_size__;
extern const void __kernel_begin__;
extern const void __kernel_end__;

extern const void __text_table_init__;
extern const void __text_table_end__;       

extern const void __stack_start__;
extern const void __stack_end__;

extern const void __text_end__;

extern const void _SYSTABLES_PHYSICAL;
__attribute__((section("._text"))) void board_init(void){
    //Van a ver que son funciones para `leer` y `escribir` registros necesarios para la MMU.

    // Variables para configuración de MMU
    TTBCR ttbcr;
    TTBR0 ttbr0;
    SCTLR sctlr;
    DACR dacr;
    uint32_t vbar;
    uint32_t i;

    // Esta lectura, limpia las variables
    ttbcr   = MMU_Get_TTBCR();
    sctlr   = MMU_Get_SCTLR();
    ttbr0   = MMU_Get_TTBR0();
    dacr    = MMU_Get_DACR();
    vbar    = MMU_Get_VBAR();

    //------------------------------------------------------------------------------//
    // Inicializamos el GIC y las interrupciones deseadas
    //------------------------------------------------------------------------------//
    __gic_init();
    //------------------------------------------------------------------------------//
    // Inicializamos y configuramos el Timer
    //------------------------------------------------------------------------------//
    __timer_init();
    //------------------------------------------------------------------------------//
    // Copiamos nuestro código a las zonas de memoria requeridas
    //------------------------------------------------------------------------------//
    

    //------------------------------------------------------------------------------//
    // Configuramos nuestro mapa de memoria
    //------------------------------------------------------------------------------//
    // Configuramos el Domain Manager Control Register - Por ahora, todo en Manager domain:
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

    //Arranco a armar el mapa de memoria del kernel

    //Se coloca TTBR0 del kernel, que como es el primero, lo tomamos como la primer direccion fisica disponible para la tabla de sistemas
    /*ttbr0.ttbr0 = ((uint32_t)&_SYSTABLES_PHYSICAL & 0xFFFFC000);
    MMU_Set_TTBR0(ttbr0);*/
    MMU_Set_FirstLevelTranslationTable_PhysicalAddress((uint32_t)&_SYSTABLES_PHYSICAL );
    //Mapeo tabla de interrupcciones
    for(i = (uint32_t)&_PUBLIC_INT_INIT;i<(uint32_t)&_PUBLIC_INT_INIT+0x1000;i+=0x1000)
      MMU_MapNewPage(i,i,_4K,XN_ALLOWEXECUTION,_NORMAL,NONCACHEABLE,_SBIT,PL1);
    //Mapeo la ROM
   for(i = (uint32_t)&_PUBLIC_ROM_INIT;i<(uint32_t)&__text_end__;i+=0x1000)
        MMU_MapNewPage(i,i,_4K,XN_ALLOWEXECUTION,_NORMAL,NONCACHEABLE,_SBIT,PL1);
    //Mapeo KERNEL que esta en RAM
    for(i = (uint32_t)&_PUBLIC_RAM_INIT;i<(uint32_t)&__kernel_end__;i+=0x1000)
       MMU_MapNewPage(i,i,_4K,XN_ALLOWEXECUTION,_NORMAL,NONCACHEABLE,_SBIT,PL1);
    //Mapeo el stack
    for(i = (uint32_t)&__stack_start__;i<(uint32_t)0x70024000;i+=0x1000)
        MMU_MapNewPage(i,i,_4K,XN_ALLOWEXECUTION,_NORMAL,NONCACHEABLE,_SBIT,PL1);
    //Mapeo perifericos GIC & TIMER
    for(i = (uint32_t)&_REG_GIC_ADDR;i<(uint32_t)&_REG_GIC_ADDR_END;i+=0x1000)
        MMU_MapNewPage(i,i,_4K,XN_ALLOWEXECUTION,_NORMAL,NONCACHEABLE,_SBIT,PL1);
    for(i = (uint32_t)&_REG_TIMER_ADDR;i<(uint32_t)&_REG_TIMER_ADDR_END;i+=0x1000)
        MMU_MapNewPage(i,i,_4K,XN_ALLOWEXECUTION,_NORMAL,NONCACHEABLE,_SBIT,PL1);
   
   MMU_Invalidate_TLB();
   asm("MRC p15, 0,R1, c1, c0, 0 ");
   asm(" ORR R1, R1, #0x1   ");
   asm(" MCR p15, 0, R1, c1, c0, 0");
   asm("DSB");
   MMU_Invalidate_TLB();
    

}
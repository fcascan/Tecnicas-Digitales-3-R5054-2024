#include "../inc/mmu_tools.h"
#include "../inc/low_level_cpu_access.h"
extern const void _SYSTABLES_PHYSICAL      ;
extern const void _SYSTABLES_PHYSICAL_L2;

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) ID_MMFR0 MMU_Get_ID_MMFR0(void)
{
    ID_MMFR0 mmfr0;

    asm("MRC p15, 0, R0, c0, c1, 4");
    asm("STR R0,%0" : "=m"(mmfr0.mmfr0));

    return mmfr0;
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) ID_MMFR1 MMU_Get_ID_MMFR1(void)
{
    ID_MMFR1 mmfr1;

    asm("MRC p15, 0, R0, c0, c1, 5");
    asm("STR R0,%0" : "=m"(mmfr1.mmfr1));

    return mmfr1;
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) ID_MMFR2 MMU_Get_ID_MMFR2(void)
{
    ID_MMFR2 mmfr2;

    asm("MRC p15, 0, R0, c0, c1, 6");
    asm("STR R0,%0" : "=m"(mmfr2.mmfr2));

    return mmfr2;
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) ID_MMFR3 MMU_Get_ID_MMFR3(void)
{
    ID_MMFR3 mmfr3;

    asm("MRC p15, 0, R0, c0, c1, 7");
    asm("STR R0,%0" : "=m"(mmfr3.mmfr3));

    return mmfr3;
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) TTBCR MMU_Get_TTBCR(void)
{
    TTBCR ttbcr;

    asm("MRC p15, 0, R0, c2, c0, 2");
    asm("STR R0,%0" : "=m"(ttbcr.ttbcr));

    return ttbcr;
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) void MMU_Set_TTBCR(TTBCR ttbcr)
{
    asm("LDR R0,%0" : "=m"(ttbcr.ttbcr));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c2, c0, 2");
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) DACR MMU_Get_DACR(void)
{
    DACR dacr;

    asm("MRC p15, 0, R0, c3, c0, 0");
    asm("STR R0,%0" : "=m"(dacr.dacr));

    return dacr;
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) void MMU_Set_DACR(DACR dacr)
{
    asm("LDR R0,%0" : "=m"(dacr.dacr));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c3, c0, 0");
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) TTBR0 MMU_Get_TTBR0()
{
    TTBR0 ttbr0;

    asm("MRC p15, 0, R0, c2, c0, 0");
    asm("STR R0,%0" : "=m"(ttbr0.ttbr0));

    return ttbr0;
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) void MMU_Set_TTBR0(TTBR0 ttbr0)
{       
    
    asm("LDR R0,%0" : "=m"(ttbr0.ttbr0));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c2, c0, 0");
    
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) void MMU_Set_TTBR0_task(uint32_t ttbr0)
{       
    
    asm("LDR R0,%0" : "=m"(ttbr0));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c2, c0, 0");
    
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) TTBR1 MMU_Get_TTBR1()
{
    TTBR1 ttbr1;

    asm("MRC p15, 0, R0, c2, c0, 1");
    asm("STR R0,%0" : "=m"(ttbr1.ttbr1));

    return ttbr1;
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) void MMU_Set_TTBR1(TTBR1 ttbr1)
{
    asm("LDR R0,%0" : "=m"(ttbr1.ttbr1));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c2, c0, 1");
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) SCTLR MMU_Get_SCTLR()
{
    SCTLR sctlr;

    asm("MRC p15, 0, R0, c1, c0, 0");
    asm("STR R0,%0" : "=m"(sctlr.sctlr));

    return sctlr;
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) void MMU_Set_SCTLR(SCTLR sctlr)
{
    asm("LDR R0,%0" : "=m"(sctlr.sctlr));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c1, c0, 0");
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) uint32_t MMU_Get_VBAR()
{
    uint32_t vbar;

    asm("MRC p15, 0, R0, c12, c0, 0");
    asm("STR R0,%0" : "=m"(vbar));

    return vbar;
}

/**
 * @brief 
 * 
 */
__attribute__((section("._text"))) void MMU_Set_VBAR(uint32_t vbar)
{
    asm("LDR R0,%0" : "=m"(vbar));
    asm("MCR p15, 0, R0, c12, c0, 0");
}

/**
 * @brief Esta función obtiene el valor de la dirección física de la primer tabla de traducción de la MMU
 * 
 */
__attribute__((section("._text"))) uint32_t MMU_Get_FirstLevelTranslationTable_PhysicalAddress(void)
{
    TTBCR ttbcr = MMU_Get_TTBCR();
    TTBR0 ttbr0 = MMU_Get_TTBR0();
    //TTBR1 ttbr1 = MMU_Get_TTBR1();
    uint8_t tt_size = ttbcr.T0SZ;
    uint32_t tt_ph_addr = 0;

    if(tt_size == 0)
    {
        // Solamente se usa TTBR0 para la traducción, ver TRM B3.5.5
        tt_ph_addr = ttbr0.ttbr0 & 0xFFFFC000;
    }

    return tt_ph_addr;
}

/**
 * @brief Esta función setea la dirección física de la primer tabla de traducción de la MMU
 * 
 */
__attribute__((section("._text"))) void MMU_Set_FirstLevelTranslationTable_PhysicalAddress(uint32_t ph_addr)
{
    TTBCR ttbcr = MMU_Get_TTBCR();
    TTBR0 ttbr0 = MMU_Get_TTBR0();
    //TTBR1 ttbr1 = MMU_Get_TTBR1();
    uint8_t tt_size = ttbcr.T0SZ;

    if(tt_size == 0)
    {
        // Solamente se usa TTBR0 para la traducción, ver TRM B3.5.5
        ttbr0.ttbr0 = (ph_addr & 0xFFFFC000);
        MMU_Set_TTBR0(ttbr0);
    }
}

/**
 * @brief Esta función es un wrapper para habilitar o no el uso del access flag en los descriptores
 * 
 */
__attribute__((section("._text"))) void MMU_Enable_AccessFlag(ENABLEAF af)
{
    SCTLR sctlr = MMU_Get_SCTLR();

    sctlr.AFE = af;

    MMU_Set_SCTLR(sctlr);
}

/**
 * @brief Esta función es un wrapper para habilitar o no el uso de la MMU
 * 
 */
__attribute__((section("._text"))) void MMU_Enable(MMUENABLE mmuenable)
{
    SCTLR sctlr = MMU_Get_SCTLR();

    sctlr.M = mmuenable;

    // dbg("\r\n\tEscribiendo SCTLR con 0x%x. Chan chan...",sctlr.sctlr);
    MMU_Set_SCTLR(sctlr);
    // dbg("\r\n\tEureka!");
}

/**
 * @brief Invalida todas las entradas de la TLB siguiendo el TRM B3.18.7
 * 
 */
__attribute__((section("._text"))) void MMU_Invalidate_TLB(void)
{
    asm("MCR p15, 0, R0, c8, c6, 0"); // Invalidate entire DATA TLB
    asm("MCR p15, 0, R0, c8, c5, 0"); // Invalidate entire INSTRUCTION TLB
    asm("MCR p15, 0, R0, c8, c7, 0"); // Invalidate entire UNIFIED TLB
}

/**
 * @brief Esta función crea las paginas
 */
__attribute__((section("._text"))) void MMU_MapNewPage( uint32_t PhysicalAddr, 
                                                        uint32_t virtualAddr,
                                                        PAGESIZE PAGESIZE,
                                                        uint8_t execType,
                                                        MEMORYTYPE MEMORYTYPE,
                                                        DESCRIPTION DESCRIPTION,
                                                        PAGESHAREABLE PAGESHAREABLE,
                                                        AP_PLX AP_PLX)
{
    uint32_t *l2Table,*l1Table;
    uint32_t offsetl2Table,offsetl1Table;
    uint32_t offsetl1Table_extra;
    TTBR0 current_ttbr0;
    //Tomo la direccion base de la tabla de 1er nivel de manera dinamica del ttbr0
    current_ttbr0 = MMU_Get_TTBR0();
//Tabla 1er nivel:
    //Recupero el offset para entrar a la Tabla lvl1 a partir de la dir virtual
    offsetl1Table = 0x04*((virtualAddr>>20)&0xFFF);
    //Armo la direccion
    l1Table =  (uint32_t *)(offsetl1Table + current_ttbr0.ttbr0);
    //Limpio la tabla de primer nivel
    *l1Table = 0; 
    //Agrego una separacion entre las distintas paginas para asegurarme de no pisar las tablas
    offsetl1Table_extra = 0x4000 + offsetl1Table*0x100;
    //La direccion base de tabla de lvl2 va a ser dinamica en funcion de la ttbr0
    *l1Table = current_ttbr0.ttbr0 + offsetl1Table_extra + 0x1;
//Tabla 2do nivel:
    //Rescato el offset de la tabla de nivel 2 a partir de la direccion virtual
    offsetl2Table = 0x04*((virtualAddr>>12)&0xFF);
    //La direccion va a ser dinamica a partir de la direccion, el offset de la tabla de primer nivel y 
    //la direccion virtual para asegurarme de no pisar las tablas
    l2Table = current_ttbr0.ttbr0   +   offsetl1Table_extra  +   offsetl2Table;
    *l2Table = 0; 
    //Asigno la direccion fisica a donde va a apuntar
    *l2Table = (PhysicalAddr)|0x32;//OK!
    return;
}

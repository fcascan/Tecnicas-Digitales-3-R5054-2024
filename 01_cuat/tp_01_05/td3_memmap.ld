/*
Definiciones necesarias: formato de salida, arquitectura y punto de entrada
*/
OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_reset_vector)

/*
Tablas de paginacion
*/
_SYSTABLES_PHYSICAL     =   0x70900000;
_SYSTABLES_PHYSICAL_T1  =   0x800D0000;
_SYSTABLES_PHYSICAL_T2  =   0x80F00000;
_SYSTABLES_PHYSICAL_T3  =   0x800E0000;

/*  
Definiciones de simbolos necesarios
*/
_PUBLIC_DATA_INIT       =   0x70031000;
_PUBLIC_RAM_INIT        =   0x70030000;
_PUBLIC_RODATA_INIT     =   0x70023000;
_PUBLIC_BSS_INIT        =   0x70022000;
_PUBLIC_STACK_INIT      =   0x70020000;
_PUBLIC_ROM_INIT        =   0x70010000;
_PUBLIC_INT_INIT        =   0x00000000;

/*
Definciones de las direcciones base de los registros
 */
_REG_GIC_ADDR           =   0x1E000000;
_REG_GIC_ADDR_END       =   0x1E031000;
_REG_GIC_LENGTH         =   _REG_GIC_ADDR_END - _REG_GIC_ADDR  ;
_REG_TIMER_ADDR         =   0x10011000;
_REG_TIMER_ADDR_END     =   0x10019000;
_REG_TIMER_LENGTH       =   _REG_TIMER_ADDR_END - _REG_TIMER_ADDR;

/*
Definiciones de las longitud de las longitudes de los stacks
 */
C_STACK_SIZE_TAREAS     = 3K;
C_STACK_SIZE            = 4K;
IRQ_STACK_SIZE          = 128;
FIQ_STACK_SIZE          = 128;
SVC_STACK_SIZE          = 128;
ABT_STACK_SIZE          = 128;
UND_STACK_SIZE          = 128;

/*
Definiciones del mapa de memoria Fisicas
*/
_T1_READING_AREA    = 0x80000000;
_T1_TEXT_INIT       = 0x80750000;
_T1_DATA_INIT       = 0x80751000;
_T1_STACK_INIT      = 0x80752000;
_T1_BSS_INIT        = 0x80753000;
_T1_RODATA_INIT     = 0x80754000;

_T2_READING_AREA    = 0x81000000;
_T2_TEXT_INIT       = 0x81750000;
_T2_DATA_INIT       = 0x81751000;
_T2_STACK_INIT      = 0x81752000;
_T2_BSS_INIT        = 0x81753000;
_T2_RODATA_INIT     = 0x81754000;

_T3_READING_AREA    = 0x80A20000;
_T3_TEXT_INIT       = 0x80F30000;
_T3_DATA_INIT       = 0x80F31000;
_T3_STACK_INIT      = 0x80F32000;
_T3_BSS_INIT        = 0x80F33000;
_T3_RODATA_INIT     = 0x80F34000;

/*
Definiciones del mapa de memoria Lineales
*/
_T1_READING_AREA_VIR    = 0x70A00000;
_T1_TEXT_INIT_VIR       = 0x70F50000;
_T1_DATA_INIT_VIR       = 0x70F51000;
_T1_STACK_INIT_VIR      = 0x70F52000;
_T1_BSS_INIT_VIR        = 0x70F53000;
_T1_RODATA_INIT_VIR     = 0x70F54000;

_T2_READING_AREA_VIR    = 0x71F50000;
_T2_TEXT_INIT_VIR       = 0x71F51000;
_T2_DATA_INIT_VIR       = 0x71F51000;
_T2_STACK_INIT_VIR      = 0x71F52000;
_T2_BSS_INIT_VIR        = 0x71F53000;
_T2_RODATA_INIT_VIR     = 0x71F54000;

_OFFSET_DIR_VIRTUAL_FISICA =  _T2_STACK_INIT - _T2_STACK_INIT_VIR;

/*
Definiciones de las longitudes asignadas a las secciones 
*/
MEMORY{
    public_int         : org = _PUBLIC_INT_INIT,    len = 0x0020
    public_stack       : org = _PUBLIC_STACK_INIT,  len = 1M
    public_rom         : org = _PUBLIC_ROM_INIT,    len = 0x3000        /* 12KiB */
    public_ram         : org = _PUBLIC_RAM_INIT,    len = 32M
    public_data        : org = _PUBLIC_DATA_INIT,   len = 0x1000    
    public_rodata      : org = _PUBLIC_RODATA_INIT, len = 0x1000 
    public_bss         : org = _PUBLIC_BSS_INIT,    len = 0x1000 
    
    T2_readingArea     : org = _T2_READING_AREA,    len = 0x1000
    T1_readingArea     : org = _T1_READING_AREA,    len = 0x1000 
    T3_readingArea     : org = _T3_READING_AREA,    len = 0x1000

    T1_textArea        : org = _T1_TEXT_INIT,       len = 0x1000
    T1_data            : org = _T1_DATA_INIT,       len = 0x1000
    T1_Pila            : org = _T1_STACK_INIT,      len = 0x1000
    T1_bss             : org = _T1_BSS_INIT,        len = 0x1000
    T1_rodata          : org = _T1_RODATA_INIT,     len = 0x1000

    T2_textArea        : org = _T2_TEXT_INIT,       len = 0x1000
    T2_data            : org = _T2_DATA_INIT,       len = 0x1000
    T2_Pila            : org = _T2_STACK_INIT,      len = 0x1000
    T2_bss             : org = _T2_BSS_INIT,        len = 0x1000
    T2_rodata          : org = _T2_RODATA_INIT,     len = 0x1000

    T3_textArea        : org = _T3_TEXT_INIT,       len = 0x1000
    T3_data            : org = _T3_DATA_INIT,       len = 0x1000
    T3_Pila            : org = _T3_STACK_INIT,      len = 0x1000
    T3_bss             : org = _T3_BSS_INIT,        len = 0x1000
    T3_rodata          : org = _T3_RODATA_INIT,     len = 0x1000
}

SECTIONS{
    . = _PUBLIC_ROM_INIT; 

    /*************       Seccion text       *************/

    .text :  { 
        *(._reset_code)
        *(._start_code)
        __text_table_init__ = .;
        *(._table_section_start)
        __text_table_end__  = .;
        *(._memcopy)
        *(._text*)
    }>public_rom
    __table_size__      = __text_table_end__    -   __text_table_init__;                      /* Longitud tabla */
    __text_end__        = LOADADDR(.text) + SIZEOF(.text);  /*Calculo la direccion del fin de la seccion text*/

    /*************       Seccion kernel       *************/

    __kernel_begin__ = LOADADDR(.text)   +   SIZEOF(.text);
    /*Esto va a tener un espacio RAM reservado (VMA) pero se va a compilar a continuacion de text ROM (LMA)*/
    .kernel  _PUBLIC_RAM_INIT : AT(__kernel_begin__)   { 
        *(._handler_code*)
        *(._kernel*)
        *(._scheduler)
        *(._global_scheduler)
        __kernel_ram_end__ = .;
    }>public_ram

    __kernel_size__     = SIZEOF(.kernel);
    __kernel_end__      = LOADADDR(.kernel) + SIZEOF(.kernel);
   
   /*************       Seccion data       *************/

    .data  _PUBLIC_DATA_INIT : AT(__kernel_end__)
    {
        *(.data*)
    } > public_data 
    __data_ram_end__ =  SIZEOF(.data) +_PUBLIC_DATA_INIT;
    __data_size__  = SIZEOF(.data);
    __data_end__ = LOADADDR(.data) + SIZEOF(.data);

    /*************       Seccion bss       *************/

    .bss _PUBLIC_BSS_INIT :  AT(__data_end__){
        . = ALIGN(4);
        *(.bss*)
    }> public_bss
    __bss_ram_end__ =  SIZEOF(.bss) +_PUBLIC_BSS_INIT;
    __bss_size__    = SIZEOF(.bss);
    __bss_end__     = LOADADDR(.bss) + SIZEOF(.bss);

    /*************       Seccion stack       *************/

    .stack _PUBLIC_STACK_INIT : AT(__bss_end__)
    {
        . = ALIGN(4);
        __stack_start__ = .;
        . += IRQ_STACK_SIZE;
        . = ALIGN(4);
        __irq_stack_top__ = .;

        . += FIQ_STACK_SIZE;
        . = ALIGN(4);
        __fiq_stack_top__ = .;

        . += SVC_STACK_SIZE;
        . = ALIGN(4);
        __svc_stack_top__ = .;

        . += ABT_STACK_SIZE;
        . = ALIGN(4);
        __abt_stack_top__ = .;

        . += UND_STACK_SIZE;
        . = ALIGN(4);
        __und_stack_top__ = .;

        . += C_STACK_SIZE;
        . = ALIGN(4);
        __sys_stack_top__ = .;

        __stack_end__ = .;
    } > public_stack

    /*************       Secciones T1       *************/

    __PublicStack_size__    = SIZEOF(.stack);
    __PublicStack_end__     = LOADADDR(.stack) + SIZEOF(.stack);
    .T1_readingArea  _T1_READING_AREA : AT(__PublicStack_end__){
        *(.T1_readingArea)
    }>T1_readingArea

    __T1_readingArea_size__    = SIZEOF(.T1_readingArea);
    __T1_readingArea_end__     = LOADADDR(.T1_readingArea) + SIZEOF(.T1_readingArea);
    __T1_textArea_begin__      = __T1_readingArea_end__;

    .T1_textArea     _T1_TEXT_INIT :  AT(__T1_readingArea_end__){
        __text_t1_init__ = .;
        *(._T1)
        __text_t1_end__ = .;
    }> T1_textArea
    
    __T1_textArea_size__    = SIZEOF(.T1_textArea);
    __T1_textArea_end__     = LOADADDR(.T1_textArea) + SIZEOF(.T1_textArea);

    .T1_data         _T1_DATA_INIT :  AT(__T1_textArea_end__){*(._dataT1)}> T1_data
    
    __T1_data_size__    = SIZEOF(.T1_data);
    __T1_data_end__     = LOADADDR(.T1_data) + SIZEOF(.T1_data);
    
    .T1_Pila         _T1_STACK_INIT : AT(__T1_data_end__){
        . = ALIGN(4);
        __stack_start__t1__ = .;
        . += IRQ_STACK_SIZE;
        . = ALIGN(4);
        __irq_stack_top__t1__ = .;
        __irq_stack_top__t1__vir__ = __irq_stack_top__t1__ - _OFFSET_DIR_VIRTUAL_FISICA;
        . += FIQ_STACK_SIZE;
        . = ALIGN(4);
        __fiq_stack_top__t1__ = .;
        __fiq_stack_top__t1__vir__ = __fiq_stack_top__t1__ - _OFFSET_DIR_VIRTUAL_FISICA;
        . += SVC_STACK_SIZE;
        . = ALIGN(4);
        __svc_stack_top__t1__ = .;
        __svc_stack_top__t1__vir__ = __svc_stack_top__t1__ - _OFFSET_DIR_VIRTUAL_FISICA;
        . += ABT_STACK_SIZE;
        . = ALIGN(4);
        __abt_stack_top__t1__ = .;
        __abt_stack_top__t1__vir__ = __abt_stack_top__t1__  - _OFFSET_DIR_VIRTUAL_FISICA;
        . += UND_STACK_SIZE;
        . = ALIGN(4);
        __und_stack_top__t1__ = .;
        __und_stack_top__t1__vir__ = __und_stack_top__t1__  - _OFFSET_DIR_VIRTUAL_FISICA;
        . += C_STACK_SIZE_TAREAS;
        . = ALIGN(4);
        __sys_stack_top__t1__ = .;
        __sys_stack_top__t1__vir__ = __sys_stack_top__t1__  - _OFFSET_DIR_VIRTUAL_FISICA;
        __stack_end__t1__ = .;
    }>T1_Pila

    __T1_pila_size__    = SIZEOF(.T1_Pila);
    __T1_pila_end__     = LOADADDR(.T1_Pila) + SIZEOF(.T1_Pila);

    /*************    Secciones T2       *************/
    
    .T2_readingArea _T2_READING_AREA : AT(__T1_pila_end__){
        
        *(.T2_readingArea);
       
    }>T2_readingArea

    __T2_readingArea_size__    = SIZEOF(.T2_readingArea);
    __T2_readingArea_end__     = LOADADDR(.T2_readingArea) + SIZEOF(.T2_readingArea);
    __T2_textArea_begin__ = __T2_readingArea_end__;

    .T2_textArea   _T2_TEXT_INIT  : AT(__T2_readingArea_end__) {
      __text_t2_init__ = .;  
        *(._T2)
     __text_t2_end__ = .;
    }> T2_textArea
    
    __T2_textArea_size__    = SIZEOF(.T2_textArea);
    __T2_textArea_end__     = LOADADDR(.T2_textArea) + SIZEOF(.T2_textArea);
    
    .T2_data       _T2_DATA_INIT  :AT(__T2_textArea_end__){*(._dataT2)}> T2_data
    
    __T2_data_size__    = SIZEOF(.T2_data);
    __T2_data_end__     = LOADADDR(.T2_data) + SIZEOF(.T2_data);
    
    .T2_Pila       _T2_STACK_INIT : AT(__T2_data_end__){
        . = ALIGN(4);
        __stack_start__t2__ = .;
        . += IRQ_STACK_SIZE;
        . = ALIGN(4);
        __irq_stack_top__t2__ = .;
        __irq_stack_top__t2__vir__ = __irq_stack_top__t2__ - _OFFSET_DIR_VIRTUAL_FISICA;
        . += FIQ_STACK_SIZE;
        . = ALIGN(4);
        __fiq_stack_top__t2__ = .;
        __fiq_stack_top__t2__vir__ = __fiq_stack_top__t2__ - _OFFSET_DIR_VIRTUAL_FISICA;
        . += SVC_STACK_SIZE;
        . = ALIGN(4);
        __svc_stack_top__t2__ = .;
        __svc_stack_top__t2__vir__ = __svc_stack_top__t2__ - _OFFSET_DIR_VIRTUAL_FISICA;
        . += ABT_STACK_SIZE;
        . = ALIGN(4);
        __abt_stack_top__t2__ = .;
        __abt_stack_top__t2__vir__ = __abt_stack_top__t2__  - _OFFSET_DIR_VIRTUAL_FISICA;
        . += UND_STACK_SIZE;
        . = ALIGN(4);
        __und_stack_top__t2__ = .;
        __und_stack_top__t2__vir__ = __und_stack_top__t2__  - _OFFSET_DIR_VIRTUAL_FISICA;
        . += C_STACK_SIZE_TAREAS;
        . = ALIGN(4);
        __sys_stack_top__t2__ = .;
        __sys_stack_top__t2__vir__ = __sys_stack_top__t2__  - _OFFSET_DIR_VIRTUAL_FISICA;
        __stack_end__t2__ = .;
    }>T2_Pila

    __T2_stack_size__    = SIZEOF(.T2_Pila);
    __T2_stack_end__     = LOADADDR(.T2_Pila) + SIZEOF(.T2_Pila);

    /*************    Secciones T3       *************/

    .T3_readingArea  _T3_READING_AREA : AT(__T2_stack_end__){*(.T3_readingArea);}>T3_readingArea

    __T3_readingArea_size__    = SIZEOF(.T3_readingArea);
    __T3_readingArea_end__     = LOADADDR(.T3_readingArea) + SIZEOF(.T3_readingArea);
    __T3_textArea_begin__ = __T3_readingArea_end__;

    .T3_textArea    _T3_TEXT_INIT  : AT(__T3_readingArea_end__){
        __text_idle_init__ = .;
        *(._idle)
        __text_idle_end__ = .;
    }> T3_textArea
    
    __T3_textArea_size__    = SIZEOF(.T3_textArea);
    __T3_textArea_end__     = LOADADDR(.T3_textArea) + SIZEOF(.T3_textArea);

    .T3_data        _T3_DATA_INIT  : AT(__T3_textArea_end__){*(._dataT3)}> T3_data

    __T3_data_size__    = SIZEOF(.T3_data);
    __T3_data_end__     = LOADADDR(.T3_data) + SIZEOF(.T3_data);

    .T3_Pila        _T3_STACK_INIT : AT(__T3_data_end__) {
        . = ALIGN(4);
        __stack_start__idle__ = .;
        . += IRQ_STACK_SIZE;
        . = ALIGN(4);
        __irq_stack_top__t3__ = .;

        . += FIQ_STACK_SIZE;
        . = ALIGN(4);
        __fiq_stack_top__t3__ = .;

        . += SVC_STACK_SIZE;
        . = ALIGN(4);
        __svc_stack_top__t3__ = .;

        . += ABT_STACK_SIZE;
        . = ALIGN(4);
        __abt_stack_top__t3__ = .;

        . += UND_STACK_SIZE;
        . = ALIGN(4);
        __und_stack_top__t3__ = .;

        . += C_STACK_SIZE_TAREAS;
        . = ALIGN(4);
        __sys_stack_top__t3__ = .;
        __stack_end__idle__ = .;
    }>T3_Pila
}

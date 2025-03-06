/*
Definiciones necesarias: formato de salida, arquitectura y punto de entrada
*/
OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_reset_vector)
/*
    Tablas de paginacion
*/
_SYSTABLES_VMA          =   0x70090000;
_SYSTABLES_LMA          =   0x70090000;
_SYSTABLES_PHYSICAL     =   0x70090000;
_SYSTABLES_PHYSICAL_L2  =   0x70300000;
/*
_SYSTABLES_PHYSICAL     =   0x70010000;
_SYSTABLES_PHYSICAL_L2  =   0x70020000;
*/
/*  
    Definiciones de simbolos necesarios
*/
_PUBLIC_INT_INIT        =   0x00000000;
_PUBLIC_ROM_INIT        =   0x70010000;  
_PUBLIC_STACK_INIT      =   0x70020000;
_PUBLIC_RAM_INIT        =   0x70030000;
/*
    Definciones de las direcciones base de los registros
 */
_REG_GIC_ADDR           =   0x1E000000;
_REG_GIC_ADDR_END       =   0x1E031000;
_REG_GIC_LENGTH         =   _REG_GIC_ADDR_END - _REG_GIC_ADDR  ;
_REG_TIMER_ADDR         =   0x10011000;
_REG_TIMER_ADDR_END     =   0x10019000;
_REG_TIMER_LENGTH       =   _REG_TIMER_ADDR_END - _REG_TIMER_ADDR;


C_STACK_SIZE = 4K;
IRQ_STACK_SIZE = 128;
FIQ_STACK_SIZE = 128;
SVC_STACK_SIZE = 128;
ABT_STACK_SIZE = 128;
UND_STACK_SIZE = 128;
/*
Definiciones del mapa de memoria
*/
MEMORY{
    public_int         : org = _PUBLIC_INT_INIT,    len = 0x0020
    public_rom         : org = _PUBLIC_ROM_INIT,    len = 0x1000
    public_ram         : org = _PUBLIC_RAM_INIT,    len = 32M
    public_stack       : org = _PUBLIC_STACK_INIT,  len = 1M
}
SECTIONS{
    . = _PUBLIC_ROM_INIT; 

    /*~~~~~~~~~~    Seccion text     ~~~~~~~~~~*/

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
    __text_end__ = .;
    /*~~~~~~~~~~    Seccion kernel     ~~~~~~~~~~*/

    __kernel_begin__ = LOADADDR(.text)   +   SIZEOF(.text);

    .kernel  _PUBLIC_RAM_INIT : AT(__kernel_begin__)   { 
        *(._handler_code*)
        *(._kernel*)
    }>public_ram

    __kernel_size__     = SIZEOF(.kernel);

    __kernel_end__      = _PUBLIC_RAM_INIT + __kernel_size__;

    /*~~~~~~~~~~    Seccion bss         ~~~~~~~~~~*/

    .bss    :  {
        . = ALIGN(4);
        *(.bss*)
    }> public_ram

    /*~~~~~~~~~~    Seccion data        ~~~~~~~~~~*/
   
    .data  :  {*(.data*)} > public_ram 

    /*~~~~~~~~~~    Seccion stack       ~~~~~~~~~~*/
    __stack_start__ = .;
    .stack :
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
}
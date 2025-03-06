/*
    Definiciones necesarias: formato de salida, arquitectura y punto de entrada
*/
OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_reset_vector)

/*  
    Definiciones de simbolos necesarios
*/
_PUBLIC_INT_INIT    =   0x00000000;
_PUBLIC_ROM_INIT    =   0x70010000;  
_PUBLIC_STACK_INIT  =   0x70020000;
_PUBLIC_RAM_INIT    =   0x70030000;

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

    /************    Seccion text     ************/

    .text : { 
        *(._reset_code)
        __text_table_init__ = .;
        *(._table_section_start)
        __text_table_end__ = .;
        *(._start_code)
        *(.text)
    }>public_rom

    __table_size__        = __text_table_end__    -   __text_table_init__;                      /* Longitud tabla */


    /************    Seccion memcopy     ************/

    __text_memcopy_rom_init__ =  LOADADDR(.text)   +   SIZEOF(.text);                           /* LMA handler_code */

    .text_memcopy _PUBLIC_RAM_INIT : AT(__text_memcopy_rom_init__){
        *(.text_memcopy)
    }>public_ram

    __text_memcopy_size__ = SIZEOF(.text_memcopy);                                              /* Longitud seccion memcopy */
    

    /************    Seccion handler_code  ************/

    __handler_code_rom_init__  =   LOADADDR(.text_memcopy)   +   SIZEOF(.text_memcopy);         /* LMA handler_code */
 
    __handler_code_ram_init__  =   _PUBLIC_RAM_INIT + __text_memcopy_size__;                    /* VMA handler_code */
     
    ._handler_code __handler_code_ram_init__ : AT (__handler_code_rom_init__){
        *(._handler_code*)
    } > public_ram
    
    __handler_code_size__ = SIZEOF(._handler_code);                                             /*Longitud seccion handler_code */


    /************    Seccion c_functions  ************/

    __c_functions_rom_init__   = LOADADDR(._handler_code)   +   SIZEOF(._handler_code);         /* LMA handler_code */

    __c_functions_ram_init__   =  __handler_code_size__ + __handler_code_ram_init__;            /* VMA handler_code */

    /* Quiero que la seccion ._c_functions este en LMA despues de la secccion 
        ._handler_code  pero en VMA despues de la seccion de ._handler_code         */
    ._c_functions  __c_functions_ram_init__ : AT(__c_functions_rom_init__){
        *(.text_timer_init)
        *(.text_gic_init)
        *(.kernel_handlers)
    }   > public_ram

    __c_functions_size__  = SIZEOF(._c_functions);                                              /*Longitud seccion c_functions */
    
    
    /************    Seccion kernel       ************/

    __kernel_code_rom_init__   = LOADADDR(._c_functions)   +   SIZEOF(._c_functions);           /* LMA c_functions*/

    __kernel_code_ram_init__   =  __c_functions_size__ + __c_functions_ram_init__;              /* VMA c_functions */

    ._kernel_code  __kernel_code_ram_init__ : AT(__kernel_code_rom_init__){
        *(._kernel_code)
    }   > public_ram

    __kernel_code_size__  = SIZEOF(._kernel_code);                                              /*Longitud seccion kernel_code */


    /************    Seccion bss         ************/

    .bss    :  {
        . = ALIGN(4);
        *(.bss*)
    }> public_ram


    /************    Seccion data        ************/

    .data  :  {*(.data*)} > public_ram 


    /************    Seccion stack       ************/
    
    .stack :
    {
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
.extern suma_c
.extern mult_c
.extern _PUBLIC_STACK_INIT

.global _start
.code 32
.section .text

_start:
    LDR SP,=_PUBLIC_STACK_INIT

    MOV R0, #0x10
    MOV R1, #0x20

    /*B suma_c*/

    LDR R10, =suma_c    @Direccion de la funcion ensamblada
    MOV LR, PC
    BX R10

    LDR R10, =mult_c
    MOV LR, PC
    BX R10

    B .     @Branch a este lugar infinitas veces parecido a while (1)
.end

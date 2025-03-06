.code 32
.extern suma_c
.section .kernel, "a"
.global _kernel

_kernel:
    MOV R0, #0x10
    MOV R1, #0x20

    ADD R0 ,R0, R1

    B .
.end

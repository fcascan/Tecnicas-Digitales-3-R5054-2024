.code 32    /* Modo de funcionamiento ARM */

.extern _start
.section ._reset_code
.global _reset_vector

_reset_vector:
    B   _start
.end

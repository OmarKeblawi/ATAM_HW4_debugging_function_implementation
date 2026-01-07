.global target_func
.text
target_func:
    nop              # Opcode 0x90 (NOT 0x55)
    ret
.global main
main:
    call target_func
    ret

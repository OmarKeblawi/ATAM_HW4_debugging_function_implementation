.global target_func
.text
target_func:
    mov $42, %eax    # Directly returns 42, no push %rbp
    ret
.global main
main:
    call target_func
    xor %eax, %eax
    ret
    
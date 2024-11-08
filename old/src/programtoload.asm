BITS 32
CPU 386
ORG 3000000000

dd 0xDDDDDDDD

start:
    mov eax, 0xDDDDDDDD
    ret


die:
    cli
    hlt
    jmp die
    ret
.model small
.data
    Bnm1 db 10011b
    Str2 db 'str'
    Dnm3 dd 10x
.code
    label1:
    std
    jnge Label2
    jmp Label2
    push DNM3
    push Dnm3
    pop eax
    pop ebx
    idiv eax
    add cl, fs:Bnm1
    adc Dnm3, ebx
    in eax,001b
    or bnm1[edi], 000000b
    jnge label1
    jmp label1
    Label2:
END

DATA SEGMENT
    Var1    DB  43h
    Var2    DW  123
    Var3    DD  1010101010b
DATA ENDS


CODE SEGMENT
lab1:
    clc
    neg eax

    dec dword ptr ES:[edx+esi*4+6]
    add eax, 5

    test ebx, [ecx+edi*8+1]
    and [ebx+eax*2+2], ecx
    mov eax, ebx
    cmp dword ptr [edx+esi*4+3], 5
    jnbe lab2


lab2:
    jmp lab1

CODE ENDS
END

Data SEGMENT
    Var1 DB 012h
    Var2 DW 232
    Var3 DD 0001110b
    String DB "Test string"
Data ENDS

Code SEGMENT
ASSUME CS:CODE, ES:DATA     

label1:
    stc
    push bx
    imul dword ptr[bx+si]
    mov ecx, ebx
    adc ebx, dword ptr FS:[eax+esi]
    sub [ebx+eax], eax
    xor word ptr[ebx+ecx], 231
    jmp label2
    jmp label1
    jc label1
    jc label2

label2:
    cmp eax, 032h
Code ENDS
END
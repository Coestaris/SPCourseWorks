Data Segment

     Hex1    db  21h
     Dec2    dw  12
     Bin2    dd  0111110b
     Txt3    db  "string"

Data ends

Assume cs:Code, ds:Data
Code Segment

lable1:
    stc
    push ebx
    jmp lable2
    jnc lable2
    Mov eax, esi
    mul SS:dword ptr[ebx+edi+9]
    Xor ax, ES:dec2[ebx+edi+1]
    Btr ES:dec2[ebx+edi+1], ax
    Sub bx, 07
    Adc ES:Bin2[bx+si+8], 05
    Jnc lable1
    jmp lable1
lable2:

Code ends
END

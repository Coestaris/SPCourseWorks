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
    Mov dx, si
    mul dword ptr[edx+esi+2]
    Xor ecx, DS:BIN2
    Btr ES:dec2[bx+di+1], ax
    Sub ebx, 07
    Adc byte ptr[bp+si+8], 05
    Jnc lable1
    jmp lable1
lable2:

Code ends
END
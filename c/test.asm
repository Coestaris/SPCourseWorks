.386
DATA SEGMENT
    Var1    DB  43h
    Var2    DB  -46h
    Var3    DB  3h
var31    DB  -128
    Var4    DW  123
    Var5    DD  1010101010b
Var6    DD  0f2ab34cdh
Var7    DD  f2ab34cdh
var11 db 1b
var db -130

DATA ENDS

CODE SEGMENT
assume CS:CODE, DS:DATA
jmp Var5
    jmp lab1
    jnbe lab1
lab1:

    jmp lab1
    jnbe lab1

    jmp lab2
    jnbe lab2

    clc
    neg al
    neg bl
    neg eax
    neg ecx
    neg edx
    neg esi
neg esp

    dec dword ptr ES:[edx+esi*4+6]
    dec byte ptr SS:[eax+esi*2+3]
    dec byte ptr [ecx+esi*4+55]
    dec dword ptr [edx+edi*1+42]
    dec dword ptr [eax+ebx*2+25]
    dec dword ptr [ecx+ecx*8+1234]
dec dword ptr ds:[ecx+ecx*2+12]
dec dword ptr [esp+ecx*8+1234]
dec dword ptr ss:[esp+ecx*8+1234]
    add ecx, 45
    add edx, 053525h
    add cl, -35
    add esi, -4535
adD esp, 1
AdD eax, 1
AdD eax, eax
    test ebx, [edx+edi*2+1]
    test al, [eax+edi*4+2]
    test cl, [ecx+ebx*8+3]
    test ecx, [ebx+edx*1+66]
    test esi, [eax+ecx*4+4]
    test edx, [edx+esi*2+6666]


    jmp lab2
    jnbe lab2

lab2:
    jmp lab1
    jnbe lab1

jmp lab3
lab3:
      jnbe lab3
dec dword ptr ss:[esp+esp*8+1234]
and ecx+eax*2+22], al
and [ecx+eax+22], al
cmp ebx, 43
mov esi, 0
cmp byte ptr [edx+esi*4+3], 0101110110b
CODE ENDS
END

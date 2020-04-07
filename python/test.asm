DATA SEGMENT
	Var1    DB  0fch
	Var2    DW  103
	Var3    DD  0101010101b
DATA ENDS


CODE SEGMENT
label1:
    cld
    dec eax
    dec ecx
    dec esi
    dec al
    dec dh
    dec bl

    inc Var1[eax+edx]
    inc Var3[eax+edx]
    inc Var1[ecx+edx]
    inc ES:Var1[eax+esi]

    cmp eax, Var3[eax+esi]
    cmp al, Var1[eax+esi]
    cmp ebx, Var3[ecx+edi]
    cmp esi, GS:Var3[edi+ecx]

    shl Var1[eax+edx], cl
    shl Var3[eax+edx], cl
    shl Var3[eax+edx], cl

    in al, 72
    in al, 22
    in eax, 32
    in eax, 023h

    add Var3[ecx+ebx], 3h
    add GS:Var3[esi+edi], 2323
    add Var1[ecx+eax], 01010101b

    jnz label1
    jnz label2

    jmp label1
    jmp label2

label2:
    or ebx, ecx
    or ecx, eax
    or bl, ch
    or al, ah

CODE ENDS
END
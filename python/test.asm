
DATA SEGMENT
	Var1    DB  0fch
Var10    DB  -128
Var11    DB  0fh
	Var2    DW  103
Var20    DW  -32768
	Var3    DD  0101010101b
Var30    DD  0ffffffffh
Var301111    DD  0ffffffffh
Var30    DD  0ffffffffh
DATA ENDS


CODE SEGMENT
Var31    DD  0fh
    jnz label1
    jmp label1
label1:
    jnz label1
    jmp label1

    jnz label2
    jmp label2

    cld
    dec eax
    dec ecx
    dec esi
    dec al
    dec dh
    dec bl
dec esp

    inc Var1[eax+edx]
    inc Var3[eax+edx]
    inc Var1[ecx+edx]
    inc ES:Var1[eax+esi]
inc dS:Var1[eax+esi]
inc ES:Var1[esp+ebp]
inc ES:Var1[eax+ebp]
    cmp eax, Var3[eax+esi]
    cmp al, Var1[eax+esi]
    cmp ebx, Var3[ecx+edi]
    cmp esi, GS:Var3[edi+ecx]

    shl Var1[eax+edx], cl
    shl Var3[eax+edx], cl
    shl Var3[eax+edx], 1

    in al, 72
    in al, 10b
    in eax, 32
    in eax, 023h
in al,255
in al,-25
in eax,256

    add Var3[ecx+ebx], 3h
    add GS:Var3[esi+edi], -23
    add Var1[ecx+eax], 01010101b
   add Var1[ecx+eax], bl
 add Var1[ecx+eax], 3abh
    jnz label1
    jnz label2

    jmp label1
    jmp label2

label2:
jmp label2
    or ebx, ecx
    or ecx, eax
    or bl, ch
    or al, ah

CODE ENDS
END 
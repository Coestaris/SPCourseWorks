DATA SEGMENT 
	Var1    DB  0fch
	Var2    DW  103
	Var3    DD  0101010101b
DATA ENDS


CODE SEGMENT
label1:
    cld
    dec eax
    inc Var1[eax+edx]

    cmp eax, ES:Var3[ecx+edi]
    shl VAR3[eax+edx], cl

    in eax, 32
    add Var3[ecx+ebx], 3h
    jnz label1
    jmp label2

label2:
    or ebx, ecx

CODE ENDS
END
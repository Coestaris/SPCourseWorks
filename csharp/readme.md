## C# version

To build use Visual studio solution (Mono compatible)

Sample test file:
```asm
DATA SEGMENT 
	Var1    DB  03fh
	Var2    DW  103
	Var3    DD  0101010101b
	String  DB  "Test string"
    equ1    EQU 5
DATA ENDS

CODE SEGMENT
label1:
    movsb
    pop eax

 Varc1 dd 10h

    dec Varc1[eax+edx]
    add eax, ecx

    cmp eax, FS:Var3[ecx+edi]
    bt Var3[eax+edx], eax
    jbe label1

    mov eax, 10

    or Var1[eax+edi], equ1
    jbe label2

label2:
    IF equ1
        mov eax, 4
    ELSE
        mov eax, 7
    ENDIF

CODE ENDS
END
```

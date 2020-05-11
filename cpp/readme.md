## CPP version

To build use MS Visual Studio Solution (Windows only)

Sample test file:

```asm
DATA1 SEGMENT
    P3 db 10010b
    PSTR db "New String"
    P1 dw 0b
    P2 dw 0ABh
    P4 dw 1b
    MACROP dw 1b
DATA1 ENDS

NOPAR MACRO
	mov si,10
endm

WITHPAR MACRO VARIABLE
    and dx,[bx+di]
    inc VARIABLE
endm

CODE SEGMENT
   neg byte ptr [bp+si]
METKA1:
    mov ax, 12h
    mov di, 2h
    WITHPAR cx
    inc ax
    NOPAR
    neg word ptr[bx+di]
    aas
    or byte ptr GS:[bx+si],2
    bt di, ax
    cmp word ptr [bx+di],ax
    jl METKA2
    mov cx, 7h
    or byte ptr [bx+di],17
    mov dx, 5h
    and dx, [bx+di]
    inc dx
    cmp word ptr [bx+si], dx
    jl METKA1
METKA2:

CODE ENDS
END
```

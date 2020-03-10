.386
.model small
.DATA
	Hex1 db 21h
	Dec1 dw 12
	Bin1 dd 0111110b
.CODE

jmp label1

label2:
	lahf
	inc ax
	dec Dec1[eax+edi*4]
	add eax, ebx
	jng label1
	lea ebx, ES:Bin1[eax+edi]
	and Dec1[ebx+edi*2], 02h
	mov eax, 0001b
label1:
    test ES:Hex1[edx+edi*4], bh
	jng label2
END

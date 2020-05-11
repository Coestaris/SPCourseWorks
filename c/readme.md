## C Version

To build `n run using GCC:
```bash
sudo apt install make cmake
cmake CMakeLists.txt
make all
./c
```

To build using Windows simply open MS Visual Studio solution.

Sample test file:
```asm
DATA SEGMENT
    Var1    DB  43h
    Var2    DW  123
    Var3    DD  1010101010b
DATA ENDS


CODE SEGMENT
lab1:
    clc
    neg eax
    neg bl

    and [ebx+eax*2+2], ecx
    and byte ptr [ebx+eax*2+2], cl

    add ecx, 5
    add edi, 99999
    add bl, 4

    dec byte ptr ES:[edx+esi*4+6]

    test ebx, [ecx+edi*8+1]

    dec byte ptr [eax+edi*1+2]
    dec dword ptr DS:[ecx+eax*8+16]

    mov eax, ebx
    mov al, bl
    cmp dword ptr [edx+esi*4+3], 5
    jnbe lab2


lab2:
    jmp lab1

CODE ENDS
END
```

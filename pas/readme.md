## Pascal version

To build project using Debian systems use:
```shell
sudo apt install fp-compiler
./build # Script to build and run project
# Use "./build clear" to clear generated info
# Use "./build build" just to build project and don't run it
```

Sample test file:
```asm
.model small

.data
  var1 db 164
  var2 dw 03e3h
  var3 dd 0110101b
  var4 db 52


.code

label1:
  sahf
  sar ax, 1
  sar eax, 1
  neg var1[eax * 4]
  bt cx, bx
  imul cx, DS:var2[ebx * 4]
  test FS:var2[ecx*4], ax
  test var1[edx*2], bl
  mov dx, -412
  mov al, 52
  xor var2[edi*8], 1000h
  xor var1, 15
  xor var3, 15
  jns label1
  jmp label1

  jns label2
  jmp label2

label2:
end
```

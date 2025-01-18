format elf64 executable
entry _start
segment readable executable
_start:
sub rsp, 0x10000
xor ebx, ebx
xor eax, eax
xor edi, edi
lea rsi, [rsp+rbx]
mov edx, 0x01
syscall
add byte [rsp+rbx], -10
loop0:
cmp byte [rsp+rbx], 0x00
jz eloop0
add byte [rsp+rbx], 10
mov eax, 0x01
mov edi, eax
lea rsi, [rsp+rbx]
mov edx, eax
syscall
xor eax, eax
xor edi, edi
lea rsi, [rsp+rbx]
mov edx, 0x01
syscall
add byte [rsp+rbx], -10
jmp loop0
eloop0:
add byte [rsp+rbx], 10
mov eax, 0x01
mov edi, eax
lea rsi, [rsp+rbx]
mov edx, eax
syscall
mov eax, 0x3C
xor edi, edi
syscall

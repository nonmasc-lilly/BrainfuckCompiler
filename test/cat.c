#include <stdio.h>
int main(int argc, char **argv) {
unsigned char memory[0x10000] = {0};
unsigned short mem_ptr = 0x00;
memory[mem_ptr] = getchar();
memory[mem_ptr] += -10;
while(memory[mem_ptr]) {
memory[mem_ptr] += 10;
putchar(memory[mem_ptr]);
memory[mem_ptr] = getchar();
memory[mem_ptr] += -10;
}
memory[mem_ptr] += 10;
putchar(memory[mem_ptr]);
return 0;
}

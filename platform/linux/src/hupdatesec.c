#include <stdio.h>
#include <stdlib.h>

int main(int argc,char* argv[])
{
    system("objcopy.exe --add-section .hsme=h.hsme HalSMExecutable.o h.o");
    return 0;
}
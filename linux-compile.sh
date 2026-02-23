gcc -c src/HalSMExecutable.c -Iinclude -m64 -ffreestanding -nostdlib -fPIC -o build/HalSMExecutableLinux.o
ar rcs lib/libHalSMExecutableLinux.a build/HalSMExecutableLinux.o
gcc -m64 -nostdlib -shared -o lib/libHalSMExecutableLinux.so build/HalSMExecutableLinux.o -ffreestanding -fPIC -Llib -lHalStringFormatLinux -lHalFileSystemLinux -lHalSMLinux
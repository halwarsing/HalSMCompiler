gcc -fPIC -o ../../build/platform/win64/hupdatesec.exe src/hupdatesec.c
gcc -fPIC -o ../../build/platform/win64/HalSMExecutable.exe src/HalSMExecutableWin64.c -I../../include -L../../lib -lHalStringFormat -lHalFileSystem -lHalSM -lHalSMExecutable
:: objcopy --set-section-flags .hsmes=contents,alloc,load,data ../../build/platform/win64/HalSMExecutable.exe
:: objcopy --remove-section .hsmeSection ../../build/platform/win64/HalSMExecutable.exe
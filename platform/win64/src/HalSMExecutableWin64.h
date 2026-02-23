#ifndef HALSMEXECUTABLEWIN64_H
#define HALSMEXECUTABLEWIN64_H

#include <HalSM.h>
#include <HalFileSystem.h>

typedef struct FilePointer {
    unsigned long long int index;
    unsigned long long int size;
    unsigned char* p;
} FilePointer;

#endif
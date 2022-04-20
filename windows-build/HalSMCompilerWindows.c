#include <stdio.h>
#include "HalSMCompilerWindows.h"
#include <HalSM.h>
#include <stdlib.h>
#include <windows.h>

void print(char* text) {
    printf("%s\n",text);
}

void printError(char* text) {
    fprintf(stderr,"%s\n",text);
}

char* input(char* text) {
    printf("%s",text);
    char* t=malloc(255);
    fgets(t,255,stdin);
    return t;
}

char* readFile(char* path) {
    FILE *f=fopen(path,"r");
    char *out=malloc(0);
    int d=0;
    char c;
    while ((c=fgetc(f))!=EOF)
    {
        d++;
        out=realloc(out,d);
        out[d-1]=c;
    }
    d++;
    out=realloc(out,d);
    out[d-1]='\0';
    fclose(f);
    return out;
}



char* Decimal2Str(long long number) {
    char* out=calloc(40,sizeof(char));
    sprintf(out,"%d",number);
    return out;
}

char* Double2Str(double d) {
    char* out=calloc(22,sizeof(char));
    sprintf(out,"%lf",d);
    return out;
}

char* Decimal2HexStr(long long number) {
    char* out=calloc(34,sizeof(char));
    sprintf(out,"%llx",number);
    return out;
}

long long ParseDecimal(char* number) {
    return strtoll(number,NULL,0);    
}

double ParseDouble(char* number) {
    return strtod(number,NULL);
}



void* loadsharedlibrary_load(char* path)
{
    HINSTANCE* out=malloc(sizeof(HINSTANCE));
    *out=LoadLibrary(TEXT(path));
    return out;
}

void* loadsharedlibrary_getAddressByName(void* library,char* name)
{
    return GetProcAddress(*(HINSTANCE*)library,name);
}

void loadsharedlibrary_close(void* library)
{
    FreeLibrary(*(HINSTANCE*)library);
}



void* mem_alloc(unsigned int size)
{
    return malloc(size);
}

void* c_alloc(unsigned int size,unsigned int n)
{
    return calloc(size,n);
}

void* re_alloc(void* ptr,unsigned int size)
{
    return realloc(ptr,size);
}

void free_mem(void* ptr)
{
    free(ptr);
}



void system_exit(int status)
{
    exit(status);
}

void main(int argc,char *argv[]) {
    HalSMMemoryManagmentLibrary memorymanagmentlibrary={&mem_alloc,&c_alloc,&re_alloc,&free_mem};
    HalSMLoadSharedLibrary loadsharedlibrary={&loadsharedlibrary_load,&loadsharedlibrary_getAddressByName,&loadsharedlibrary_close};
    HalSMStringLibrary stringlibrary={&Decimal2Str,&Decimal2HexStr,&Double2Str,&ParseDecimal,&ParseDouble};
    HalSMSystemLibrary systemlibrary={&system_exit};
    HalSM* hsm=HalSM_init(HalSMArray_init(&memorymanagmentlibrary),print,printError,input,readFile,"",&loadsharedlibrary,&stringlibrary,&memorymanagmentlibrary,&systemlibrary);
    HalSM_compile_without_path(hsm,halsmfile);
}
#include "HalSMExecutableLinux.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <locale.h>
#include <time.h>
#include <HalSMExecutable.h>
#include <string.h>
#include <elf.h>
#include <sys/auxv.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#if defined(__LP64__)
#define ElfW(type) Elf64_ ## type
#else
#define ElfW(type) Elf32_ ## type
#endif

unsigned char* hsmeSectionStart;

HalSMNull hnulld;
HalSMVariable nulld={&hnulld,HalSMVariableType_HalSMNull};

//HALSM

//HalSM Memory Library

void* HML_malloc(unsigned long long int size) {
    return malloc(size);
}

void* HML_calloc(unsigned long long int n,unsigned long long int size) {
    return calloc(n,size);
}

void* HML_realloc(void* p,unsigned long long int nsize) {
    return realloc(p,nsize);
}

void HML_free(void* p) {free(p);}

//HalSM Memory Library

//HalSM String Library

HalStringFormatChar* HSL_Decimal2Str(long long d)
{
    char* out=calloc(21,sizeof(char));
    sprintf(out,"%lld",d);
    return U8H(out);
}

HalStringFormatChar* HSL_Decimal2HexStr(long long d)
{
    char* out=calloc(21,sizeof(char));
    sprintf(out,"%x",d);
    return U8H(out);
}

HalStringFormatChar* HSL_Double2Str(double d)
{
    char* out=calloc(256,sizeof(char));
    sprintf(out,"%d",d);
    return U8H(out);
}

long long HSL_ParseDecimal(HalStringFormatChar* str)
{
    return strtoll(HSF2ASCII(str),NULL,10);
}

double HSL_ParseDouble(HalStringFormatChar* str)
{
    return strtod(HSF2ASCII(str),NULL);
}

//HalSM String Library

//HalSM System Library

void HSYSL_exit(int code){exit(code);}

//HalSM System Library

void HSM_print(HalStringFormatChar* str){printf("%s\n",HU8(str));}

void HSM_printError(HalStringFormatChar* str){printf("ERROR: %s\n",HU8(str));}

HalStringFormatChar* HSM_input(HalStringFormatChar* str){printf("%s",HU8(str));char* out=calloc(100,sizeof(char));fgets(out,100,stdin);return U8H(out);}

HalSMVariable* HSM_getVariable(HalStringFormatChar* name){return &nulld;}

//HalSM File System Library

void* HFSL_openFile(HalStringFormatChar* path,HalStringFormatChar* mode){return fopen(HU8(path),HU8(mode));}

void HFSL_closeFile(void* p){fclose(p);}

unsigned long long int HFSL_read(void* out,unsigned long long int size,unsigned long long int count,void* p) {
    return fread(out,size,count,p);
}

unsigned long long int HFSL_write(void* in,unsigned long long int size,unsigned long long int count,void* p) {
    return fwrite(in,size,count,p);
}

void HFSL_puts(void* p,HalStringFormatChar* str){fputs((char*)str,p);}

void HFSL_seek(void* p,unsigned long long int offset,unsigned long long int origin){fseek(p,offset,origin);}

unsigned long long int HFSL_tell(void* p){return ftell(p);}

void HFSL_rewind(void* p){rewind(p);}

void HFSL_flush(void* p){fflush(p);}

int HFSL_eof(void* p){return feof(p);}

unsigned char HFSL_isExistsDir(HalStringFormatChar* path){
    struct stat sb;
    if (stat(HU8(path), &sb) == 0 && S_ISDIR(sb.st_mode)) {
        return 1;
    }
    return 0;
}

unsigned char HFSL_isExistsFile(HalStringFormatChar* path){
    struct stat sb;
    if (stat(HU8(path), &sb) == 0 && S_ISDIR(sb.st_mode)==0) {
        return 1;
    }
    return 0;
}

HalSMArray* HFSL_getListFiles(HalSMCompiler* hsmc,HalStringFormatChar* path){return HalSMArray_init(hsmc->memorymanagmentlibrary);}

HalSMVariable* HSMLoadModule(HalSMCompiler* hsmc,HalStringFormatChar* name){return &nulld;}

unsigned long long int HFS_read(void* out,unsigned long long int size,unsigned long long int count,void* p) {
    FilePointer* fp=(FilePointer*)p;
    memcpy(out,fp->p+fp->index,size*count);
    fp->index+=size*count;
    return size*count;
}

unsigned long long int HFS_write(void* out,unsigned long long int size,unsigned long long int count,void* p){return 0;}

void HFS_seek(void* p,unsigned long long int s,unsigned long long int w) {
    FilePointer* fp=(FilePointer*)p;
    if (w==SEEK_SET) {
        fp->index=s;
    } else if (w==SEEK_END) {
        fp->index=fp->size-s;
    }
}

unsigned long long int HFS_tell(void* p) {
    return ((FilePointer*)p)->index;
}

int HFS_eof(void* p) {
    return 0;
}

void HFS_flush(void* p){}

int main(void) {
    HalStringFormat_init((void*(*)(unsigned long long int))malloc);
    HalSMStringLibrary hsl={HSL_Decimal2Str,HSL_Decimal2HexStr,HSL_Double2Str,HSL_ParseDecimal,HSL_ParseDouble};
    HalSMMemoryManagmentLibrary hmml={HML_malloc,HML_calloc,HML_realloc,HML_free};
    HalSMSystemLibrary hsysl={HSYSL_exit};
    HalSMFileSystemLibrary hfsl={HFSL_isExistsDir,HFSL_isExistsFile,HFSL_getListFiles,HFSL_openFile,HFSL_closeFile,HFSL_read,HFSL_write,HFSL_puts,HFSL_seek,HFSL_tell,HFSL_rewind,HFSL_flush,HFSL_eof};
    HalSMCompiler* hsmc=HalSMCompiler_init(HSM_print,HSM_printError,HSM_input,HSM_getVariable,&hsl,&hmml,&hsysl,&hfsl,HSMLoadModule);
    HalFileSystem hfs;
    HalFileSystem_init(&hfs,HFS_read,HFS_write,HFS_seek,HFS_tell,HFS_eof,HFS_flush);
    //uintptr_t main_addr = (uintptr_t)main;

    // clear bottom 12 bits so that it points to the beginning of page
    //main_addr &= ~0xFFFLLU;

    // subtract one page so that we're in the elf headers...
    //main_addr -= 0x1000;
    //void* data=(void*)getauxval(AT_ENTRY);
    char* fn=(char*)getauxval(AT_EXECFN);
    int fd=open(fn,O_RDONLY);
    size_t size=lseek(fd, 0, SEEK_END);
    void* data=mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    ElfW(Ehdr)* header=(ElfW(Ehdr)*)data;
    ElfW(Shdr)* shdr=(ElfW(Shdr)*)(data+header->e_shoff);
    ElfW(Shdr)* symtab;
    ElfW(Shdr)* shstrtab;
    ElfW(Shdr)* strtab;
    char* str=(char*)(data+shdr[header->e_shstrndx].sh_offset);
    for (int i=0;i<header->e_shnum;i++) {
        if (strncmp(&str[shdr[i].sh_name],".hsme",5)==0) {
            hsmeSectionStart=data+shdr[i].sh_offset;
            FilePointer* fp=malloc(sizeof(FilePointer));
            fp->index=0;
            fp->p=&hsmeSectionStart[8];
            fp->size=*(unsigned long long int*)hsmeSectionStart;
            /*for (unsigned long long int b=0;b<fp->size;b++) {
                printf("%02x",fp->p[b]);
            }
            printf("\n");*/
            HalFileSystem_open(&hfs,fp,0);
            HalSMExecutable hsme;
            HalSMExecutable_init(&hsme,&hfs);
            if (HalSMExecutable_load(&hsme,hsmc)){printf("Error: load HalSMExecutable unsuccessfully\n");return -1;}
            HalSMExecutable_run(&hsme,hsmc);
            close(fd);
            munmap(data,size);
            return 0;
        }
    }
    close(fd);
    munmap(data,size);

    return -1;
}
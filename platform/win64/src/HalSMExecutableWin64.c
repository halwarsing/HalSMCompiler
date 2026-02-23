#include "HalSMExecutableWin64.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <Windows.h>
#include <locale.h>
#include <time.h>
#include <HalSMExecutable.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
unsigned char* hsmeSectionStart;
HalSMNull hnulld;
HalSMVariable nulld={&hnulld,HalSMVariableType_HalSMNull};
Dict nullhsmcwdict={0,NOTHALSMNULLPOINTER};
char* tempPathFile=NULL;
HalSMArray* dllModules=NULL;
HalFileSystem hfs;
#define sizebuffer 4096U

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

HalStringFormatChar* getNameFile(HalStringFormatChar* path) {
    HalStringFormatChar* temp=malloc(0);
    HalStringFormatChar t;
    unsigned long long int i,l,size;
    l=HalStringFormat_length(path);
    size=0;
    HalStringFormatChar* p=path;
    for (i=0;i<l;i++,p++) {
        t=*p;
        if (HSFCharCompare(t,ASCIIH('/'))||HSFCharCompare(t,ASCIIH('\\'))) {
            size=0;
            free(temp);
            temp=malloc(0);
        } else {
            temp=realloc(temp,size*sizeof(HalStringFormatChar)+sizeof(HalStringFormatChar));
            temp[size++]=t;
        }
    }
    temp=realloc(temp,size*sizeof(HalStringFormatChar)+sizeof(HalStringFormatChar));
    temp[size]=ASCIIH('\0');
    return temp;
}

HalSMVariable* HSMLoadModule(HalSMCompiler* hsmc,HalStringFormatChar* name) {
    HalStringFormatChar* modulePath=name,*moduleDirPath,*dataStr;
    unsigned char isModule=0,*data;
    unsigned long long int size,i,l;
    HalSMCompiler* hsmctemp;
    HalSMVariable* res;
    char exePathStr[MAX_PATH+1]={0};
    GetModuleFileNameA(NULL,exePathStr,MAX_PATH);
    HalStringFormatChar* exePath=U8H(exePathStr);
    long long int indexStr=StringLastIndexOf(hsmc->memorymanagmentlibrary,exePath,U8H("\\"));
    if (indexStr>-1){exePath[indexStr+1]=ASCIIH('\0');}
    hsmctemp=HalSMCompiler_init(hsmc->print,hsmc->printErrorf,hsmc->inputf,hsmc->getVariable,hsmc->stringlibrary,hsmc->memorymanagmentlibrary,hsmc->systemlibrary,hsmc->filesystemlibrary,hsmc->HalSMLoadModule);
    if (HSFCharCompare(modulePath[1],ASCIIH(':'))==0) {
        if (StringIndexOf(hsmc->memorymanagmentlibrary,modulePath,U8H("/"))==-1&&(tempPathFile!=NULL)) {
            moduleDirPath=ConcatenateStrings(hsmc->memorymanagmentlibrary,U8H(tempPathFile),ConcatenateStrings(hsmc->memorymanagmentlibrary,ConcatenateStrings(hsmc->memorymanagmentlibrary,U8H("\\"),modulePath),U8H("\\")));
            modulePath=ConcatenateStrings(hsmc->memorymanagmentlibrary,moduleDirPath,U8H("module.hcfg"));
            isModule=1;
        } else {
            modulePath=ConcatenateStrings(hsmc->memorymanagmentlibrary,modulePath,U8H(".hsm"));
        }
    } else {
        modulePath=ConcatenateStrings(hsmc->memorymanagmentlibrary,modulePath,U8H(".hsm"));
    }
    FILE* fp=fopen(HU8(modulePath),"rb");
    if (fp==NULL) {
        if (isModule==1) {
            modulePath=ConcatenateStrings(hsmc->memorymanagmentlibrary,name,U8H(".hsm"));
            isModule=0;
            fp=fopen(HU8(modulePath),"rb");
            if (fp==NULL){return &nulld;}
        } else {
            return &nulld;
        }
    }
    if (isModule==1) {
        fseek(fp,0L,SEEK_END);
        size=ftell(fp);
        fseek(fp,0L,SEEK_SET);
        data=calloc(size+1,sizeof(char));
        fread(data,1,size,fp);
        fclose(fp);
        dataStr=U8H(data);
        free(data);
        if (HSFCharCompare(dataStr[0],ASCIIH('{'))==0||HSFCharCompare(dataStr[HalStringFormat_length(dataStr)-1],ASCIIH('}'))==0||HalStringFormat_length(dataStr)<3) {
            free(dataStr);
            return &nulld;
        }
        string_cpy(dataStr,&dataStr[1]);
        dataStr[HalStringFormat_length(dataStr)-1]=ASCIIH('\0');
        Dict* config=HalSMCompiler_getArgsDict(hsmc,dataStr,&nullhsmcwdict);
        free(dataStr);
        HalSMVariable* tempKey=HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("platforms"));
        if (DictElementIndexByKey(hsmc,config,tempKey)==-1){return &nulld;}
        HalSMVariable* tempValue=DictElementFindByKey(hsmc,config,tempKey)->value;
        if (tempValue->type!=HalSMVariableType_HalSMDict){return &nulld;}
        Dict* platforms=(Dict*)tempValue->value;
        tempKey=HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("win64"));
        if (DictElementIndexByKey(hsmc,platforms,tempKey)==-1){return &nulld;}
        tempValue=DictElementFindByKey(hsmc,platforms,tempKey)->value;
        if (tempValue->type!=HalSMVariableType_HalSMDict){return &nulld;}
        Dict* platform=(Dict*)tempValue->value;
        tempKey=HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("main"));
        if (DictElementIndexByKey(hsmc,platform,tempKey)==-1){return &nulld;}
        tempValue=DictElementFindByKey(hsmc,platform,tempKey)->value;
        if (tempValue->type!=HalSMVariableType_str){return &nulld;}
        HalStringFormatChar* platformMain=*(HalStringFormatChar**)tempValue->value;
        if (string_len(platformMain)==0){return &nulld;}
        tempKey=HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("module"));
        if (DictElementIndexByKey(hsmc,platform,tempKey)==-1){return &nulld;}
        tempValue=DictElementFindByKey(hsmc,platform,tempKey)->value;
        if (tempValue->type!=HalSMVariableType_str){return &nulld;}
        HalStringFormatChar* platformModulePath=*(HalStringFormatChar**)tempValue->value;
        if (string_len(platformModulePath)==0){return &nulld;}
        platformModulePath=ConcatenateStrings(hsmc->memorymanagmentlibrary,moduleDirPath,platformModulePath);
        data=HU8(platformModulePath);
        free(platformModulePath);
        HINSTANCE dllModule=LoadLibraryA(data);
        HalSMArray_add(hsmc->memorymanagmentlibrary,dllModules,HalSMVariable_init(hsmc->memorymanagmentlibrary,dllModule,HalSMVariableType_void));
        free(data);
        if (dllModule==NULL){return &nulld;}
        char* platformMainStr=HU8(platformMain);
        free(platformMain);
        HalSMCModule_entry*(*platformMainFunc)(HalSMCompiler*,HalSMArray*)=(HalSMCModule_entry*(*)(HalSMCompiler*,HalSMArray*))GetProcAddress(dllModule,platformMainStr);
        free(platformMainStr);
        if (platformMainFunc==NULL){return &nulld;}
        HalSMCModule_entry* moduleEntry=platformMainFunc(hsmc,HalSMArray_init(hsmc->memorymanagmentlibrary));
        return HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMCModule_load(hsmc,moduleEntry),HalSMVariableType_HalSMCModule);
    } else {
        fseek(fp,0L,SEEK_END);
        size=ftell(fp);
        fseek(fp,0L,SEEK_SET);
        if (size==0){return &nulld;}
        data=calloc(size+1,sizeof(char));
        data[size]='\0';
        fread(data,1,size,fp);
        fclose(fp);
        dataStr=U8H(data);
        free(data);
        res=HalSMCompiler_compile(hsmctemp,dataStr,0,&nulld);
        free(dataStr);
        return HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMModule_init(hsmc->memorymanagmentlibrary,getNameFile(name),hsmctemp),HalSMVariableType_HalSMModule);
    }
}

void fnExit(void) {
    if (tempPathFile!=NULL) {
        SHFILEOPSTRUCTA shfo = {
            NULL,
            FO_DELETE,
            tempPathFile,
            NULL,
            FOF_SILENT|FOF_NOERRORUI|FOF_NOCONFIRMATION,
            FALSE,
            NULL,
            NULL
        };
        unsigned long long int i;
        if (dllModules!=NULL) {
            for (i=0;i<dllModules->size;i++) {
                FreeLibrary((HINSTANCE)dllModules->arr[i]->value);
            }
        }
        SHFileOperationA(&shfo);
    }
}

int main(void) {
    atexit(fnExit);
    HalStringFormat_init((void*(*)(unsigned long long int))malloc);
    SetConsoleOutputCP(CP_UTF8);
    HalSMStringLibrary hsl={HSL_Decimal2Str,HSL_Decimal2HexStr,HSL_Double2Str,HSL_ParseDecimal,HSL_ParseDouble};
    HalSMMemoryManagmentLibrary hmml={HML_malloc,HML_calloc,HML_realloc,HML_free};
    HalSMSystemLibrary hsysl={HSYSL_exit};
    HalSMFileSystemLibrary hfsl={HFSL_isExistsDir,HFSL_isExistsFile,HFSL_getListFiles,HFSL_openFile,HFSL_closeFile,HFSL_read,HFSL_write,HFSL_puts,HFSL_seek,HFSL_tell,HFSL_rewind,HFSL_flush,HFSL_eof};
    HalSMCompiler* hsmc=HalSMCompiler_init(HSM_print,HSM_printError,HSM_input,HSM_getVariable,&hsl,&hmml,&hsysl,&hfsl,HSMLoadModule);
    dllModules=HalSMArray_init(hsmc->memorymanagmentlibrary);
    HalFileSystem_init(&hfs,HFS_read,HFS_write,HFS_seek,HFS_tell,HFS_eof,HFS_flush);
    //void* imageBase=(void*)0x00010000;
    PIMAGE_NT_HEADERS pinth=(PIMAGE_NT_HEADERS)(((PBYTE)&__ImageBase)+((PIMAGE_DOS_HEADER)&__ImageBase)->e_lfanew);
    unsigned long long int i,nos,l;
    nos=pinth->FileHeader.NumberOfSections;
    PIMAGE_SECTION_HEADER pish=IMAGE_FIRST_SECTION(pinth);
    for (i=0;i<nos;i++,pish++) {
        if (strncmp(pish->Name,".hsme",5)==0) {
            hsmeSectionStart=(PBYTE)&__ImageBase+pish->VirtualAddress;
            FilePointer* fp=malloc(sizeof(FilePointer));
            fp->index=0;
            fp->p=&hsmeSectionStart[8];
            fp->size=*(unsigned long long int*)hsmeSectionStart;
            HalFileSystem_open(&hfs,fp,0);
            HalSMExecutable hsme;
            HalSMExecutable_init(&hsme,&hfs);
            if (HalSMExecutable_load(&hsme,hsmc)){printf("Error: load HalSMExecutable unsuccessfully\n");return -1;}
            char tempPath[MAX_PATH+1]={0};
            char tempFilePath[MAX_PATH+1]={0};
            tempPathFile=calloc(MAX_PATH+1,sizeof(char));
            if (GetTempPathA(MAX_PATH,tempPath)==0){printf("Error: get temp path failed\n");return -1;}
            sprintf(tempPathFile,"%shsmc%lld",tempPath,time(NULL));
            if (CreateDirectoryA(tempPathFile,NULL)==0&&ERROR_ALREADY_EXISTS!=GetLastError()){printf("Error: create temp directory failed %x\n",GetLastError());return -1;}
            unsigned long long int* modulesDirectories,*modulesFiles,modulesCount,modulesFilesCount;
            if (HalFileSystem_getCountDirectoriesDirectory(&hfs,U8H("module"),&modulesCount,1)>0){modulesCount=0;}
            if (modulesCount>0) {
                modulesDirectories=calloc(modulesCount,sizeof(unsigned long long int));
                if (HalFileSystem_getListDirectoriesDirectory(&hfs,U8H("module"),modulesDirectories,&modulesCount,modulesCount,1)==0) {
                    unsigned char* temp=calloc(sizebuffer,sizeof(unsigned char));
                    for (i=0;i<modulesCount;i++) {
                        HalFileSystemFile file;
                        HalFileSystemDirectory dir;
                        if (HalFileSystem_getDirectoryByIndex(&hfs,modulesDirectories[i],&dir)>0){continue;}
                        memset(tempFilePath,'\0',MAX_PATH+1);
                        sprintf(tempFilePath,"%s\\%s",tempPathFile,HU8(dir.name));
                        if (CreateDirectoryA(tempFilePath,NULL)==0&&ERROR_ALREADY_EXISTS!=GetLastError()){continue;}
                        if (HalFileSystem_getCountFilesDirectoryByIndex(&hfs,modulesDirectories[i],&modulesFilesCount,1)>0||modulesFilesCount==0){continue;}
                        modulesFiles=calloc(modulesFilesCount,sizeof(unsigned long long int));
                        if (HalFileSystem_getListFilesDirectoryByIndex(&hfs,modulesDirectories[i],modulesFiles,&modulesFilesCount,modulesFilesCount,1)>0){continue;}
                        for (unsigned long long int j=0;j<modulesFilesCount;j++) {
                            if (HalFileSystem_getFileByIndex(&hfs,modulesFiles[j],&file)>0||file.size==0){continue;}
                            memset(tempFilePath,'\0',MAX_PATH+1);
                            sprintf(tempFilePath,"%s\\%s\\%s",tempPathFile,HU8(dir.name),HU8(file.name));
                            FILE* moduleFile=fopen(tempFilePath,"wb");
                            if (moduleFile==NULL){continue;}
                            l=file.size/sizebuffer;
                            hfs.seek(hfs.disk,hfs.startIndexFiles+hfs.maxcountfiles*hfs_size_entry+file.startAddress,SEEK_SET);
                            for (unsigned long long int k=0;k<l;k++) {
                                hfs.read(temp,sizebuffer,1,hfs.disk);
                                fwrite(temp,sizebuffer,1,moduleFile);
                            }
                            if (file.size%sizebuffer) {
                                hfs.read(temp,file.size%sizebuffer,1,hfs.disk);
                                fwrite(temp,file.size%sizebuffer,1,moduleFile);
                            }
                            fclose(moduleFile);
                        }
                    }
                }
            }

            HalSMExecutable_run(&hsme,hsmc);
            HalFileSystem_close(&hfs);
            return 0;
        }
    }

    return -1;
}
#include <HalSMCompilerWindows.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <Windows.h>
#include <locale.h>
#include <time.h>
#include <HalSMExecutable.h>

HalSMNull hnulld;
HalSMVariable nulld={&hnulld,HalSMVariableType_HalSMNull};
HalSMMemory* memory;
Dict nullhsmcwdict={0,NOTHALSMNULLPOINTER};
char* tempPathFile=NULL;
HalSMArray* dllModules=NULL;

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

void HSYSL_exit(int code) {
    if (tempPathFile!=NULL) {
        RemoveDirectoryA(tempPathFile);
    }
    exit(code);
}

//HalSM System Library

void HSM_print(HalStringFormatChar* str){printf("%s\n",HU8(str));}

void HSM_printError(HalStringFormatChar* str){printf("ERROR: %s\n",HU8(str));}

HalStringFormatChar* HSM_input(HalStringFormatChar* str){printf("%s",HU8(str));char* out=calloc(100,sizeof(char));fgets(out,100,stdin);return U8H(out);}

HalSMVariable* HSM_getVariable(HalStringFormatChar* name){return &nulld;}

//HalSM File System Library

void* HFSL_openFile(HalStringFormatChar* path,HalStringFormatChar* mode){return fopen(HU8(path),HU8(mode));}

void HFSL_closeFile(void* p){fclose(p);}

unsigned long long int HFSL_read(void* out,unsigned long long int size,unsigned long long int count,void* p){return fread(out,size,count,p);}

unsigned long long int HFSL_write(void* in,unsigned long long int size,unsigned long long int count,void* p) {
    return fwrite(in,size,count,p);
}

unsigned long long int HFS_read(void* out,unsigned long long int size,void* p){return fread(out,size,1,p);}

unsigned long long int HFS_write(void* in,unsigned long long int size,void* p) {
    return fwrite(in,size,1,p);
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

HalSMArray* HFSL_getListFiles(HalSMCompiler* hsmc,HalStringFormatChar* path){return HalSMArray_init(hsmc->memory);}

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
    long long int indexStr=StringLastIndexOf(hsmc->memory,exePath,U8H("\\"));
    if (indexStr>-1){exePath[indexStr+1]=ASCIIH('\0');}
    if (HSFCharCompare(modulePath[1],ASCIIH(':'))==0) {
        if (StringIndexOf(hsmc->memory,modulePath,U8H("/"))==-1) {
            if (tempPathFile!=NULL) {
                moduleDirPath=ConcatenateStrings(hsmc->memory,U8H(tempPathFile),ConcatenateStrings(hsmc->memory,ConcatenateStrings(hsmc->memory,U8H("\\"),modulePath),U8H("\\")));
                modulePath=ConcatenateStrings(hsmc->memory,moduleDirPath,U8H("module.hcfg"));
            } else {
                moduleDirPath=ConcatenateStrings(hsmc->memory,exePath,ConcatenateStrings(hsmc->memory,U8H("module\\"),ConcatenateStrings(hsmc->memory,modulePath,U8H("\\"))));
                modulePath=ConcatenateStrings(hsmc->memory,moduleDirPath,U8H("module.hcfg"));
            }
            isModule=1;
        } else {
            modulePath=ConcatenateStrings(hsmc->memory,modulePath,U8H(".hsm"));
        }
    } else {
        modulePath=ConcatenateStrings(hsmc->memory,modulePath,U8H(".hsm"));
    }
    FILE* fp=fopen(HU8(modulePath),"rb");
    if (fp==NULL) {
        if (isModule==1) {
            modulePath=ConcatenateStrings(hsmc->memory,name,U8H(".hsm"));
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
        HalSMVariable* tempKey=HalSMVariable_init_str(hsmc->memory,U8H("platforms"));
        if (DictElementIndexByKey(hsmc,config,tempKey)==-1){return &nulld;}
        HalSMVariable* tempValue=DictElementFindByKey(hsmc,config,tempKey)->value;
        if (tempValue->type!=HalSMVariableType_HalSMDict){return &nulld;}
        Dict* platforms=(Dict*)tempValue->value;
        tempKey=HalSMVariable_init_str(hsmc->memory,U8H("win64"));
        HalStringFormatChar* typeModule=U8H("Module");
        if (DictElementIndexByKey(hsmc,platforms,tempKey)>-1){tempValue=DictElementFindByKey(hsmc,platforms,tempKey)->value;}
        else {
            tempKey=HalSMVariable_init_str(hsmc->memory,U8H("other"));
            if (DictElementIndexByKey(hsmc,platforms,tempKey)>-1){tempValue=DictElementFindByKey(hsmc,platforms,tempKey)->value;}
            else {return &nulld;}
        }
        
        if (tempValue->type!=HalSMVariableType_HalSMDict){return &nulld;}
        Dict* platform=(Dict*)tempValue->value;

        tempKey=HalSMVariable_init_str(hsmc->memory,U8H("type"));
        if (DictElementIndexByKey(hsmc,platform,tempKey)>-1){
            tempValue=DictElementFindByKey(hsmc,platform,tempKey)->value;
            if (tempValue->type!=HalSMVariableType_str){return &nulld;}
            typeModule=*(HalStringFormatChar**)tempValue->value;
        }

        tempKey=HalSMVariable_init_str(hsmc->memory,U8H("module"));
        if (DictElementIndexByKey(hsmc,platform,tempKey)==-1){return &nulld;}
        tempValue=DictElementFindByKey(hsmc,platform,tempKey)->value;
        if (tempValue->type!=HalSMVariableType_str){return &nulld;}
        HalStringFormatChar* platformModulePath=*(HalStringFormatChar**)tempValue->value;
        if (string_len(platformModulePath)==0){return &nulld;}
        platformModulePath=ConcatenateStrings(hsmc->memory,moduleDirPath,platformModulePath);
        data=HU8(platformModulePath);
        free(platformModulePath);
        
        if (HSFCompare(typeModule,U8H("HalSM"))) {
            fp=fopen(data,"rb");
            free(data);
            fseek(fp,0L,SEEK_END);
            size=ftell(fp);
            fseek(fp,0L,SEEK_SET);
            data=calloc(size+1,sizeof(char));
            fread(data,1,size,fp);
            data[size]='\0';
            fclose(fp);
            dataStr=U8H(data);
            free(data);
            hsmctemp=HalSMCompiler_init(U8H(""),hsmc->print,hsmc->printErrorf,hsmc->inputf,hsmc->getVariable,hsmc->memory,hsmc->systemlibrary,hsmc->filesystemlibrary,hsmc->HalSMLoadModule);
            HalSMCompiler_compile(hsmctemp,dataStr,0,&nulld);
            return HalSMVariable_init(hsmc->memory,HalSMModule_init(hsmc->memory,getNameFile(name),hsmctemp),HalSMVariableType_HalSMModule);
        } else {
            tempKey=HalSMVariable_init_str(hsmc->memory,U8H("main"));
            if (DictElementIndexByKey(hsmc,platform,tempKey)==-1){return &nulld;}
            tempValue=DictElementFindByKey(hsmc,platform,tempKey)->value;
            if (tempValue->type!=HalSMVariableType_str){return &nulld;}
            HalStringFormatChar* platformMain=*(HalStringFormatChar**)tempValue->value;
            if (string_len(platformMain)==0){return &nulld;}
            HINSTANCE dllModule=LoadLibraryA(data);
            HalSMArray_add(hsmc->memory,dllModules,HalSMVariable_init(hsmc->memory,dllModule,HalSMVariableType_void));
            free(data);
            if (dllModule==NULL){return &nulld;}
            char* platformMainStr=HU8(platformMain);
            free(platformMain);
            HalSMCModule_entry*(*platformMainFunc)(HalSMCompiler*,HalSMArray*)=(HalSMCModule_entry*(*)(HalSMCompiler*,HalSMArray*))GetProcAddress(dllModule,platformMainStr);
            free(platformMainStr);
            if (platformMainFunc==NULL){return &nulld;}
            HalSMCModule_entry* moduleEntry=platformMainFunc(hsmc,HalSMArray_init(hsmc->memory));
            return HalSMVariable_init(hsmc->memory,HalSMCModule_load(hsmc,moduleEntry),HalSMVariableType_HalSMCModule);
        }
        return &nulld;
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
        hsmctemp=HalSMCompiler_init(U8H(""),hsmc->print,hsmc->printErrorf,hsmc->inputf,hsmc->getVariable,hsmc->memory,hsmc->systemlibrary,hsmc->filesystemlibrary,hsmc->HalSMLoadModule);
        res=HalSMCompiler_compile(hsmctemp,dataStr,0,&nulld);
        free(dataStr);
        return HalSMVariable_init(hsmc->memory,HalSMModule_init(hsmc->memory,getNameFile(name),hsmctemp),HalSMVariableType_HalSMModule);
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

int main(int argc,char *argv[]) {
    atexit(fnExit);
    HalStringFormat_init((void*(*)(unsigned long long int))malloc);
    SetConsoleOutputCP(CP_UTF8);
    HalSMMemoryManagmentLibrary hmml={HML_malloc,HML_calloc,HML_realloc,HML_free};
    
    memory=(HalSMMemory*)malloc(sizeof(HalSMMemory));
    HalSMMemory_init(memory,&hmml);

    HalSMSystemLibrary hsysl={HSYSL_exit};
    HalSMFileSystemLibrary hfsl={HFSL_isExistsDir,HFSL_isExistsFile,HFSL_getListFiles,HFSL_openFile,HFSL_closeFile,HFSL_read,HFSL_write,HFSL_puts,HFSL_seek,HFSL_tell,HFSL_rewind,HFSL_flush,HFSL_eof};
    HalSMCompiler* hsmc=HalSMCompiler_init(U8H(""),HSM_print,HSM_printError,HSM_input,HSM_getVariable,memory,&hsysl,&hfsl,HSMLoadModule);
    dllModules=HalSMArray_init(hsmc->memory);
    if (argc==1) {
        while (1) {
            char com[32765]={0};
            memset(com,0,32765);
            printf("==> ");
            fgets(com,32764,stdin);
            if (strcmp(com,"exit\n")==0||strcmp(com,"exit")==0||strcmp(com,"\0")==0){break;}
            HalSMCompiler_compile(hsmc,U8H(com),1,&nulld);
        }
        goto Return_Error;
    }
    unsigned char isActivated[9]={0};
    HalStringFormatChar* outFile=NULL,*diskname=NULL,*s,*mainFile,*compileType;
    HalSMArray* files=HalSMArray_init(memory),*modules=HalSMArray_init(memory);
    HalSMVariable* tempStr;
    unsigned long long int i,l,maxsizememory=0,maxcountfiles=0,ignoresectors=0,startindex=0,sizebuffer=0;
    for (int a=1;a<argc;a++) {
        s=U8H(argv[a]);
        if (HSFCompare(s,U8H("-v"))) {
            isActivated[0]=1;
        } else if (HSFCompare(s,U8H("-c"))||HSFCompare(s,U8H("--compile-type"))) {
            if (a<argc-1) {
                compileType=U8H(argv[++a]);
                isActivated[8]=1;
            }
        } else if (HSFCompare(s,U8H("-o"))) {
            if (a<argc-1) {
                outFile=U8H(argv[++a]);
            }
        } else if (HSFCompare(s,U8H("-diskname"))) {
            if (a<argc-1) {
                diskname=U8H(argv[++a]);
            }
        } else if (HSFCompare(s,U8H("-maxsizememory"))) {
            if (a<argc-1) {
                maxsizememory=strtoll(argv[++a],(char**)NULL,10);
                isActivated[1]=1;
            }
        } else if (HSFCompare(s,U8H("-maxcountfiles"))) {
            if (a<argc-1) {
                maxcountfiles=strtoll(argv[++a],(char**)NULL,10);
                isActivated[2]=1;
            }
        } else if (HSFCompare(s,U8H("-ignoresectors"))) {
            if (a<argc-1) {
                ignoresectors=strtoll(argv[++a],(char**)NULL,10);
                isActivated[3]=1;
            }
        } else if (HSFCompare(s,U8H("-startindex"))) {
            if (a<argc-1) {
                startindex=strtoll(argv[++a],(char**)NULL,10);
                isActivated[4]=1;
            }
        } else if (HSFCompare(s,U8H("-help"))) {
            isActivated[5]=1;
        } else if (HSFCompare(s,U8H("-main-file"))) {
            if (a<argc-1) {
                mainFile=U8H(argv[++a]);
                isActivated[6]=1;
            }
        } else if (HSFCompare(s,U8H("-sizebuffer"))) {
            if (a<argc-1) {
                startindex=strtoll(argv[++a],(char**)NULL,10);
                isActivated[7]=1;
            }
        } else if (HSFCompare(s,U8H("-modules"))) {
            if (a<argc-1) {
                modules=HalSMArray_split_str(memory,U8H(argv[++a]),U8H(","));
            }
        } else {
            HalSMArray_add(memory,files,HalSMVariable_init_str(memory,s));
        }
    }

    if (files->size==0) {
        if (isActivated[0]) {
            printf("HalSMCompiler v0.0.1\nHalSM %s (%lld)\n",HU8(hsmc->versionName),hsmc->version);
            goto Return_Error;
        } else if (isActivated[5]) {
            printf("-v: version\n-hsme: HalSMExecutable\n-win32: Windows x32\n-win64: Windows x64\n-linux: linux (64 bit)\n-android: Android Application (.apk)\n-web: Javascript Application (.js)\n-o: output file\n-diskname: name of disk HalFileSystem\n");
            goto Return_Error;
        }
    }

    if (isActivated[7]==0) {
        sizebuffer=4096;
    }

    if (files->size>0&&isActivated[8]) {
        if (isActivated[6]) {
            unsigned char isFind=0;
            for (i=0;i<files->size;i++) {
                HalStringFormatChar* temp=getNameFile(*(HalStringFormatChar**)files->arr[i]->value);
                if (HSFCompare(mainFile,temp)) {
                    isFind=1;
                    break;
                }
            }
            if (isFind==0){printf("Error: wrong main file name\n");goto Return_Error;}
        } else {
            mainFile=*(HalStringFormatChar**)files->arr[0]->value;
        }

        char hsmcPathStr[MAX_PATH+1]={0};
        GetModuleFileNameA(NULL,hsmcPathStr,MAX_PATH);
        HalStringFormatChar* hsmcPath=U8H(hsmcPathStr);
        long long int indexStr=StringLastIndexOf(hsmc->memory,hsmcPath,U8H("\\"));
        if (indexStr>-1){hsmcPath[indexStr+1]=ASCIIH('\0');}

        if (HSFCompare(compileType,U8H("hsme"))) {
            if (diskname==NULL) {
                diskname=U8H("HalSMExecutable v0.0.1 (HalSMCompiler v0.0.1)");
            }
            unsigned long long int detMem=0;
            FILE* fhsmmain;
            for (i=0;i<files->size;i++) {
                fhsmmain=fopen(HU8(*(HalStringFormatChar**)files->arr[i]->value),"rb");
                if (fhsmmain==NULL){printf("Error: cannot open HalSM file %s\n",HU8(*(HalStringFormatChar**)files->arr[i]->value));goto Return_Error;}
                fseek(fhsmmain,0,SEEK_END);
                detMem+=ftell(fhsmmain);
                fclose(fhsmmain);
            }

            HalSMArray* modulesDirectories=HalSMArray_init(hsmc->memory);
            Dict* modulesFiles=DictInit(hsmc->memory);
            char exePathStr[MAX_PATH+1]={0};
            GetModuleFileNameA(NULL,exePathStr,MAX_PATH);
            HalStringFormatChar* exePath=U8H(exePathStr),*modulePathStart,*modulePath,*moduleHCFG=U8H("\\module.hcfg"),*moduleDirectory=U8H("module"),*moduleDirectoryHFS=U8H("module/"),*moduleDirectoryPath,*moduleHCFGHFS=U8H("module.hcfg");
            long long int indexStr=StringLastIndexOf(hsmc->memory,exePath,U8H("\\"));
            if (indexStr>-1){exePath[indexStr+1]=ASCIIH('\0');}
            modulePathStart=ConcatenateStrings(hsmc->memory,exePath,U8H("module\\"));
            FILE* fconfig,*fmodule;
            unsigned char* configData;
            HalStringFormatChar* configDataStr;
            unsigned long long int j;

            if (isActivated[2]==0) {
                //file config - HalSMExecutable.hcfg
                //3 directories: main, module, files
                //Total: 4
                maxcountfiles=4+files->size;
            }

            for (i=0;i<modules->size;i++) {
                modulePath=ConcatenateStrings(hsmc->memory,modulePathStart,*(HalStringFormatChar**)modules->arr[i]->value);
                fconfig=fopen(HU8(ConcatenateStrings(hsmc->memory,modulePath,moduleHCFG)),"rb");
                if (fconfig==NULL){printf("Error: cannot open Module config file %s\n",HU8(*(HalStringFormatChar**)modules->arr[i]->value));goto Return_Error;}
                fseek(fconfig,0L,SEEK_END);
                l=ftell(fconfig);
                detMem+=l;
                fseek(fconfig,0L,SEEK_SET);
                if (l<3){continue;}
                moduleDirectoryPath=ConcatenateStrings(hsmc->memory,moduleDirectoryHFS,*(HalStringFormatChar**)modules->arr[i]->value);
                configData=calloc(l+1,sizeof(char));
                fread(configData,1,l,fconfig);
                fclose(fconfig);
                configDataStr=U8H(configData);
                free(configData);
                if (HSFCharCompare(configDataStr[0],ASCIIH('{'))==0||HSFCharCompare(configDataStr[HalStringFormat_length(configDataStr)-1],ASCIIH('}'))==0||HalStringFormat_length(configDataStr)<3) {
                    free(configDataStr);
                    continue;
                }
                string_cpy(configDataStr,&configDataStr[1]);
                configDataStr[HalStringFormat_length(configDataStr)-1]=ASCIIH('\0');
                Dict* config=HalSMCompiler_getArgsDict(hsmc,configDataStr,&nullhsmcwdict);
                free(configDataStr);

                HalSMVariable* tempKey=HalSMVariable_init_str(hsmc->memory,U8H("platforms"));
                if (DictElementIndexByKey(hsmc,config,tempKey)==-1){continue;}
                HalSMVariable* tempValue=DictElementFindByKey(hsmc,config,tempKey)->value;
                if (tempValue->type!=HalSMVariableType_HalSMDict){continue;}
                Dict* platforms=(Dict*)tempValue->value;
                HalSMArray_add(hsmc->memory,modulesDirectories,HalSMVariable_init_str(hsmc->memory,*(HalStringFormatChar**)modules->arr[i]->value));
                PutDictElementToDict(hsmc,modulesFiles,DictElementInit(hsmc->memory,HalSMVariable_init_str(hsmc->memory,ConcatenateStrings(hsmc->memory,modulePath,moduleHCFG)),HalSMVariable_init(hsmc->memory,DictElementInit(hsmc->memory,HalSMVariable_init_str(hsmc->memory,moduleDirectoryPath),HalSMVariable_init_str(hsmc->memory,moduleHCFGHFS)),HalSMVariableType_void)));
                maxcountfiles+=2;
                for (j=0;j<platforms->size;j++) {
                    tempValue=platforms->elements[j]->value;
                    if (tempValue->type!=HalSMVariableType_HalSMDict){continue;}
                    Dict* platform=(Dict*)tempValue->value;
                    tempKey=HalSMVariable_init_str(hsmc->memory,U8H("module"));
                    if (DictElementIndexByKey(hsmc,platform,tempKey)==-1){continue;}
                    tempValue=DictElementFindByKey(hsmc,platform,tempKey)->value;
                    if (tempValue->type!=HalSMVariableType_str){continue;}
                    HalStringFormatChar* platformModulePath=*(HalStringFormatChar**)tempValue->value;
                    if (string_len(platformModulePath)==0){continue;}
                    HalStringFormatChar* platformPath=ConcatenateStrings(hsmc->memory,modulePath,ConcatenateStrings(hsmc->memory,U8H("\\"),platformModulePath));
                    fmodule=fopen(HU8(platformPath),"rb");
                    fseek(fmodule,0L,SEEK_END);
                    detMem+=ftell(fmodule);
                    fclose(fmodule);
                    PutDictElementToDict(hsmc,modulesFiles,DictElementInit(hsmc->memory,HalSMVariable_init_str(hsmc->memory,platformPath),HalSMVariable_init(hsmc->memory,DictElementInit(hsmc->memory,HalSMVariable_init_str(hsmc->memory,moduleDirectoryPath),HalSMVariable_init_str(hsmc->memory,platformModulePath)),HalSMVariableType_void)));
                    maxcountfiles++;
                }
            }
            free(exePath);

            if (isActivated[1]) {
                if (maxsizememory<detMem){printf("Error: not enough max size memory, detected size memory: %lld\n",detMem);goto Return_Error;}
            } else {
                maxsizememory=detMem;
            }
            if (isActivated[3]==0) {
                ignoresectors=0;
            }
            if (isActivated[4]==0) {
                startindex=0;
            }
            if (outFile==NULL) {
                outFile=U8H("h.hsme");
            }
            FILE* fhsme=fopen(HU8(outFile),"w+b");
            if (fhsme==NULL){printf("Error: cannot open file\n");goto Return_Error;}
            HalFileSystem hfs;
            unsigned long long int d,b,tim;
            tim=(unsigned long long int)time(NULL);
            Dict* hsmecfg=DictInit(hsmc->memory);
            PutDictElementToDict(hsmc,hsmecfg,DictElementInit(hsmc->memory,HalSMVariable_init_str(hsmc->memory,U8H("mainFile")),HalSMVariable_init_str(hsmc->memory,mainFile)));
            HalStringFormatChar* hsmecfgstr=Dict_to_print(hsmc,hsmecfg);
            l=HalStringFormat_length(hsmecfgstr)*sizeof(HalStringFormatChar);
            maxsizememory+=l;
            HalFileSystem_init(&hfs,HFS_read,HFS_write,HFSL_seek,HFSL_tell,HFSL_eof,HFSL_flush,HML_malloc,HML_calloc,HML_realloc,HML_free);
            HalFileSystem_create(&hfs,fhsme,diskname,maxsizememory,maxcountfiles,ignoresectors,startindex);
            HalFileSystem_addDirectory(&hfs,U8H(""),U8H("main"),0,tim);
            HalFileSystem_addDirectory(&hfs,U8H(""),U8H("module"),0,tim);
            HalFileSystem_addDirectory(&hfs,U8H(""),U8H("files"),0,tim);
            HalStringFormatChar* pathFile,*nameFile;


            //Config HalSMExecutable.hcfg
            HalFileSystem_addFile(&hfs,U8H(""),U8H("HalSMExecutable.hcfg"),0,tim);
            pathFile=U8H("HalSMExecutable.hcfg");
            HalFileSystem_allocateMemory(&hfs,pathFile,l,&d);

            d=l/sizebuffer;
            for (b=0;b<d;b++) {
                HalFileSystem_writeFileWithoutAllocate(&hfs,pathFile,(unsigned char*)hsmecfgstr,b*sizebuffer,sizebuffer,tim);
                hsmecfgstr+=sizebuffer;
            }
            if (l%sizebuffer) {
                HalFileSystem_writeFileWithoutAllocate(&hfs,pathFile,(unsigned char*)hsmecfgstr,b*sizebuffer,l%sizebuffer,tim);
            }
            //Config HalSMExecutable.hcfg;

            //build\HalSMCompiler.exe -c hsme testhalchatplugin.hsm -o testhalchatplugin.hsme
            unsigned char* tempData=malloc(sizebuffer);
            for (i=0;i<files->size;i++) {
                fhsmmain=fopen(HU8(*(HalStringFormatChar**)files->arr[i]->value),"rb");
                if (fhsmmain==NULL){printf("Error: cannot open HalSM file %s\n",HU8(*(HalStringFormatChar**)files->arr[i]->value));goto Return_Error;}
                nameFile=getNameFile(*(HalStringFormatChar**)files->arr[i]->value);
                HalFileSystem_addFile(&hfs,U8H("main"),nameFile,0,tim);
                pathFile=malloc(6*sizeof(HalStringFormatChar)+HalStringFormat_length(nameFile)*sizeof(HalStringFormatChar));
                pathFile[0]=ASCIIH('m');
                pathFile[1]=ASCIIH('a');
                pathFile[2]=ASCIIH('i');
                pathFile[3]=ASCIIH('n');
                pathFile[4]=ASCIIH('/');
                pathFile[5+HalStringFormat_length(nameFile)]=ASCIIH('\0');
                memcpy(&pathFile[5],nameFile,HalStringFormat_length(nameFile)*sizeof(HalStringFormatChar));
                fseek(fhsmmain,0L,SEEK_END);
                l=ftell(fhsmmain);
                fseek(fhsmmain,0L,SEEK_SET);
                d=l/sizebuffer;
                if (l>0) {HalFileSystem_allocateMemory(&hfs,pathFile,l,&b);}

                for (b=0;b<d;b++) {
                    fread(tempData,sizebuffer,1,fhsmmain);
                    HalFileSystem_writeFileWithoutAllocate(&hfs,pathFile,tempData,b*sizebuffer,sizebuffer,tim);
                }
                if (l%sizebuffer) {
                    fread(tempData,l%sizebuffer,1,fhsmmain);
                    HalFileSystem_writeFileWithoutAllocate(&hfs,pathFile,tempData,b*sizebuffer,l%sizebuffer,tim);
                }
                fclose(fhsmmain);
            }

            for (i=0;i<modulesDirectories->size;i++) {
                HalFileSystem_addDirectory(&hfs,moduleDirectory,*(HalStringFormatChar**)modulesDirectories->arr[i]->value,0,tim);
            }

            for (i=0;i<modulesFiles->size;i++) {
                HalStringFormatChar* pathCopyFile,*pathHFSFile,*nameHFSFile;
                pathCopyFile=*(HalStringFormatChar**)modulesFiles->elements[i]->key->value;
                pathHFSFile=*(HalStringFormatChar**)((DictElement*)modulesFiles->elements[i]->value->value)->key->value;
                nameHFSFile=*(HalStringFormatChar**)((DictElement*)modulesFiles->elements[i]->value->value)->value->value;
                fhsmmain=fopen(HU8(pathCopyFile),"rb");
                if (fhsmmain==NULL){printf("Error: cannot open HalSM file %s\n",HU8(nameHFSFile));goto Return_Error;}
                HalFileSystem_addFile(&hfs,pathHFSFile,nameHFSFile,0,tim);
                fseek(fhsmmain,0L,SEEK_END);
                l=ftell(fhsmmain);
                fseek(fhsmmain,0L,SEEK_SET);
                d=l/sizebuffer;
                pathFile=ConcatenateStrings(hsmc->memory,pathHFSFile,ConcatenateStrings(hsmc->memory,U8H("/"),nameHFSFile));
                if (l>0) {HalFileSystem_allocateMemory(&hfs,pathFile,l,&b);}
                for (b=0;b<d;b++) {
                    fread(tempData,sizebuffer,1,fhsmmain);
                    HalFileSystem_writeFileWithoutAllocate(&hfs,pathFile,tempData,b*sizebuffer,sizebuffer,tim);
                }
                if (l%sizebuffer) {
                    fread(tempData,l%sizebuffer,1,fhsmmain);
                    HalFileSystem_writeFileWithoutAllocate(&hfs,pathFile,tempData,b*sizebuffer,l%sizebuffer,tim);
                }
                fclose(fhsmmain);
            }
            HalFileSystem_close(&hfs);
        } else {
            HalStringFormatChar* platformPath=ConcatenateStrings(hsmc->memory,hsmcPath,ConcatenateStrings(hsmc->memory,U8H("platform\\"),compileType));
            FILE* fconfig=fopen(HU8(ConcatenateStrings(hsmc->memory,platformPath,U8H("\\config.hcfg"))),"rb"); 
            if (fconfig==NULL){printf("Error: compile type not found\n");goto Return_Error;}
            fseek(fconfig,0L,SEEK_END);
            l=ftell(fconfig);
            fseek(fconfig,0L,SEEK_SET);
            unsigned char* temp=calloc(l+1,sizeof(char));
            fread(temp,1,l,fconfig);
            fclose(fconfig);
            HalStringFormatChar* configstr=U8H(temp);
            free(temp);
            if (HSFCharCompare(configstr[0],ASCIIH('{'))==0||HSFCharCompare(configstr[HalStringFormat_length(configstr)-1],ASCIIH('}'))==0||HalStringFormat_length(configstr)<3){printf("Error: not wrong config of compile type");goto Return_Error;}
            string_cpy(configstr,&configstr[1]);
            configstr[HalStringFormat_length(configstr)-1]=ASCIIH('\0');
            Dict* config=HalSMCompiler_getArgsDict(hsmc,configstr,DictInit(hsmc->memory));
            free(configstr);
            tempStr=HalSMVariable_init_str(hsmc->memory,U8H("win_command"));
            if (DictElementIndexByKey(hsmc,config,tempStr)==-1||(tempStr=DictElementFindByKey(hsmc,config,tempStr)->value)->type!=HalSMVariableType_str){printf("Error: command not found in compile type config\n");goto Return_Error;}
            
            FILE* fhsme=fopen(HU8(mainFile),"rb");
            FILE* fouthsme=fopen(HU8(ConcatenateStrings(hsmc->memory,platformPath,U8H("\\h.hsme"))),"wb");
            if (fhsme==NULL||fouthsme==NULL){printf("Error: cannot copy HalSMExecutable to platform directory\n");goto Return_Error;}
            fseek(fhsme,0L,SEEK_END);
            unsigned long long int size=ftell(fhsme);
            fseek(fhsme,0L,SEEK_SET);
            l=size/sizebuffer;
            temp=malloc(sizebuffer);
            fwrite(&size,8,1,fouthsme);
            for (i=0;i<l;i++) {
                fread(temp,sizebuffer,1,fhsme);
                fwrite(temp,sizebuffer,1,fouthsme);
            }
            if ((size%sizebuffer)>0) {
                fread(temp,size%sizebuffer,1,fhsme);
                fwrite(temp,size%sizebuffer,1,fouthsme);
            }
            free(temp);
            fclose(fhsme);
            fclose(fouthsme);

            GetCurrentDirectoryA(MAX_PATH,hsmcPathStr);
            HalStringFormatChar* curPath=U8H(hsmcPathStr);

            if (SetCurrentDirectoryA(HU8(platformPath))==0){printf("Error: set current directory unsuccessfully\n");goto Return_Error;}

            system(HU8(*(HalStringFormatChar**)tempStr->value));
            DeleteFileA("h.hsme");

            if (SetCurrentDirectoryA(HU8(curPath))==0){printf("Error: set current directory unsuccessfully\n");goto Return_Error;}

            tempStr=HalSMVariable_init_str(hsmc->memory,U8H("out"));
            if (DictElementIndexByKey(hsmc,config,tempStr)==-1||(tempStr=DictElementFindByKey(hsmc,config,tempStr)->value)->type!=HalSMVariableType_str){printf("Error: command not found in compile type config\n");goto Return_Error;}
            if (fhsme==NULL||fouthsme==NULL){printf("Error: cannot open out files\n");goto Return_Error;}
            HalSMArray* spl=HalSMArray_split_str(hsmc->memory,*(HalStringFormatChar**)tempStr->value,U8H("\\"));
            CopyFileA(HU8(ConcatenateStrings(hsmc->memory,ConcatenateStrings(hsmc->memory,platformPath,U8H("\\")),*(HalStringFormatChar**)tempStr->value)),HU8(outFile==NULL?*(HalStringFormatChar**)spl->arr[spl->size-1]->value:outFile),0);
            //DeleteFileA(HU8(ConcatenateStrings(hsmc->memory,ConcatenateStrings(hsmc->memory,platformPath,U8H("\\")),*(HalStringFormatChar**)tempStr->value)));
            DictFree(hsmc->memory,config);
            HalSMArray_free(hsmc->memory,spl);
            printf("Program succefully compiled\n");
        }
        goto Return_Error;
    }

    if (files->size==1) {
        HalStringFormatChar* nameFile=getNameFile(*(HalStringFormatChar**)files->arr[0]->value);
        FILE* fhsm=fopen(HU8(*(HalStringFormatChar**)files->arr[0]->value),"rb");
        if (fhsm==NULL) {
            printf("Error: file %s not found\n",HU8(*(HalStringFormatChar**)files->arr[0]->value));
            goto Return_Error;
        }
        HalFileSystem hfs;
        HalFileSystem_init(&hfs,HFS_read,HFS_write,HFSL_seek,HFSL_tell,HFSL_eof,HFSL_flush,HML_malloc,HML_calloc,HML_realloc,HML_free);
        if (StringEndsWith(hsmc->memory,nameFile,U8H(".hsme"))&&HalFileSystem_open(&hfs,fhsm,0)==0) {
            HalFileSystem_open(&hfs,fhsm,0);
            HalSMExecutable hsme;
            HalSMExecutable_init(&hsme,&hfs);
            if (HalSMExecutable_load(&hsme,hsmc)){printf("Error: load HalSMExecutable unsuccessfully\n");goto Return_Error;}

            char tempPath[MAX_PATH+1]={0};
            char tempFilePath[MAX_PATH+1]={0};
            tempPathFile=calloc(MAX_PATH+1,sizeof(char));
            if (GetTempPathA(MAX_PATH,tempPath)==0){printf("Error: get temp path failed\n");goto Return_Error;}
            sprintf(tempPathFile,"%shsmc%lld",tempPath,time(NULL));
            if (CreateDirectoryA(tempPathFile,NULL)==0&&ERROR_ALREADY_EXISTS!=GetLastError()){printf("Error: create temp directory failed %x\n",GetLastError());goto Return_Error;}

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
                        sprintf(tempFilePath,"%s\\%s",tempPathFile,HU8(HalFileSystem_getDirectoryName(&hfs,&dir)));
                        if (CreateDirectoryA(tempFilePath,NULL)==0&&ERROR_ALREADY_EXISTS!=GetLastError()){continue;}
                        if (HalFileSystem_getCountFilesDirectoryByIndex(&hfs,modulesDirectories[i],&modulesFilesCount,1)>0||modulesFilesCount==0){continue;}
                        modulesFiles=calloc(modulesFilesCount,sizeof(unsigned long long int));
                        if (HalFileSystem_getListFilesDirectoryByIndex(&hfs,modulesDirectories[i],modulesFiles,&modulesFilesCount,modulesFilesCount,1)>0){continue;}
                        for (unsigned long long int j=0;j<modulesFilesCount;j++) {
                            if (HalFileSystem_getFileByIndex(&hfs,modulesFiles[j],&file)>0||file.size==0){continue;}
                            memset(tempFilePath,'\0',MAX_PATH+1);
                            sprintf(tempFilePath,"%s\\%s\\%s",tempPathFile,HU8(HalFileSystem_getDirectoryName(&hfs,&dir)),HU8(HalFileSystem_getFileName(&hfs,&file)));
                            FILE* moduleFile=fopen(tempFilePath,"wb");
                            if (moduleFile==NULL){continue;}
                            l=file.size/sizebuffer;
                            fseek(hfs.disk,hfs.startIndexFiles+hfs.maxcountfiles*HalFileSystemFileDirectorySize+file.startAddress,SEEK_SET);
                            for (unsigned long long int k=0;k<l;k++) {
                                fread(temp,sizebuffer,1,hfs.disk);
                                fwrite(temp,sizebuffer,1,moduleFile);
                            }
                            if (file.size%sizebuffer) {
                                fread(temp,file.size%sizebuffer,1,hfs.disk);
                                fwrite(temp,file.size%sizebuffer,1,moduleFile);
                            }
                            fclose(moduleFile);
                        }
                    }
                }
            }

            HalSMExecutable_run(&hsme,hsmc);
            HalFileSystem_close(&hfs);
            fclose(fhsm);
            goto Return_Error;
        }
        fseek(fhsm, 0L, SEEK_END);
        long fsize=ftell(fhsm);
        fseek(fhsm, 0L, SEEK_SET);
        char* temp=malloc(fsize+1);
        fread(temp,1,fsize,fhsm);
        temp[fsize]='\0';
        fclose(fhsm);
        HalSMCompiler_compile(hsmc,U8H(temp),0,&nulld);
        free(temp);
        printf("Нажмите Enter чтобы закрыть программу: ");
        while(1) {
            if (fgetc(stdin)=='\n'){break;}
        }
        goto Return_Error;
    }
    printf("Error: unknown parameters\n");
Return_Error:
    if (tempPathFile!=NULL) {
        //RemoveDirectoryA(tempPathFile);
    }
    HalSMCompiler_exitApp(hsmc);
    return 0;
}
#include <HalSM.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
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
    HalSM* hsm=HalSM_init(
        HalSMArray_init(&memorymanagmentlibrary),
        print,
        printError,
        input,
        readFile,
        "",
        &loadsharedlibrary,
        &stringlibrary,
        &memorymanagmentlibrary,
        &systemlibrary
    );
    if (argc==1) {
        printf("HalSMCompiler 0.0.1 (HalSM ");
        printf(hsm->version);
        printf(")\n");
        printf("Enter command \"help\", \"copyright\" for get information\n");
        HalSMCompiler* hsmc=HalSMCompiler_init("",hsm->externModules,hsm->print,hsm->printErrorf,hsm->inputf,hsm->readFilef,hsm->pathModules,hsm->loadsharedlibrary,hsm->stringlibrary,hsm->memorymanagmentlibrary,hsm->systemlibrary);
        HalSMArray* outhsmc;
        while (1) {
            char command[100];
            printf("--> ");
            fgets(command,100,stdin);
            if (strlen(command)>0) {
                command[strlen(command)-1]='\0';
                outhsmc=HalSMCompiler_compile(hsmc,command,1);
                hsmc->variables=(Dict*)outhsmc->arr[0]->value;
                hsmc->localFunctions=(Dict*)outhsmc->arr[1]->value;
                hsmc->classes=(Dict*)outhsmc->arr[2]->value;
                hsmc->modules=(Dict*)outhsmc->arr[3]->value;
            }
        }
    } else if (argc==2) {
        char* arg=argv[1];
        if (arg[0]=='-'&&StringIndexOf(&memorymanagmentlibrary,arg,".")==-1) {
            if (strcmp(arg,"-help")==0) {
                printf("HalSMCompiler.exe {file or arguments}\n--build-windows platform windows\n-o output file path\n-c run script in command line from arguments, Example: HalSMCompiler.exe -c \"print(3+2)\"\n");
            }
        } else {
            if (access(arg,F_OK)==0) {
                HalSMCompiler* hsmc=HalSMCompiler_init("",hsm->externModules,hsm->print,hsm->printErrorf,hsm->inputf,hsm->readFilef,hsm->pathModules,hsm->loadsharedlibrary,hsm->stringlibrary,hsm->memorymanagmentlibrary,hsm->systemlibrary);
                HalSMCompiler_compile(hsmc,*(char**)HalSMCompiler_readFile(hsmc,HalSMArray_init_with_elements(&memorymanagmentlibrary,(HalSMVariable*[]){HalSMVariable_init_str(&memorymanagmentlibrary,arg)},1))->value,0);
            } else {
                printf("File %s not exists\n",arg);
            }
        }
    } else {
        char* argsWithVal[][2]={
            {"-c",NULL},
            {"-o",NULL},
            NULL
        };

        char* argsWithoutVal[][2]={
            {"--build-windows",NULL},
            {"--build-linux",NULL},
            {"--build-android",NULL},
            NULL
        };
        
        char* file=NULL;
        char* arg;
        unsigned int indexArgs=0;
        unsigned int lindex=argc-1;
        unsigned char isFind;
        unsigned int i=0;

        while (i<argc) 
        {
            arg=argv[i];
            indexArgs=0;
            isFind=0;
            while (argsWithVal[indexArgs][0]!=NULL) {
                if (strcmp(arg,argsWithVal[indexArgs][0])==0) {
                    if (i==lindex) {
                        printf("Argument %s cannot be without value\n",argsWithVal[indexArgs][0]);
                        exit(0);
                    }
                    i++;
                    argsWithVal[indexArgs][1]=argv[i];
                    isFind=1;
                    break;
                }
                indexArgs++;
            }
            indexArgs=0;
            while (argsWithoutVal[indexArgs][0]!=NULL) {
                if (strcmp(arg,argsWithoutVal[indexArgs][0])==0) {
                    argsWithoutVal[indexArgs][1]="1";
                    isFind=1;
                    break;
                }
                indexArgs++;
            }

            if (isFind==0) {file=arg;}
            i++;
        }

        if (argsWithVal[0][1]!=NULL) {
            HalSMCompiler* hsmc=HalSMCompiler_init("",hsm->externModules,hsm->print,hsm->printErrorf,hsm->inputf,hsm->readFilef,hsm->pathModules,hsm->loadsharedlibrary,hsm->stringlibrary,hsm->memorymanagmentlibrary,hsm->systemlibrary);
            HalSMCompiler_compile(hsmc,argsWithVal[0][1],0);
        } else if (argsWithVal[1][1]!=NULL) {
            if (file==NULL) {
                printf("You didn't specify a file for build\n");
            } else if (argsWithoutVal[0][1]!=NULL) {
                char* outFile=argsWithVal[1][1];
                if (outFile==NULL) {
                    HalSMArray* spliti=HalSMArray_split_str(&memorymanagmentlibrary,file,".");
                    if (spliti->size==0) {
                        outFile=ConcatenateStrings(&memorymanagmentlibrary,file,".exe");
                    } else {
                        outFile=ConcatenateStrings(&memorymanagmentlibrary,HalSMArray_join_str(&memorymanagmentlibrary,HalSMArray_slice(&memorymanagmentlibrary,spliti,0,spliti->size-1),"."),".exe");
                    }
                    outFile=ConcatenateStrings(&memorymanagmentlibrary,"build\\",outFile);
                }
                
                FILE* hcwc=fopen("windows-build\\HalSMCompilerWindows.h","w");
                char* fw=ConcatenateStrings(&memorymanagmentlibrary,"unsigned char* halsmfile=\"",ConcatenateStrings(&memorymanagmentlibrary,StringReplace(&memorymanagmentlibrary,readFile(file),"\n","\\n"),"\";"));
                fwrite(fw,1,strlen(fw),hcwc);
                fclose(hcwc);
                char pBuf[1024];
                GetModuleFileName(NULL,pBuf,1024);
                HalSMArray* pathBuf=HalSMArray_split_str(&memorymanagmentlibrary,pBuf,"\\");
                char* cmd=ConcatenateStrings(&memorymanagmentlibrary,ConcatenateStrings(&memorymanagmentlibrary,"gcc -o ",outFile),ConcatenateStrings(&memorymanagmentlibrary," ",ConcatenateStrings(&memorymanagmentlibrary,HalSMArray_join_str(&memorymanagmentlibrary,HalSMArray_slice(&memorymanagmentlibrary,pathBuf,0,pathBuf->size-2),"\\"),"\\windows-build\\HalSMCompilerWindows.c -IHalSM -LHalSM -l:LibHalSM.a")));
                system(cmd);
            } else {
                printf("You didn't choose platform for build\n");
                exit(0);
            }
        }
    }
}
#include <stdio.h>
#include <Windows.h>

DWORD align(DWORD size,DWORD align,DWORD addr) {
    if ((size%align)==0)return addr+size;
    return addr+(size/align+1)*align;
}

/*unsigned char UpdateSection(char* filepath,char* sectionName) {
    HANDLE file=CreateFile(filepath,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    HANDLE hsmefile=CreateFile("h.hsme",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (file==INVALID_HANDLE_VALUE||hsmefile==INVALID_HANDLE_VALUE){return 0;}
    DWORD fileSize=GetFileSize(file,NULL),bytes;
    DWORD hsmeFileSize=GetFileSize(hsmefile,NULL);
    BYTE* pByte=malloc(fileSize*sizeof(BYTE));
    ReadFile(file,pByte,fileSize,&bytes,0);
    PIMAGE_DOS_HEADER dos=(PIMAGE_DOS_HEADER)pByte;
    if (dos->e_magic!=IMAGE_DOS_SIGNATURE)return -1;
    PIMAGE_OPTIONAL_HEADER oh=(PIMAGE_OPTIONAL_HEADER)(pByte+dos->e_lfanew+sizeof(DWORD)+sizeof(IMAGE_FILE_HEADER));
    PIMAGE_NT_HEADERS nth=(PIMAGE_NT_HEADERS)(pByte+dos->e_lfanew);

    if (nth->Signature!=IMAGE_NT_SIGNATURE)return -2;
    PIMAGE_SECTION_HEADER sec=IMAGE_FIRST_SECTION(nth);
    while (memcmp(sec->Name,".hsme",5)){sec++;}
    DWORD vs=sec->Misc.VirtualSize;
    DWORD secsize=sec->SizeOfRawData;
    BYTE* secbuf=malloc(secsize);
    sec->Misc.VirtualSize=align(hsmeFileSize,oh->SectionAlignment,0);
    sec->VirtualAddress=align((sec-1)->Misc.VirtualSize,oh->SectionAlignment,(sec-1)->VirtualAddress);
    sec->SizeOfRawData=align(hsmeFileSize,oh->FileAlignment,0);
    sec->PointerToRawData=align((sec-1)->SizeOfRawData,oh->FileAlignment,(sec-1)->PointerToRawData);
    sec->Characteristics=IMAGE_SCN_MEM_WRITE|IMAGE_SCN_MEM_READ|IMAGE_SCN_CNT_INITIALIZED_DATA|IMAGE_SCN_CNT_UNINITIALIZED_DATA|IMAGE_SCN_MEM_PRELOAD;
    SetFilePointer(file,0,NULL,FILE_BEGIN);
    WriteFile(file,pByte,fileSize,&bytes,NULL);
    SetFilePointer(file,sec->PointerToRawData,NULL,FILE_BEGIN);
    BYTE* temp=malloc(4096);
    DWORD l=hsmeFileSize/4096,i;
    for (i=0;i<l;i++) {
        ReadFile(hsmefile,temp,4096,&bytes,0);
        WriteFile(file,temp,4096,&bytes,0);
    }
    if (hsmeFileSize%4096) {
        ReadFile(hsmefile,temp,hsmeFileSize%4096,&bytes,0);
        WriteFile(file,temp,hsmeFileSize%4096,&bytes,0);
    }
    CloseHandle(hsmefile);
    CloseHandle(file);
}*/

/*unsigned char UpdateSection(char *filepath, char *sectionName, DWORD sizeOfSection){
    HANDLE file = CreateFile(filepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) 
        return 0;
    DWORD fileSize = GetFileSize(file, NULL);
    //so we know how much buffer to allocate
    BYTE *pByte = malloc(fileSize);
    DWORD dw;
    //lets read the entire file,so we can use the PE information
    ReadFile(file, pByte, fileSize, &dw, NULL);
 
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pByte;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
        return 0; //invalid PE
    PIMAGE_FILE_HEADER FH = (PIMAGE_FILE_HEADER)(pByte + dos->e_lfanew + sizeof(DWORD));
    PIMAGE_OPTIONAL_HEADER OH = (PIMAGE_OPTIONAL_HEADER)(pByte + dos->e_lfanew + sizeof(DWORD)+sizeof(IMAGE_FILE_HEADER));
    PIMAGE_NT_HEADERS nth=(PIMAGE_NT_HEADERS)(pByte+dos->e_lfanew);
    //PIMAGE_SECTION_HEADER SH = (PIMAGE_SECTION_HEADER)(pByte + dos->e_lfanew + sizeof(IMAGE_NT_HEADERS));
    PIMAGE_SECTION_HEADER sec=IMAGE_FIRST_SECTION(nth);
    while (memcmp(sec->Name,".hsme",5)){sec++;}
    //We use 8 bytes for section name,cause it is the maximum allowed section name size
 
    //lets insert all the required information about our new PE section
    sec->Misc.VirtualSize = align(sizeOfSection, OH->SectionAlignment, 0);
    sec->VirtualAddress = align((sec-1)->Misc.VirtualSize, OH->SectionAlignment, (sec-1)->VirtualAddress);
    sec->SizeOfRawData = align(sizeOfSection, OH->FileAlignment, 0);
    sec->PointerToRawData = align((sec-1)->SizeOfRawData, OH->FileAlignment, (sec-1)->PointerToRawData);
    sec->Characteristics = IMAGE_SCN_MEM_WRITE|IMAGE_SCN_MEM_READ|IMAGE_SCN_CNT_INITIALIZED_DATA|IMAGE_SCN_CNT_UNINITIALIZED_DATA|IMAGE_SCN_MEM_PRELOAD;

    SetFilePointer(file, sec->PointerToRawData + sec->SizeOfRawData, NULL, FILE_BEGIN);
    //end the file right here,on the last section + it's own size
    SetEndOfFile(file);
    //now lets change the size of the image,to correspond to our modifications
    //by adding a new section,the image size is bigger now
    OH->SizeOfImage = sec->VirtualAddress + sec->Misc.VirtualSize;
    //and we added a new section,so we change the NOS too
    //FH->NumberOfSections += 1;
    SetFilePointer(file, 0, NULL, FILE_BEGIN);
    //and finaly,we add all the modifications to the file
    WriteFile(file, pByte, fileSize, &dw, NULL);
    CloseHandle(file);
    return 1;
}*/

unsigned char AddSection(char *filepath, char *sectionName, DWORD sizeOfSection,HANDLE hsmefile){
    HANDLE file = CreateFile(filepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE fileOut = CreateFile("h.exe", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) 
        return 0;
    DWORD fileSize = GetFileSize(file, NULL);
    //so we know how much buffer to allocate
    BYTE *pByte = (BYTE*)VirtualAlloc(NULL, fileSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);;
    DWORD dw;
    //lets read the entire file,so we can use the PE information
    ReadFile(file, pByte, fileSize, &dw, NULL);
 
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pByte;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
        return 0; //invalid PE
    PIMAGE_FILE_HEADER FH = (PIMAGE_FILE_HEADER)(pByte + dos->e_lfanew + sizeof(DWORD));
    PIMAGE_OPTIONAL_HEADER OH = (PIMAGE_OPTIONAL_HEADER)(pByte + dos->e_lfanew + sizeof(DWORD)+sizeof(IMAGE_FILE_HEADER));
    PIMAGE_SECTION_HEADER SH = (PIMAGE_SECTION_HEADER)(pByte + dos->e_lfanew + sizeof(IMAGE_NT_HEADERS));
 
    ZeroMemory(&SH[FH->NumberOfSections], sizeof(IMAGE_SECTION_HEADER));
    CopyMemory(&SH[FH->NumberOfSections].Name, sectionName, 8); 
    //We use 8 bytes for section name,cause it is the maximum allowed section name size
 
    //lets insert all the required information about our new PE section
    SH[FH->NumberOfSections].Misc.VirtualSize = align(sizeOfSection, OH->SectionAlignment, 0);
    SH[FH->NumberOfSections].VirtualAddress = align(SH[FH->NumberOfSections - 1].Misc.VirtualSize, OH->SectionAlignment, SH[FH->NumberOfSections - 1].VirtualAddress);
    SH[FH->NumberOfSections].SizeOfRawData = align(sizeOfSection, OH->FileAlignment, 0);
    SH[FH->NumberOfSections].PointerToRawData = align(SH[FH->NumberOfSections - 1].SizeOfRawData, OH->FileAlignment, SH[FH->NumberOfSections - 1].PointerToRawData);
    SH[FH->NumberOfSections].Characteristics = IMAGE_SCN_MEM_WRITE|IMAGE_SCN_MEM_READ|IMAGE_SCN_CNT_INITIALIZED_DATA|IMAGE_SCN_MEM_PRELOAD;
    //|IMAGE_SCN_CNT_UNINITIALIZED_DATA
    /*
        0xE00000E0 = IMAGE_SCN_MEM_WRITE |
                     IMAGE_SCN_CNT_CODE  |
                     IMAGE_SCN_CNT_UNINITIALIZED_DATA  |
                     IMAGE_SCN_MEM_EXECUTE |
                     IMAGE_SCN_CNT_INITIALIZED_DATA |
                     IMAGE_SCN_MEM_READ 
    */
    SetFilePointer(fileOut, SH[FH->NumberOfSections].PointerToRawData + SH[FH->NumberOfSections].SizeOfRawData, NULL, FILE_BEGIN);
    //end the file right here,on the last section + it's own size
    SetEndOfFile(fileOut);
    //now lets change the size of the image,to correspond to our modifications
    //by adding a new section,the image size is bigger now
    OH->SizeOfImage = SH[FH->NumberOfSections].VirtualAddress + SH[FH->NumberOfSections].Misc.VirtualSize;
    //and we added a new section,so we change the NOS too
    FH->NumberOfSections += 1;
    //SetFilePointer(file, 0, NULL, FILE_BEGIN);
    //and finaly,we add all the modifications to the file
    SetFilePointer(fileOut,0,NULL,FILE_BEGIN);
    WriteFile(fileOut, pByte, fileSize, &dw, NULL);
    CloseHandle(file);
    SetFilePointer(fileOut,SH[FH->NumberOfSections-1].PointerToRawData,0,FILE_BEGIN);
    BYTE* temp=malloc(4096);
    DWORD l=sizeOfSection/4096,i,bytes;
    for (i=0;i<l;i++) {
        ReadFile(hsmefile,temp,4096,&bytes,0);
        WriteFile(fileOut,temp,4096,&bytes,0);
    }
    if (sizeOfSection%4096) {
        ReadFile(hsmefile,temp,sizeOfSection%4096,&bytes,0);
        WriteFile(fileOut,temp,sizeOfSection%4096,&bytes,0);
    }
    CloseHandle(hsmefile);
    VirtualFree(pByte,fileSize,MEM_RELEASE);
    CloseHandle(fileOut);
    return 1;
}
 
unsigned char AddData(char *filepath,DWORD hsmeFileSize,HANDLE hsmefile) {
    HANDLE file = CreateFile(filepath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) 
        return 0;
    DWORD filesize = GetFileSize(file, NULL);
    BYTE *pByte = malloc(filesize);
    DWORD dw;
    ReadFile(file, pByte, filesize, &dw, NULL);
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pByte;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pByte + dos->e_lfanew);
 
    //since we added a new section,it must be the last section added,cause of the code inside
    //AddSection function,thus we must get to the last section to insert our secret data :)
    PIMAGE_SECTION_HEADER first = IMAGE_FIRST_SECTION(nt);
    PIMAGE_SECTION_HEADER last = first + (nt->FileHeader.NumberOfSections - 1);
 
    SetFilePointer(file, last->PointerToRawData, NULL, FILE_BEGIN);
    //char *str = "ATHENIAN WAS HERE";
    //WriteFile(file, str, strlen(str), &dw, 0);
    BYTE* temp=malloc(4096);
    DWORD l=hsmeFileSize/4096,i,bytes;
    for (i=0;i<l;i++) {
        ReadFile(hsmefile,temp,4096,&bytes,0);
        WriteFile(file,temp,4096,&bytes,0);
    }
    if (hsmeFileSize%4096) {
        ReadFile(hsmefile,temp,hsmeFileSize%4096,&bytes,0);
        WriteFile(file,temp,hsmeFileSize%4096,&bytes,0);
    }
    CloseHandle(hsmefile);
    CloseHandle(file);
    return 1;
}

int main(int argc,char* argv[])
{
    HANDLE hsmefile=CreateFile("h.hsme",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hsmefile==INVALID_HANDLE_VALUE) {
        return -1;
    }
    DWORD hsmeFileSize=GetFileSize(hsmefile,NULL);
    AddSection("HalSMExecutable.exe",".hsme",hsmeFileSize,hsmefile);
    //AddData("h.exe",hsmeFileSize,hsmefile);
    return 0;
}
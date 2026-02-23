#include <SocketWindows.h>
#include <stdio.h>

HalSMVoid hsmsvoid;
HalSMVariable hsvoid={&hsmsvoid,HalSMVariableType_void};

HalSMVariableTypeCustom* HalSMSocketType;
HalStringFormatChar* HalSMSocketName;
HalStringFormatChar* ByteArrayName;
HalSMVariable* ByteArrayNameVar;

HalSMCModule_entry* EXPORT Socket_init(HalSMCompiler* hsmc,HalSMArray* args)
{
    HalStringFormat_init((void*(*)(unsigned long long))malloc);
    HalSMSocketName=U8H("socket");
    ByteArrayName=U8H("ByteArray");
    ByteArrayNameVar=HalSMVariable_init_str(hsmc->memorymanagmentlibrary,ByteArrayName);
    HalSMCModule_entry* out=malloc(sizeof(HalSMCModule_entry));
    out->author=U8H("Halwarsing");
    out->description=U8H("Socket library");
    out->name=HalSMSocketName;
    out->version=U8H("0.0.1");
    out->classes=DictInit(hsmc->memorymanagmentlibrary);
    out->lfuncs=DictInit(hsmc->memorymanagmentlibrary);
    out->vrs=DictInit(hsmc->memorymanagmentlibrary);
    WSADATA wsaData={0};
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    HalSMSocketType=HalSMVariableTypeCustom_init(hsmc->memorymanagmentlibrary,(HalSMVariableTypeCustom){
        HalSMSocketName,
        HalSMSocket_toString,
        HalSMSocket_add,
        HalSMSocket_sub,
        HalSMSocket_mul,
        HalSMSocket_div,
        HalSMSocket_pow,
        HalSMSocket_modulo,
        HalSMSocket_increment,
        HalSMSocket_decrement,
        HalSMSocket_isEqual,
        HalSMSocket_isMore,
        HalSMSocket_isLess,
        HalSMSocket_getBool,
        HalSMSocket_indexVariableByName,
        HalSMSocket_indexFunctionByName,
        HalSMSocket_getVariableByName,
        HalSMSocket_getFunctionByName,
        HalSMSocket_getItem,
        HalSMSocket_shiftRight,
        HalSMSocket_shiftLeft,
        HalSMSocket_convert,
        HalSMSocket_xor,
        HalSMSocket_and,
        HalSMSocket_or,
        HalSMSocket_bnot,
        DictInit(hsmc->memorymanagmentlibrary),
        DictInit(hsmc->memorymanagmentlibrary)
    });

    PutDictElementToDict(hsmc,HalSMSocketType->funcs,DictElementInit(hsmc->memorymanagmentlibrary,HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("connect")),HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMFunctionC_init(hsmc,HalSMSocket_connect),HalSMVariableType_HalSMFunctionC)));
    PutDictElementToDict(hsmc,HalSMSocketType->funcs,DictElementInit(hsmc->memorymanagmentlibrary,HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("bind")),HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMFunctionC_init(hsmc,HalSMSocket_bind),HalSMVariableType_HalSMFunctionC)));
    PutDictElementToDict(hsmc,HalSMSocketType->funcs,DictElementInit(hsmc->memorymanagmentlibrary,HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("listen")),HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMFunctionC_init(hsmc,HalSMSocket_listen),HalSMVariableType_HalSMFunctionC)));
    PutDictElementToDict(hsmc,HalSMSocketType->funcs,DictElementInit(hsmc->memorymanagmentlibrary,HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("accept")),HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMFunctionC_init(hsmc,HalSMSocket_accept),HalSMVariableType_HalSMFunctionC)));
    PutDictElementToDict(hsmc,HalSMSocketType->funcs,DictElementInit(hsmc->memorymanagmentlibrary,HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("send")),HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMFunctionC_init(hsmc,HalSMSocket_send),HalSMVariableType_HalSMFunctionC)));
    PutDictElementToDict(hsmc,HalSMSocketType->funcs,DictElementInit(hsmc->memorymanagmentlibrary,HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("recv")),HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMFunctionC_init(hsmc,HalSMSocket_recv),HalSMVariableType_HalSMFunctionC)));
    PutDictElementToDict(hsmc,HalSMSocketType->funcs,DictElementInit(hsmc->memorymanagmentlibrary,HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("sendto")),HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMFunctionC_init(hsmc,HalSMSocket_sendto),HalSMVariableType_HalSMFunctionC)));
    PutDictElementToDict(hsmc,HalSMSocketType->funcs,DictElementInit(hsmc->memorymanagmentlibrary,HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("recvfrom")),HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMFunctionC_init(hsmc,HalSMSocket_recvfrom),HalSMVariableType_HalSMFunctionC)));
    PutDictElementToDict(hsmc,HalSMSocketType->funcs,DictElementInit(hsmc->memorymanagmentlibrary,HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("close")),HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMFunctionC_init(hsmc,HalSMSocket_close),HalSMVariableType_HalSMFunctionC)));
    PutDictElementToDict(hsmc,HalSMSocketType->funcs,DictElementInit(hsmc->memorymanagmentlibrary,HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("shutdown")),HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMFunctionC_init(hsmc,HalSMSocket_shutdown),HalSMVariableType_HalSMFunctionC)));
    
    PutDictElementToDict(hsmc,out->lfuncs,DictElementInit(hsmc->memorymanagmentlibrary,HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H("socket")),HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMFunctionC_init(hsmc,Socket_socket),HalSMVariableType_HalSMFunctionC)));
    
    return out;
}

HalStringFormatChar* IntegerToString(int a)
{
    char out[12]={0};
    itoa(a,out,10);
    return U8H(out);
}

//HalSMSocketType

HalStringFormatChar* HalSMSocket_toString(HalSMCompiler* hsmc,HalSMVariable* v)
{
    HalSMSocket* sock=(HalSMSocket*)((HalSMVariableCustom*)v->value)->value;
    return ConcatenateStrings(hsmc->memorymanagmentlibrary,U8H("<socket sock="),ConcatenateStrings(hsmc->memorymanagmentlibrary,IntegerToString(sock->sock),ConcatenateStrings(hsmc->memorymanagmentlibrary,U8H(", family="),ConcatenateStrings(hsmc->memorymanagmentlibrary,IntegerToString(sock->family),ConcatenateStrings(hsmc->memorymanagmentlibrary,U8H(", type="),ConcatenateStrings(hsmc->memorymanagmentlibrary,IntegerToString(sock->type),ConcatenateStrings(hsmc->memorymanagmentlibrary,U8H(", protocol="),ConcatenateStrings(hsmc->memorymanagmentlibrary,IntegerToString(sock->protocol),U8H(">")))))))));
}

HalSMVariable* HalSMSocket_add(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return a;}
HalSMVariable* HalSMSocket_sub(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return a;}
HalSMVariable* HalSMSocket_mul(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return a;}
HalSMVariable* HalSMSocket_div(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return a;}
HalSMVariable* HalSMSocket_pow(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return a;}
HalSMVariable* HalSMSocket_modulo(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return a;}
HalSMVariable* HalSMSocket_increment(HalSMCompiler* hsmc,HalSMVariable* v){return v;}
HalSMVariable* HalSMSocket_decrement(HalSMCompiler* hsmc,HalSMVariable* v){return v;}
unsigned char HalSMSocket_isEqual(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return a->type==b->type&&HSFCompare(((HalSMVariableCustom*)a->value)->type->name,((HalSMVariableCustom*)b->value)->type->name)&&((HalSMVariableCustom*)a->value)->value==((HalSMVariableCustom*)b->value)->value;}
unsigned char HalSMSocket_isMore(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return 0;}
unsigned char HalSMSocket_isLess(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return 0;}
unsigned char HalSMSocket_getBool(HalSMCompiler* hsmc,HalSMVariable* v){return ((HalSMSocket*)((HalSMVariableCustom*)v->value)->value)->isActive;}

long long int HalSMSocket_indexVariableByName(HalSMCompiler* hsmc,HalSMVariable* v,HalStringFormatChar* name){return -1;}

long long int HalSMSocket_indexFunctionByName(HalSMCompiler* hsmc,HalSMVariable* v,HalStringFormatChar* name)
{
    HalSMVariable* n=HalSMVariable_init_str(hsmc->memorymanagmentlibrary,name);
    long long int out=DictElementIndexByKey(hsmc,((HalSMVariableCustom*)v->value)->type->funcs,n);
    free(n->value);
    free(n);
    return out;
}

HalSMVariable* HalSMSocket_getVariableByName(HalSMCompiler* hsmc,HalSMVariable* v,HalStringFormatChar* name){return &hsvoid;}

HalSMVariable* HalSMSocket_getFunctionByName(HalSMCompiler* hsmc,HalSMVariable* v,HalStringFormatChar* name)
{
    HalSMVariable* n=HalSMVariable_init_str(hsmc->memorymanagmentlibrary,name);
    long long int out=DictElementIndexByKey(hsmc,((HalSMVariableCustom*)v->value)->type->funcs,n);
    free(n->value);
    free(n);
    if (out>-1) {
        return ((HalSMVariableCustom*)v->value)->type->funcs->elements[out]->value;
    }
    return &hsvoid;
}

HalSMVariable* HalSMSocket_getItem(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return &hsvoid;}
HalSMVariable* HalSMSocket_shiftRight(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return a;}
HalSMVariable* HalSMSocket_shiftLeft(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return a;}
HalSMVariable* HalSMSocket_convert(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return a;}
HalSMVariable* HalSMSocket_xor(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return a;}
HalSMVariable* HalSMSocket_and(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return a;}
HalSMVariable* HalSMSocket_or(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b){return a;}
HalSMVariable* HalSMSocket_bnot(HalSMCompiler* hsmc,HalSMVariable* v){return v;}

HalSMVariable* HalSMSocket_connect(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars)
{
    if (args->size==3&&args->arr[0]->type==HalSMVariableType_custom&&args->arr[1]->type==HalSMVariableType_str&&args->arr[2]->type==HalSMVariableType_int) {
        int iResult;
        HalSMSocket* sock;
        unsigned int port;
        HalStringFormatChar* addr;
        struct sockaddr_in saClient;
        struct hostent* host;
        char* ip;
        if (HSFCompare(((HalSMVariableCustom*)args->arr[0]->value)->type->name,HalSMSocketName)==0){return &hsvoid;}
        sock=(HalSMSocket*)((HalSMVariableCustom*)args->arr[0]->value)->value;
        addr=*(HalStringFormatChar**)args->arr[1]->value;
        port=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[2]->value);
        host=gethostbyname(HU8(addr));
        ip=inet_ntoa(*(struct in_addr*)*host->h_addr_list);

        saClient.sin_family=sock->family;
        saClient.sin_addr.S_un.S_addr=inet_addr(ip);
        saClient.sin_port=htons(port);
        iResult=connect(sock->sock,(SOCKADDR*)&saClient,sizeof(saClient));
        return HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMInteger_FromSignedInteger(hsmc->memorymanagmentlibrary,iResult),HalSMVariableType_int);
    }
    return &hsvoid;
}

HalSMVariable* HalSMSocket_bind(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars)
{
    if (args->size==3&&args->arr[0]->type==HalSMVariableType_custom&&args->arr[1]->type==HalSMVariableType_str&&args->arr[2]->type==HalSMVariableType_int) {
        int iResult;
        HalSMSocket* sock;
        unsigned int port;
        HalStringFormatChar* addr;
        struct sockaddr_in saClient;
        struct hostent* host;
        char* ip;
        if (HSFCompare(((HalSMVariableCustom*)args->arr[0]->value)->type->name,HalSMSocketName)==0){return &hsvoid;}
        sock=(HalSMSocket*)((HalSMVariableCustom*)args->arr[0]->value)->value;
        addr=*(HalStringFormatChar**)args->arr[1]->value;
        port=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[2]->value);
        host=gethostbyname(HU8(addr));
        ip=inet_ntoa(*(struct in_addr*)*host->h_addr_list);

        saClient.sin_family=sock->family;
        saClient.sin_addr.S_un.S_addr=inet_addr(ip);
        saClient.sin_port=htons(port);
        iResult=bind(sock->sock,(SOCKADDR*)&saClient,sizeof(saClient));
        return HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMInteger_FromSignedInteger(hsmc->memorymanagmentlibrary,iResult),HalSMVariableType_int);
    }
    return &hsvoid;
}

HalSMVariable* HalSMSocket_listen(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars)
{
    if (args->size>0&&args->size<3&&args->arr[0]->type==HalSMVariableType_custom) {
        int iResult;
        HalSMSocket* sock;
        int backlog;
        if (HSFCompare(((HalSMVariableCustom*)args->arr[0]->value)->type->name,HalSMSocketName)==0){return &hsvoid;}
        sock=(HalSMSocket*)((HalSMVariableCustom*)args->arr[0]->value)->value;
        if (args->size==2&&args->arr[1]->type==HalSMVariableType_int) {
            backlog=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[1]->value);
        } else {
            backlog=SOMAXCONN;
        }
        iResult=listen(sock->sock,backlog);
        return HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMInteger_FromSignedInteger(hsmc->memorymanagmentlibrary,iResult),HalSMVariableType_int);
    }
    return &hsvoid;
}

HalSMVariable* HalSMSocket_accept(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars)
{
    if (args->size==1&&args->arr[0]->type==HalSMVariableType_custom) {
        int iResult;
        HalSMSocket* sock;
        struct sockaddr_in saClient;
        int addrlen;
        SOCKET s;
        HalSMArray* out;
        if (HSFCompare(((HalSMVariableCustom*)args->arr[0]->value)->type->name,HalSMSocketName)==0){return &hsvoid;}
        sock=(HalSMSocket*)((HalSMVariableCustom*)args->arr[0]->value)->value;
        addrlen=sizeof(saClient);
        s=accept(sock->sock,(SOCKADDR*)&saClient,&addrlen);
        if (s==INVALID_SOCKET){return HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMInteger_FromSignedInteger(hsmc->memorymanagmentlibrary,-1),HalSMVariableType_int);}
        out=HalSMArray_init(hsmc->memorymanagmentlibrary);
        HalSMSocket* outsock=(HalSMSocket*)malloc(sizeof(HalSMSocket));
        outsock->family=saClient.sin_family;
        outsock->isActive=1;
        outsock->protocol=sock->protocol;
        outsock->type=sock->type;
        outsock->sock=s;
        HalSMArray_add(hsmc->memorymanagmentlibrary,out,HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMVariableCustom_init(hsmc->memorymanagmentlibrary,HalSMSocketType,outsock,DictInit(hsmc->memorymanagmentlibrary),DictInit(hsmc->memorymanagmentlibrary)),HalSMVariableType_custom));
        HalSMArray_add(hsmc->memorymanagmentlibrary,out,HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H(inet_ntoa(saClient.sin_addr))));
        HalSMArray_add(hsmc->memorymanagmentlibrary,out,HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMInteger_FromUnsignedInteger(hsmc->memorymanagmentlibrary,ntohs(saClient.sin_port)),HalSMVariableType_int));
        return HalSMVariable_init(hsmc->memorymanagmentlibrary,out,HalSMVariableType_HalSMArray);
    }
    return &hsvoid;
}

HalSMVariable* HalSMSocket_send(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars)
{
    if (args->size>1&&args->size<5&&args->arr[0]->type==HalSMVariableType_custom) {
        char* data;
        int len;
        int flags;
        int iResult;
        HalSMSocket* sock;
        if (HSFCompare(((HalSMVariableCustom*)args->arr[0]->value)->type->name,HalSMSocketName)==0){return &hsvoid;}
        sock=(HalSMSocket*)((HalSMVariableCustom*)args->arr[0]->value)->value;
        if (args->arr[1]->type==HalSMVariableType_str) {
            data=(char*)(*(HalStringFormatChar**)args->arr[1]->value);
            len=string_len(*(HalStringFormatChar**)args->arr[1]->value)*4;
        } else if (args->arr[1]->type==HalSMVariableType_custom&&HSFCompare(((HalSMVariableCustom*)args->arr[1]->value)->type->name,ByteArrayName)) {
            data=(char*)((HalSMByteArray*)((HalSMVariableCustom*)args->arr[1]->value)->value)->bytes;
            len=((HalSMByteArray*)((HalSMVariableCustom*)args->arr[1]->value)->value)->size;
        } else {
            return &hsvoid;
        }
        flags=0;
        if (args->size==3) {
            if (args->arr[2]->type!=HalSMVariableType_int) {
                return &hsvoid;
            }
            flags=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[2]->value);
        } else if (args->size==4) {
            if (args->arr[2]->type!=HalSMVariableType_int||args->arr[3]->type!=HalSMVariableType_int) {
                return &hsvoid;
            }
            len=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[2]->value);
            flags=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[3]->value);
        }
        iResult=send(sock->sock,data,len,flags);
        return HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMInteger_FromSignedInteger(hsmc->memorymanagmentlibrary,iResult),HalSMVariableType_int);
    }
    return &hsvoid;
}

HalSMVariable* HalSMSocket_recv(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars)
{
    if (args->size>1&&args->size<4&&args->arr[0]->type==HalSMVariableType_custom) {
        char* data;
        int len;
        int flags;
        int iResult;
        HalSMSocket* sock;
        if (HSFCompare(((HalSMVariableCustom*)args->arr[0]->value)->type->name,HalSMSocketName)==0){return &hsvoid;}
        sock=(HalSMSocket*)((HalSMVariableCustom*)args->arr[0]->value)->value;
        if (args->arr[1]->type!=HalSMVariableType_int){return &hsvoid;}
        flags=0;
        len=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[1]->value);
        data=calloc(len,sizeof(char));
        if (args->size==3) {
            if (args->arr[2]->type!=HalSMVariableType_int){return &hsvoid;}
            flags=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[2]->value);
        }
        iResult=recv(sock->sock,data,len,flags);
        if (iResult!=SOCKET_ERROR) {
            HalSMByteArray* out=(HalSMByteArray*)malloc(sizeof(HalSMByteArray));
            out->bytes=(unsigned char*)data;
            if (iResult<len){out->bytes=(unsigned char*)realloc(out->bytes,iResult);}
            out->size=iResult;
            return HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMVariableCustom_init(hsmc->memorymanagmentlibrary,(HalSMVariableTypeCustom*)DictElementFindByKey(hsmc,hsmc->sys_custom_variables,ByteArrayNameVar)->value->value,out,DictInit(hsmc->memorymanagmentlibrary),DictInit(hsmc->memorymanagmentlibrary)),HalSMVariableType_custom);
        }
        return HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMInteger_FromSignedInteger(hsmc->memorymanagmentlibrary,iResult),HalSMVariableType_int);
    }
    return &hsvoid;
}
HalSMVariable* HalSMSocket_sendto(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars)
{
    if (args->size>3&&args->size<7&&args->arr[0]->type==HalSMVariableType_custom&&args->arr[1]->type==HalSMVariableType_str&&args->arr[2]->type==HalSMVariableType_int) {
        char* data;
        int len;
        int flags;
        int iResult;
        HalSMSocket* sock;
        unsigned int port;
        HalStringFormatChar* addr;
        struct sockaddr_in saClient;
        struct hostent* host;
        char* ip;
        if (HSFCompare(((HalSMVariableCustom*)args->arr[0]->value)->type->name,HalSMSocketName)==0){return &hsvoid;}
        sock=(HalSMSocket*)((HalSMVariableCustom*)args->arr[0]->value)->value;
        addr=*(HalStringFormatChar**)args->arr[1]->value;
        port=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[2]->value);
        host=gethostbyname(HU8(addr));
        ip=inet_ntoa(*(struct in_addr*)*host->h_addr_list);

        saClient.sin_family=sock->family;
        saClient.sin_addr.S_un.S_addr=inet_addr(ip);
        saClient.sin_port=htons(port);
        if (args->arr[3]->type==HalSMVariableType_str) {
            data=(char*)args->arr[3]->value;
            len=string_len(*(HalStringFormatChar**)args->arr[3]->value)*4;
        } else if (args->arr[3]->type==HalSMVariableType_custom&&HSFCompare(((HalSMVariableCustom*)args->arr[3]->value)->type->name,ByteArrayName)) {
            data=(char*)((HalSMByteArray*)((HalSMVariableCustom*)args->arr[3]->value)->value)->bytes;
            len=((HalSMByteArray*)((HalSMVariableCustom*)args->arr[3]->value)->value)->size;
        } else {
            return &hsvoid;
        }
        flags=0;
        if (args->size==3) {
            if (args->arr[4]->type!=HalSMVariableType_int) {
                return &hsvoid;
            }
            flags=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[4]->value);
        } else if (args->size==4) {
            if (args->arr[4]->type!=HalSMVariableType_int||args->arr[5]->type!=HalSMVariableType_int) {
                return &hsvoid;
            }
            len=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[4]->value);
            flags=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[5]->value);
        }
        iResult=sendto(sock->sock,data,len,flags,(SOCKADDR*)&saClient,sizeof(saClient));
        return HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMInteger_FromSignedInteger(hsmc->memorymanagmentlibrary,iResult),HalSMVariableType_int);
    }
    return &hsvoid;
}

HalSMVariable* HalSMSocket_recvfrom(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars)
{
    if (args->size>1&&args->size<4&&args->arr[0]->type==HalSMVariableType_custom) {
        char* data;
        int len;
        int flags;
        struct sockaddr_in saClient;
        int addrlen;
        int iResult;
        HalSMSocket* sock;
        if (HSFCompare(((HalSMVariableCustom*)args->arr[0]->value)->type->name,HalSMSocketName)==0){return &hsvoid;}
        sock=(HalSMSocket*)((HalSMVariableCustom*)args->arr[0]->value)->value;
        if (args->arr[1]->type!=HalSMVariableType_int){return &hsvoid;}
        flags=0;
        len=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[1]->value);
        data=calloc(len,sizeof(char));
        if (args->size==3) {
            if (args->arr[2]->type!=HalSMVariableType_int){return &hsvoid;}
            flags=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[2]->value);
        }
        iResult=recvfrom(sock->sock,data,len,flags,(SOCKADDR*)&saClient,&addrlen);
        if (iResult!=SOCKET_ERROR) {
            HalSMByteArray* out=(HalSMByteArray*)malloc(sizeof(HalSMByteArray));
            out->bytes=(unsigned char*)data;
            if (iResult<len){out->bytes=(unsigned char*)realloc(out->bytes,iResult);}
            out->size=iResult;
            HalSMArray* outarr=HalSMArray_init(hsmc->memorymanagmentlibrary);
            HalSMArray_add(hsmc->memorymanagmentlibrary,outarr,HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMVariableCustom_init(hsmc->memorymanagmentlibrary,(HalSMVariableTypeCustom*)DictElementFindByKey(hsmc,hsmc->sys_custom_variables,ByteArrayNameVar)->value->value,out,DictInit(hsmc->memorymanagmentlibrary),DictInit(hsmc->memorymanagmentlibrary)),HalSMVariableType_custom));
            HalSMArray_add(hsmc->memorymanagmentlibrary,outarr,HalSMVariable_init_str(hsmc->memorymanagmentlibrary,U8H(inet_ntoa(saClient.sin_addr))));
            HalSMArray_add(hsmc->memorymanagmentlibrary,outarr,HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMInteger_FromUnsignedInteger(hsmc->memorymanagmentlibrary,ntohs(saClient.sin_port)),HalSMVariableType_int));
            return HalSMVariable_init(hsmc->memorymanagmentlibrary,outarr,HalSMVariableType_HalSMArray);
        }
        return HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMInteger_FromSignedInteger(hsmc->memorymanagmentlibrary,iResult),HalSMVariableType_int);
    }
    return &hsvoid;
}

HalSMVariable* HalSMSocket_close(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars)
{
    if (args->size==1&&args->arr[0]->type==HalSMVariableType_custom) {
        HalSMSocket* sock;
        int iResult;
        if (HSFCompare(((HalSMVariableCustom*)args->arr[0]->value)->type->name,HalSMSocketName)==0){return &hsvoid;}
        sock=(HalSMSocket*)((HalSMVariableCustom*)args->arr[0]->value)->value;
        iResult=closesocket(sock->sock);
        sock->isActive=0;
        return HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMInteger_FromSignedInteger(hsmc->memorymanagmentlibrary,iResult),HalSMVariableType_int);
    }
    return &hsvoid;
}

HalSMVariable* HalSMSocket_shutdown(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars)
{
    if (args->size==2&&args->arr[0]->type==HalSMVariableType_custom&&args->arr[1]->type==HalSMVariableType_int) {
        int how;
        HalSMSocket* sock;
        int iResult;
        if (HSFCompare(((HalSMVariableCustom*)args->arr[0]->value)->type->name,HalSMSocketName)==0){return &hsvoid;}
        sock=(HalSMSocket*)((HalSMVariableCustom*)args->arr[0]->value)->value;
        how=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[1]->value);
        iResult=shutdown(sock->sock,how);
        sock->isActive=0;
        return HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMInteger_FromSignedInteger(hsmc->memorymanagmentlibrary,iResult),HalSMVariableType_int);
    }
    return &hsvoid;
}

//HalSMSocketType

//Socket

HalSMVariable* Socket_socket(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars)
{
    if ((args->size==4&&args->arr[0]->type==HalSMVariableType_HalSMCModule&&args->arr[1]->type==HalSMVariableType_int&&args->arr[2]->type==HalSMVariableType_int&&args->arr[3]->type==HalSMVariableType_int)||(args->size==3&&args->arr[0]->type==HalSMVariableType_int&&args->arr[1]->type==HalSMVariableType_int&&args->arr[2]->type==HalSMVariableType_int)) {
        int af,type,protocol;
        unsigned long long int s=0;
        if (args->size==4) {
            s=1;
        }
        af=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[s]->value);
        type=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[s+1]->value);
        protocol=HalSMInteger_ToUnsignedInteger((HalSMInteger*)args->arr[s+2]->value);
        SOCKET out=socket(af,type,protocol);
        if (out==INVALID_SOCKET) {
            return &hsvoid;
        }
        HalSMSocket* hsocket=(HalSMSocket*)malloc(sizeof(HalSMSocket));
        hsocket->sock=out;
        hsocket->family=af;
        hsocket->type=type;
        hsocket->protocol=protocol;
        hsocket->isActive=1;
        return HalSMVariable_init(hsmc->memorymanagmentlibrary,HalSMVariableCustom_init(hsmc->memorymanagmentlibrary,HalSMSocketType,hsocket,DictInit(hsmc->memorymanagmentlibrary),DictInit(hsmc->memorymanagmentlibrary)),HalSMVariableType_custom);
    }
    return &hsvoid;
}

//Socket
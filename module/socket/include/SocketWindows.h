#ifndef SOCKETWINDOWS_H
#define SOCKETWINDOWS_H

#include <HalSM.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#ifdef BUILD_DLL
//dll exports
#define EXPORT __declspec(dllexport)
#else
//exe imports
#define EXPORT __declspec(dllimport)
#endif

typedef struct HalSMSocket {
    SOCKET sock;
    int family;
    int type;
    int protocol;
    unsigned char isActive;
} HalSMSocket;

//HalSMSocketType

HalStringFormatChar* HalSMSocket_toString(HalSMCompiler* hsmc,HalSMVariable* v);
HalSMVariable* HalSMSocket_add(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
HalSMVariable* HalSMSocket_sub(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
HalSMVariable* HalSMSocket_mul(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
HalSMVariable* HalSMSocket_div(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
HalSMVariable* HalSMSocket_pow(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
HalSMVariable* HalSMSocket_modulo(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
HalSMVariable* HalSMSocket_increment(HalSMCompiler* hsmc,HalSMVariable* v);
HalSMVariable* HalSMSocket_decrement(HalSMCompiler* hsmc,HalSMVariable* v);
unsigned char HalSMSocket_isEqual(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
unsigned char HalSMSocket_isMore(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
unsigned char HalSMSocket_isLess(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
unsigned char HalSMSocket_getBool(HalSMCompiler* hsmc,HalSMVariable* v);
long long int HalSMSocket_indexVariableByName(HalSMCompiler* hsmc,HalSMVariable* v,HalStringFormatChar* name);
long long int HalSMSocket_indexFunctionByName(HalSMCompiler* hsmc,HalSMVariable* v,HalStringFormatChar* name);
HalSMVariable* HalSMSocket_getVariableByName(HalSMCompiler* hsmc,HalSMVariable* v,HalStringFormatChar* name);
HalSMVariable* HalSMSocket_getFunctionByName(HalSMCompiler* hsmc,HalSMVariable* v,HalStringFormatChar* name);
HalSMVariable* HalSMSocket_getItem(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
HalSMVariable* HalSMSocket_shiftRight(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
HalSMVariable* HalSMSocket_shiftLeft(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
HalSMVariable* HalSMSocket_convert(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
HalSMVariable* HalSMSocket_xor(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
HalSMVariable* HalSMSocket_and(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
HalSMVariable* HalSMSocket_or(HalSMCompiler* hsmc,HalSMVariable* a,HalSMVariable* b);
HalSMVariable* HalSMSocket_bnot(HalSMCompiler* hsmc,HalSMVariable* v);

HalSMVariable* HalSMSocket_connect(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars);
HalSMVariable* HalSMSocket_bind(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars);
HalSMVariable* HalSMSocket_listen(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars);
HalSMVariable* HalSMSocket_accept(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars);
HalSMVariable* HalSMSocket_send(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars);
HalSMVariable* HalSMSocket_recv(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars);
HalSMVariable* HalSMSocket_sendto(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars);
HalSMVariable* HalSMSocket_recvfrom(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars);
HalSMVariable* HalSMSocket_close(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars);
HalSMVariable* HalSMSocket_shutdown(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars);

//HalSMSocketType

//Socket

HalSMCModule_entry* EXPORT Socket_init(HalSMCompiler* hsmc,HalSMArray* args);
HalSMVariable* Socket_socket(HalSMCompiler* hsmc,HalSMArray* args,Dict* vars);

//Socket

#endif
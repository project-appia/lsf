#pragma once

#include "module_definitions.h"

//---MACROS---
#if defined(_MSC_VER)
    //  Microsoft
    #define EXPORT extern "C" __declspec(dllexport)
    #define IMPORT extern "C" __declspec(dllimport)
#elif defined(__GNUC__)
    //  GCC
    #define EXPORT extern "C" __attribute__((visibility("default")))
    #define IMPORT
#else
    //  do nothing and hope for the best! issue warning
    #define EXPORT
    #define IMPORT
    #pragma warning Unknown dynamic link import/export semantics.
#endif



//---FACTORIES---
//destroy factory
EXPORT void destroy(Module* p_sc)
{
    p_sc->Exit();
    delete p_sc;
}

//macro create factory
#define EXPORT_MODULE(name) EXPORT Module* create_##name(void) {return new name;}






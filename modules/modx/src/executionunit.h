#pragma once

#include "module_definitions.h"

// the typedef for function pointer types used for dynamic library import
typedef Module* create_t();
typedef void destroy_t(Module*);


class ExecutionUnit
{
public:
    ExecutionUnit(create_t* p_create, destroy_t* p_destroy) : f_create(p_create), f_destroy(p_destroy){}
    inline void create()
    {
        m_mod =  f_create();
    }
    inline void destroy()
    {
        f_destroy(m_mod);
    }

    inline void Init()
    {
        m_mod->Init();
    }

    inline void Tick()
    {
        m_mod->Tick();
    }


private:
    Module* m_mod;
    create_t* f_create;
    destroy_t* f_destroy;
};


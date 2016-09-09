#pragma once

#include "module_definitions.h"
#include "executionunit.h"

#include <vector>
#include <string>




class Librarian
{
public:
    //interfaces
    Librarian();
    void load_modules(std::vector<ExecutionUnit> &p_modules_vec);

private:
    //methods


    //members
    std::vector<std::string> m_lib_names;
    std::vector<std::string> m_class_names;
    std::vector<void*> m_lib_handles;

};



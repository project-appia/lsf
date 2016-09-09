#include "librarian.h"
#include <iostream>
#include <dlfcn.h>


Librarian::Librarian()
{

    //tmp push lib names
    m_lib_names.push_back("mod_a");
    m_class_names.push_back("ModA");
    m_lib_names.push_back("mod_b");
    m_class_names.push_back("ModB");

}






void Librarian::load_modules(std::vector<ExecutionUnit> &p_modules_vec)
{
    void* handle;

    for(size_t i = 0; i< m_lib_names.size(); ++i)
    {
//        std::cout << "Opening Lib " << *lib_name << std::endl;

        //opening lib
        handle = dlopen(("./lib"+m_lib_names[i]+".so").c_str(), RTLD_LAZY);
        if(handle == NULL)
        {
            std::cerr << dlerror() << std::endl;
        }
        m_lib_handles.push_back(handle);
        dlerror(); //reset errors

        //obtain function pointers

        ExecutionUnit yada((create_t*) dlsym(handle, ("create_"+m_class_names[i]).c_str()), (destroy_t *) dlsym(handle, "destroy"));
        const char *dlsym_error = dlerror();
        if (dlsym_error) {
            std::cerr << "Cannot load symbol " << m_class_names[i] << ": " << dlsym_error << std::endl;
//            dlclose(handle);
            return ;
        }


        //pusback the execution unit
        p_modules_vec.push_back(yada);


   }

}


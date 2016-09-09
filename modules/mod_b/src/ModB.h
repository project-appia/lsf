#include "module_definitions.h"
#include "module_exports.h"



class ModB : public Module
{
public:
    ModB();
    ~ModB();
    virtual void Init();
    virtual void Input();
    virtual void Tick();
    virtual void Output();
    virtual void Exit();


};



//class export factory macro
EXPORT_MODULE(ModB)

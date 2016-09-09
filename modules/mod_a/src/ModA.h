#include "module_definitions.h"
#include "module_exports.h"



class ModA : public Module
{
public:
    ModA();
    ~ModA();
    virtual void Init();
    virtual void Input();
    virtual void Tick();
    virtual void Output();
    virtual void Exit();


};



//class export factory macro
EXPORT_MODULE(ModA)

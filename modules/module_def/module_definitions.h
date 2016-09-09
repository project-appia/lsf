#pragma once

//#include <string>
//#include <nanomsg/nn.h>
//#include <nanomsg/bus.h>


class Module
{

public:
    Module():m_deltat(0){};
    virtual ~Module(){}
    virtual void Init()=0;
    virtual void Input()=0;
    virtual void Tick()=0;
    virtual void Output()=0;
    virtual void Exit()=0;

protected:
    const double m_deltat;
};












































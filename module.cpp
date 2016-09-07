#include "module.h"

Module::Module(const std::string &p_id, dispatcher *dsp) :
    id(p_id),
    ntick(0)
{
    pub = nn_socket(AF_SP,NN_PUB);
    sub = nn_socket(AF_SP,NN_SUB);

    std::cout << "Module: " << p_id << " pub: " << pub << " sub: " << sub << std::endl;

    for (const auto &x : dsp->mdlMap[id].out)
    {
        if ( x != std::string())
        {
            out.push_back(x);
            nn_bind(pub,dsp->getDataPubAddr(x).c_str());
        }
    }

    for (const auto &x : dsp->mdlMap[id].inp)
    {
        if ( x != std::string())
        {
            inp.push_back(x);
            nn_setsockopt(sub, NN_SUB, NN_SUB_SUBSCRIBE, x.c_str(),0);
            nn_connect(sub,dsp->getDataSubAddr(x).c_str());
        }
    }
}

Module::~Module()
{
    nn_shutdown(pub,0);
    nn_shutdown(sub,0);
}

int Module::input()
{
    for (const auto &x : inp)
    {
        char *buf = nullptr;
        int  bytes = nn_recv(sub, &buf, NN_MSG, 0);
        std::cout << "Module " << id << " receiving  " << buf << std::endl;
        nn_freemsg(buf);
    }
}

int Module::tick(int n)
{
    ntick = n;
    input();
    output();
}

int Module::output()
{
    for (const auto &x : out)
    {
        int sz_x = x.length() +1;
        std::cout << "Module " << id << " publishing " << x << std::endl;
        int bytes = nn_send (pub, x.c_str(), sz_x, 0);
    }
}


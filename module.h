#ifndef MODULE_H
#define MODULE_H

#include <iostream>
#include <string>

#include "dispatcher.hpp"

// nanomsg includes
#include "nn.h"
#include "pubsub.h"

class Module
{
public:
    explicit Module(const std::string &p_id, dispatcher *dsp);
     ~Module();

     int input();
     int tick(int n = 0);
     int output();

private:
    int pub;
    int sub;

    std::string id;

    std::vector<std::string> inp;
    std::vector<std::string> out;

    int ntick;
};

#endif // MODULE_H

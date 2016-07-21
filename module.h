#ifndef MODULE_H
#define MODULE_H

#include <iostream>
#include <string>

#include "zmq.hpp"
#include "zmq.h"
#include "zhelpers.hpp"

#include "data.pb.h"
#include "dispatcher.hpp"

class Module
{
public:
    explicit Module(const std::string &p_id, dispatcher *dsp);
     ~Module();

     int input();
     int tick(int n = 0);
     int output();

private:
    zmq::context_t context;
    zmq::socket_t pub;
    zmq::socket_t sub;

    std::string id;

    std::vector<std::string> inp;
    std::vector<std::string> out;

    void logMessage(const google::protobuf::Message &m) {}

    int ntick;
};

#endif // MODULE_H

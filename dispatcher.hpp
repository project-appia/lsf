#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>

#include "data.pb.h"

class dispatcher
{
 public:

    struct sys_io {
       std::set<std::string> inp;
       std::set<std::string> out;
       std::string node;
    };

    struct msg_io{
       std::string pub;
       std::string sub;
    };

    std::map<std::string,sys_io>        mdlMap;
    std::map<std::string,msg_io>        addrMap;

    dispatcher(const std::string &file = std::string()) {
        //            |         inputs      |       output          |   node     |
        mdlMap["A"] = {{}                   ,{"data1"}              ,"localhost"};
        mdlMap["B"] = {{"data1"}            ,{"data2"}              ,"localhost"};
        mdlMap["C"] = {{}                   ,{"data3"}              ,"localhost"};
        mdlMap["D"] = {{"data1","data3"}    ,{}                     ,"localhost"};
        mdlMap["E"] = {{"data2"}            ,{"data4","data5"}      ,"localhost"};

        addrMap["data1"] = {{"tcp://*:5563"},{"tcp://" + getDataNode("data1") + ":5563"}};
        addrMap["data2"] = {{"tcp://*:5564"},{"tcp://" + getDataNode("data2") + ":5564"}};
        addrMap["data3"] = {{"tcp://*:5565"},{"tcp://" + getDataNode("data3") + ":5565"}};
        addrMap["data4"] = {{"tcp://*:5566"},{"tcp://" + getDataNode("data3") + ":5566"}};
        addrMap["data5"] = {{"tcp://*:5567"},{"tcp://" + getDataNode("data3") + ":5567"}};
    }

    std::string getDataPubAddr(const std::string &data) { return addrMap[data].pub;}
    std::string getDataSubAddr(const std::string &data) { return addrMap[data].sub;}

    std::string getDataNode(const std::string &data) {
        for (const auto &x : mdlMap) {
            if (x.second.out.find(data) != x.second.out.end())
                return x.second.node;
        }
    }
};

#endif // DISPATHCER_HPP

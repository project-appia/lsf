#pragma once

#include "librarian.h"
#include "rt-thread.h"



class Executioner
{
public:
    //interfaces
    Executioner();
    ~Executioner();
    void Run();

private:
    //methods
    void memory_lock();
    void memory_unlock();
    void set_latency_target();
    static void signal_handler(int sig);

    //members
    Librarian m_librarian;
    std::vector<ExecutionUnit> m_sim_modules; //array of function pointers
    RThread myt;


    int latency_target_fd = -1;
};

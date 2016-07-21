#include <iostream>
#include <chrono>
#include <thread>

#include "module.h"
#include "dispatcher.hpp"

int main(int argc, char *argv[])
{
    dispatcher *dsp = new dispatcher();

    Module mod_1("A",dsp);
    Module mod_2("B",dsp);
    Module mod_3("C",dsp);
    Module mod_4("D",dsp);
    Module mod_5("E",dsp);

    int i = 0;

    while (i < 20){
        std::cout << "Tick " << i << std::endl;
        mod_1.tick(i);
        mod_2.tick(i);
        mod_3.tick(i);
        mod_4.tick(i);
        mod_5.tick(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        i++;
    }
    return 0;
}

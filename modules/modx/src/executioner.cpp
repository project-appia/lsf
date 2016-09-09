#include "executioner.h"
#include <sys/mman.h>
#include <iostream>
#include <atomic>
#include <signal.h>

volatile std::atomic<bool> shutdown;

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>






Executioner::Executioner()
{
    //lock memory
    memory_lock();

    //define terminate signals
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);


    //very important to set cpu dma value
    set_latency_target();

    //load the library
    m_librarian.load_modules(m_sim_modules);


}


Executioner::~Executioner()
{
    //close cpu dma latency file
   if (latency_target_fd >= 0)
        close(latency_target_fd);

   //unlock memory
   memory_unlock();
}


void Executioner::Run()
{

    myt.create(m_sim_modules);


    while (!shutdown)
    {

        usleep(500000);
        myt.stats_send();
    }
    myt.join();
}






void Executioner::signal_handler(int sig)
{
     shutdown = true;
}





void Executioner::memory_lock()
{
    /* lock all memory (prevent swapping) */
    if (mlockall(MCL_CURRENT|MCL_FUTURE) == -1)
    {
        std::cout << "error: couldn't lock memory" << std::endl;
        exit(-1);
    }
    else
    {
        std::cout << "Memory Locked" << std::endl;
    }
}

void Executioner::memory_unlock()
{
    /* unlock everything */
    munlockall();
}




/* Latency trick
 * if the file /dev/cpu_dma_latency exists,
 * open it and write a zero into it. This will tell
 * the power management system not to transition to
 * a high cstate (in fact, the system acts like idle=poll)
 * When the fd to /dev/cpu_dma_latency is closed, the behavior
 * goes back to the system default.
 *
 * Documentation/power/pm_qos_interface.txt
 */
void Executioner::set_latency_target()
{
    struct stat s;
    int err;
    int latency_target_value =0;

    errno = 0;
    err = stat("/dev/cpu_dma_latency", &s);
    if (err == -1) {
            std::cout << errno <<  "WARN: stat /dev/cpu_dma_latency failed" << std::endl;
            return;
    }

    errno = 0;
    latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
    if (latency_target_fd == -1) {
            std::cout << errno <<  "WARN: open /dev/cpu_dma_latency" << std::endl;
            return;
    }

    errno = 0;
    err = write(latency_target_fd, &latency_target_value, 4);
    if (err < 1) {
            std::cout << errno <<  "# error setting cpu_dma_latency to " << latency_target_value << std::endl;
            close(latency_target_fd);
            return;
    }
    printf("# /dev/cpu_dma_latency set to %dus\n", latency_target_value);
}



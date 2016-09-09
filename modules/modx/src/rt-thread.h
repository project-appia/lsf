#pragma once

#include <iostream>
#include <atomic>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <sched.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <linux/unistd.h>

#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/mman.h>

#include "rt_numa.h"
#include "rt-utils.h"
#include "executionunit.h"
#include <vector>
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>
#include <assert.h>




#define gettid() syscall(__NR_gettid)
#define USEC_PER_SEC		1000000
#define NSEC_PER_SEC		1000000000

extern volatile std::atomic<bool> shutdown;

struct stats {
    unsigned short tuid;
    unsigned short priority;
    unsigned long  periodic_interval;
    unsigned long cycles;
    double avg;
    long min;
    long max;
};



class RThread
{
public:
    RThread();
    ~RThread();
    void create(std::vector<ExecutionUnit> p_runque);
    void join();
    void stats_send();

private:
    //methods
    void Start();
    void Stop();
    void Run();
    static void *timer_callback(void *tp);
    void signal_block(const unsigned short signum); const
    void set_cpu_affinity();
    void set_priority();
    int setscheduler(pid_t pid, int policy, const struct sched_param *param);
    int raise_soft_prio(int policy, const struct sched_param *param);

    //members
    pthread_t m_thread;			//actual thread
    pthread_attr_t m_thread_attr;

    unsigned short m_thread_uid;	//thread unique id
    unsigned short m_numa_node;
    unsigned short m_clock_mode;
    unsigned short m_linux_scheduler = SCHED_FIFO;
    unsigned short m_clock_source =  CLOCK_MONOTONIC;
    unsigned short m_timermode =  TIMER_ABSTIME;
    unsigned short m_priority = 99;
    unsigned long  m_periodic_interval = 10000; //in microseconds
    unsigned short m_cpu_core = 0;

    unsigned long  max_cycles;
    double avg = 0;
    unsigned long cycles = 0;
    long min = 100000;
    long max = 0;
    long act = 0;


    std::vector<ExecutionUnit> runqueue;

    std::string url = "ipc:///tmp/pipeline.ipc";
    int sock = nn_socket (AF_SP, NN_PUSH);
};

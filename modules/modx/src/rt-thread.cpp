#include "rt-thread.h"

static struct bitmask *affinity_mask = NULL;


static inline void tsnorm(struct timespec *ts) {

    while (ts->tv_nsec >= NSEC_PER_SEC) {
        ts->tv_nsec -= NSEC_PER_SEC;
        ts->tv_sec++;
    }
}

static inline int tsgreater(struct timespec *a, struct timespec *b) {

    return ((a->tv_sec > b->tv_sec) || (a->tv_sec == b->tv_sec && a->tv_nsec > b->tv_nsec));
}

static inline int64_t calcdiff(struct timespec t1, struct timespec t2) {

    int64_t diff;
    diff = USEC_PER_SEC * (long long)((int) t1.tv_sec - (int) t2.tv_sec);
    diff += ((int) t1.tv_nsec - (int) t2.tv_nsec) / 1000;

    return diff;
}

static inline int64_t calcdiff_ns(struct timespec t1, struct timespec t2) {

    int64_t diff;
    diff = NSEC_PER_SEC * (int64_t)((int) t1.tv_sec - (int) t2.tv_sec);
    diff += ((int) t1.tv_nsec - (int) t2.tv_nsec);

    return diff;
}




static int cpu_for_thread(int thread_num, int max_cpus) {

    unsigned int m, cpu, i, num_cpus;
    num_cpus = rt_numa_bitmask_count(affinity_mask);

    m = thread_num % num_cpus;

    /* there are num_cpus bits set, we want position of m'th one */
    for (i = 0, cpu = 0; i < max_cpus; i++) {
        if (rt_numa_bitmask_isbitset(affinity_mask, i)) {
            if (cpu == m)
                return i;
            cpu++;
        }
    }

    fprintf(stderr, "Bug in cpu mask handling code.\n");
    return 0;
}







RThread::RThread() {

    assert(sock >= 0);
    assert(nn_connect(sock, url.c_str()) >= 0);
}


RThread::~RThread(){

    nn_shutdown(sock, 0);
}


void RThread::create(std::vector<ExecutionUnit> p_runque) {

    runqueue = p_runque;


    void *stack;
    void *currstk;
    size_t stksize;
    int status;
    int max_cpus = sysconf(_SC_NPROCESSORS_ONLN);


    numa_on_and_available();


//    m_cpu_core = cpu_for_thread(1, max_cpus);

    status = pthread_attr_init(&m_thread_attr);
    if(status !=0)
        std::cout << "error from pthread_attr_init for thread " << m_thread_uid << "  " << strerror(status) << std::endl;

    /* find the memory node associated with the cpu i */
    m_numa_node = rt_numa_numa_node_of_cpu(m_cpu_core);

    /* get the stack size set for for this thread */
    if (pthread_attr_getstack(&m_thread_attr, &currstk, &stksize))
        std::cout << "failed to get stack size for thread " << m_thread_uid << std::endl;

    /* if the stack size is zero, set a default */
    if (stksize == 0)
        stksize = PTHREAD_STACK_MIN * 2;

    /*  allocate memory for a stack on appropriate node */
    stack = rt_numa_numa_alloc_onnode(stksize, m_numa_node, m_cpu_core);

    /* set the thread's stack */
    if (pthread_attr_setstack(&m_thread_attr, stack, stksize))
        std::cout << "failed to set stack addr for thread " << m_thread_uid << " to 0x" << std::hex << ((size_t)stack+stksize) << std::endl;



    if(pthread_create(&m_thread, &m_thread_attr, timer_callback, this))
        std::cout << "failed to create thread " << m_thread_uid << std::endl;
}


void RThread::join() {

    pthread_kill(m_thread, SIGTERM);
    pthread_join(m_thread, NULL);
}


void RThread::signal_block(const unsigned short signum = SIGALRM) {

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, signum);
    sigprocmask (SIG_BLOCK, &sigset, NULL);
}


int RThread::setscheduler(pid_t pid, int policy, const struct sched_param *param) {

    int err = 0;

try_again:
    err = sched_setscheduler(pid, policy, param);
    if (err) {
        err = errno;
        if (err == EPERM) {
            int err1;
            err1 = raise_soft_prio(policy, param);
            if (!err1) goto try_again;
        }
    }

    return err;
}




void *RThread::timer_callback(void * tp) {


    RThread* lrt = static_cast<RThread*>(tp);

    assert(lrt->sock >= 0);
    assert(nn_connect(lrt->sock, lrt->url.c_str()) >= 0);




    lrt->Start();



    cpu_set_t mask;
    sched_param schedp;
    struct timespec now, next, interval, stop;

    lrt->m_thread_uid = gettid();

    lrt->signal_block();
    rt_numa_set_numa_run_on_node(lrt->m_numa_node, lrt->m_cpu_core);


    interval.tv_sec = lrt->m_periodic_interval / USEC_PER_SEC;
    interval.tv_nsec = (lrt->m_periodic_interval% USEC_PER_SEC) * 1000;


    CPU_ZERO(&mask);
    CPU_SET(lrt->m_cpu_core, &mask);
    if(pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) == -1)
        std::cout << "Could not set CPU affinity to CPU " << lrt->m_cpu_core << std::endl;

    schedp.sched_priority = lrt->m_priority;
    if (lrt->setscheduler(0, lrt->m_linux_scheduler, &schedp))
        std::cout << "timerthread" << lrt->m_cpu_core << ": failed to set priority to " << lrt->m_priority << "\nProbably not superuser" << std::endl;


    clock_gettime(lrt->m_clock_source, &now);

    next = now;
    next.tv_sec += interval.tv_sec;
    next.tv_nsec += interval.tv_nsec;
    tsnorm(&next);

    unsigned long counter;

    while(!shutdown) {

        uint64_t diff;
        int sigs, ret;

        clock_nanosleep(lrt->m_clock_source, TIMER_ABSTIME, &next, NULL);

        clock_gettime(lrt->m_clock_source, &now);
        diff = calcdiff(now, next);

        if (diff < lrt->min)
                lrt->min = diff;
        if (diff > lrt->max)
                lrt->max = diff;

        lrt->act = diff;
        lrt->avg += (double) diff;
        lrt->cycles++;


        lrt->Run();

//        if( lrt->cycles > counter) { //trying thread side stat sending
//             lrt->stats_send();
//             counter  = counter + 50;
//        }


        next.tv_sec += interval.tv_sec;
        next.tv_nsec += interval.tv_nsec;
        tsnorm(&next);

        while (tsgreater(&now, &next)) {
                next.tv_sec += interval.tv_sec;
                next.tv_nsec += interval.tv_nsec;
                tsnorm(&next);
        }
    }

    schedp.sched_priority = 0;
    sched_setscheduler(0, SCHED_OTHER, &schedp);

    std::cout << "\n"<< std::endl;

    lrt->Stop();


    return NULL;
}



void RThread::Start() {

    //execute create in all units
    for(std::vector<ExecutionUnit>::iterator exec_unit = runqueue.begin(); exec_unit != runqueue.end(); ++exec_unit ) {
        exec_unit->create();
        exec_unit->Init();
    }
}

void RThread::Stop() {

    //execute create in all units
    for(std::vector<ExecutionUnit>::iterator exec_unit = runqueue.begin(); exec_unit != runqueue.end(); ++exec_unit ) {
        exec_unit->destroy();
    }
}

void RThread::Run() {

    //execute create in all units
    for(std::vector<ExecutionUnit>::iterator exec_unit = runqueue.begin(); exec_unit != runqueue.end(); ++exec_unit ) {
        exec_unit->Tick();
    }
}





int RThread::raise_soft_prio(int policy, const struct sched_param *param) {

    int err;
    int policy_max;	/* max for scheduling policy such as SCHED_FIFO */
    int soft_max;
    int hard_max;
    int prio;
    struct rlimit rlim;

    prio = param->sched_priority;

    policy_max = sched_get_priority_max(policy);

    if (policy_max == -1) {
        err = errno;
        std::cout << "WARN: no such policy" << std::endl;
        return err;
    }

    err = getrlimit(RLIMIT_RTPRIO, &rlim);

    if (err) {
        err = errno;
        std::cout << err << "WARN: getrlimit failed"<< std::endl;
        return err;
    }

    soft_max = (rlim.rlim_cur == RLIM_INFINITY) ? policy_max : rlim.rlim_cur;
    hard_max = (rlim.rlim_max == RLIM_INFINITY) ? policy_max : rlim.rlim_max;

    if (prio > soft_max && prio <= hard_max) {
        rlim.rlim_cur = prio;
        err = setrlimit(RLIMIT_RTPRIO, &rlim);
        if (err) {
             err = errno;
             std::cout << err << "WARN: setrlimit failed" << std::endl;
             /* return err; */
        }
    } else {
        err = -1;
    }

    return err;
}

void RThread::stats_send() {

    struct stats *lsat;
    lsat = (struct stats *) nn_allocmsg(sizeof(struct stats),0);

    lsat->tuid = m_thread_uid;
    lsat->priority = m_priority;
    lsat->periodic_interval = m_periodic_interval;
    lsat->cycles= cycles;
    lsat->avg = cycles ? (avg/cycles):0;
    lsat->min = min;
    lsat->max = max;

    int bytes = nn_send(sock, &lsat, NN_MSG, NN_DONTWAIT);
//    assert(bytes == sizeof(struct stats));
}

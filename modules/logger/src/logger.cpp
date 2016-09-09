#include <assert.h>
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>
#include <string>
#include <iostream>
#include <signal.h>
#include <sys/stat.h>
#include <sys/unistd.h>

volatile sig_atomic_t shutdown;



struct stats {
    unsigned short tuid;
    unsigned short priority;
    unsigned long  periodic_interval;
    unsigned long cycles;
    double avg;
    long min;
    long max;
};

void sighand(int signum)
{
   shutdown = true;
}



int main() {


   signal(SIGTERM,sighand);
   signal(SIGINT,sighand);



  std::string url = "ipc:///tmp/pipeline.ipc";
//  std::string url = "tcp:////pipeline.ipc";



  int sock = nn_socket (AF_SP, NN_PULL);
  assert (sock >= 0);
  assert (nn_bind (sock, url.c_str()) >= 0);



  std::string fmt;
  fmt = "T:%2d (%5d) P:%2d I:%ld C:%7lu Min:%7ld Avg:%6.2f Max:%8ld\n\033[%dA";

  struct stats *buf = (struct stats *)nn_allocmsg(sizeof(struct stats),0);

  while (!shutdown)
    {
      int bytes = nn_recv (sock, &buf, NN_MSG, 0);
      assert (bytes != sizeof(struct stat));

      fprintf(stdout, fmt.c_str(), 0, buf->tuid, buf->priority, buf->periodic_interval, buf->cycles, buf->min, buf->avg, buf->max, 0);
//      nn_freemsg (buf);
    }


}

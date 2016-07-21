#include "module.h"

Module::Module(const std::string &p_id, dispatcher *dsp) :
    id(p_id),
    context(1),
    pub(context,ZMQ_PUB),
    sub(context,ZMQ_SUB),
    ntick(0)
{
    for (const auto &x : dsp->mdlMap[id].out)
    {
        if ( x != std::string())
        {
            out.push_back(x);
            pub.bind(dsp->getDataPubAddr(x));
        }
    }

    for (const auto &x : dsp->mdlMap[id].inp)
    {
        if ( x != std::string())
        {
            inp.push_back(x);
            sub.connect(dsp->getDataSubAddr(x));
            sub.setsockopt(ZMQ_SUBSCRIBE,x.c_str(), 1);
        }
    }
}

Module::~Module()
{

}

int Module::input()
{
    for (const auto &x : inp)
    {
        std::string address = s_recv(sub);
        std::string content = s_recv(sub);

        if (content != std::string())
            std::cout << "[" << id << "]" << " received " << address << " " << content << std::endl;
        else
            std::cout << "[" << id << "]" << " missed data" << std::endl;
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
     /*  if (ntick > 10 && x == "data1") // simulates a malfunction after 10 ticks
           return 0;*/

       std::string content = "message " + std::to_string(ntick);
       std::cout << "[" << id << "]" << " published " << x << " " << content << std::endl;
       s_sendmore(pub,x);
       s_send(pub,content);
   }


  /* test::A2B message;
   message.set_src("A");
   message.set_dst("B");
   message.set_name("A2B");
   message.set_id(1);
   message.set_content("message from A to B");*/

   /*s_sendmore(pub, "data1");
   s_send(pub,"message" + id);*/
}


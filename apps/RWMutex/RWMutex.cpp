#include <base/Time/Global_Timer.h>
#include <base/Svar/Svar.h>
#include <base/Debug/StackTrace.h>
#include <list>

using namespace std;
using namespace pi;

int THREAD_COUNT= 4;

class Queue
{
public:
    void write(int value)
    {
        pi::WriteMutex   lockWrite(m_piMutex);
        m_list.push_back(value);
    }

    int  read(int pos)
    {
        pi::ReadMutex  lockRead(m_piMutex);

        if(size()==0 || pos>=size())
            return -1;

        std::list<int>::iterator itr = m_list.begin();
        std::advance(itr , pos);
        int value = *itr;

        return value;
    }

    void remove(int pos)
    {
        pi::WriteMutex   lockWrite(m_piMutex);
        if(size()==0 || pos>=size())
            return;

        std::list<int>::iterator itr = m_list.begin();
        std::advance(itr , pos);
        m_list.erase(itr);
    }

    int  size()
    {
        pi::ReadMutex  lockRead(m_piMutex);
        return m_list.size();
    }

private:
    std::list<int>  m_list;

    pi::MutexRW     m_piMutex;
    pi::Mutex       m_Mutex;
};

class QueueOperates : public pi::Runnable
{
public:
    QueueOperates(Queue& queue,int mode=0)
        :que(queue),md(mode)
    {

    }

    virtual void run()
    {
//        std::cout<<"Thread with mode "<<md<<" started.\n";
        int circulate=svar.GetInt("Circle",100000);
        for(int i=0;i<circulate;i++)
        {
            pi::sleep(1);
            switch (md) {

            case 0:
            {
                // write
                pi::timer.enter("Write");
//                if(que.size()<1000) que.write(rand());
                pi::timer.leave("Write");
                break;
            }
            case 1:
                // read
                pi::timer.enter("Read");
//                if(que.size())
//                    que.read(rand()%que.size());
                pi::timer.leave("Read");
                break;
            case 2:
                // remove
                pi::timer.enter("Remove");
//                if(que.size())
//                    que.remove(rand()%que.size());
                pi::timer.leave("Remove");
                break;
            default:
                break;
            }
        }
//        std::cout<<"Thread with mode "<<md<<" stoped.\n";
    }

    Queue& que;
    int    md;
};

int main(int argc , char* argv[])
{
    pi::dbg_stacktrace_setup();
    svar.ParseMain(argc,argv);
    vector<QueueOperates> operates;
    vector<SPtr<pi::Thread> >    threads;
    Queue                 queue;
    for(int i=0;i<3;i++)
    {
        cout<<"add thread "<<i<<endl;
        operates.push_back(QueueOperates(queue,i));
        pi::Thread* thread=new pi::Thread();
        threads.push_back(SPtr<pi::Thread>(thread));
        thread->start(&operates[i]);
    }


}

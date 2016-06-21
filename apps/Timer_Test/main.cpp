#include <iostream>

#include <base/Time/Global_Timer.h>

using namespace pi;
using namespace std;

int main()
{
    int i=100;
    while(i--)
    {
        timer.enter("Timer::DelayTest");
        timer.leave("Timer::DelayTest");
        timer.enter("Timer::True=10us");
        pi::sleep(10);
        timer.leave("Timer::True=10us");

        timer.enter("Timer::True=1ms");
        pi::sleep(1000);
        timer.leave("Timer::True=1ms");

        timer.enter("Timer::True=10ms");
        pi::sleep(10000);
        timer.leave("Timer::True=10ms");
    }

    TicTac tictac;
    Rate rate(30);
    cout<<"Rate.cycle(in microseconds)="<<rate.cycle<<endl;
    tictac.Tic();
    for(int j=0;j<30;j++)
    {
        timer.enter("Timer::Rate30");
        pi::sleep(1000);
        rate.sleep();
        timer.leave("Timer::Rate30");
    }
    cout<<"TicTac:"<<tictac.Tac();
}

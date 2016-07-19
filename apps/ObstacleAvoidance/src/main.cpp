#include <iostream>
#include <QApplication>

#include <base/Svar/Svar.h>
//#include <base/debug/debug_config.h>

#include "System.h"

int main(int argc,char** argv)
{
    svar.ParseMain(argc,argv);
//    pi::dbg_stacktrace_setup();
    if(svar.GetInt("Win3d.Enable",1))
    {
        QApplication app(argc,argv);

        System system;
        return app.exec();
    }
    else
    {
        System system;
    }
    return 0;
}

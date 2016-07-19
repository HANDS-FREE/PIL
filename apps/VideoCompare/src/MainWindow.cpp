#include "MainWindow.h"

#include <base/Svar/Scommand.h>
#include <gui/widgets/SvarTable.h>

using namespace std;

void GuiHandle(void *ptr,string cmd,string para)
{
    if(cmd=="MainWindow.show")
    {
        MainWindow* mainwindow=(MainWindow*)ptr;
        mainwindow->call("show");
    }

}

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent)
{
    // set window minimum size
    this->setMinimumSize(1000, 700);

    // window title
    setWindowTitle("Innno VideoCompare");

    // setup layout
    setupLayout();
    connect(this, SIGNAL(call_signal() ), this, SLOT(call_slot()) );

    scommand.RegisterCommand("MainWindow.show",GuiHandle,this);
}

int MainWindow::setupLayout(void)
{
    QWidget *wAll = new QWidget(this);

    // left pannel
    QTabWidget* m_tabWidget = new QTabWidget(this);

    SvarWidget* svarWidget=new SvarWidget(this);
    m_tabWidget->addTab(svarWidget,"SvarWidget");


    // overall layout
    QHBoxLayout *hl = new QHBoxLayout(wAll);
    wAll->setLayout(hl);

    hl->addWidget(m_tabWidget, 1);

    setCentralWidget(wAll);

    return 0;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int     key;

    key  = event->key();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
#if 0
    // 1 - left
    // 2 - right
    // 4 - middle
    printf("window pressed, %d, %d, %d\n", event->button(), event->pos().x(), event->pos().y());

    if( event->button() == 1 ) {

    }
#endif
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
}

void MainWindow::timerEvent(QTimerEvent *event)
{
}

void MainWindow::call(const std::string& cmd)
{
    cmds.push(cmd);
    emit call_signal();
}

void MainWindow::call_slot()
{
    if(cmds.size())
    {
        std::string& cmd=(cmds.front());
        if("show"==cmd)   show();
        if("update"==cmd) update();
        else
            scommand.Call(cmd);
        cmds.pop();
    }

}

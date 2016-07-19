#include <gui/widgets/SvarTable.h>
#include <base/Svar/Scommand.h>

#include "MainWindow.h"
#include "GLScence.h"

using namespace std;

void GuiHandle(void *ptr,string cmd,string para)
{
    if(cmd=="show")
    {
        MainWindow* mainwindow=(MainWindow*)ptr;
        mainwindow->call("show");
    }
    else if(cmd=="MainWindow")
    {
        MainWindow* mainwindow=(MainWindow*)ptr;
        mainwindow->call(para);
    }
}

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent),win3d(NULL)
{
    // set window minimum size
    this->setMinimumSize(1000, 700);

    // window title
    setWindowTitle("Obstacle Avoidance");

    // setup layout
    setupLayout();
    connect(this, SIGNAL(call_signal() ), this, SLOT(call_slot()) );

    scommand.RegisterCommand("show",GuiHandle,this);
    scommand.RegisterCommand("MainWindow",GuiHandle,this);
}

/**
                            MainWindow
                          /          \
                         TAB          Menu
                       /  |  \
              Simulation Svar PathPlan
             /        \
         Left          Right
         /   \         /   \
     Camera Scence   Info
       / \
    RGB Depth

 */

int MainWindow::setupLayout(void)
{
    scence=SPtr<GLScence>(new GLScence);
    // 0. Overall layout
    QWidget     *wAll = new QWidget(this);//overall widget
    QHBoxLayout *layoutAll = new QHBoxLayout(wAll);
    wAll->setLayout(layoutAll);

    // 1.Left pannel
    QWidget*  wLeft=new QWidget(wAll);
    QVBoxLayout *layoutLeft = new QVBoxLayout(wLeft);
    wLeft->setLayout(layoutLeft);
    layoutAll->addWidget(wLeft, 1);

    // 1.1. View table
    QTabWidget* viewTab = new QTabWidget(wLeft);
    win3d   = new pi::gl::Win3D(viewTab);
    viewTab->addTab(win3d,"Win3D");
    viewTab->setMinimumSize(640,480);
//    viewTab->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    win3d->insert(scence);
    win3d->setSceneRadius(500);

    setCentralWidget(viewTab);
    SvarWidget* svarWidget=new SvarWidget(this);
    viewTab->addTab(svarWidget,"SvarWidget");

    // 1.2. Simulate View
    QDockWidget* dockBottom=new QDockWidget(wAll);
    dockBottom->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea|Qt::BottomDockWidgetArea);
    QWidget*  wBottom=new QWidget(dockBottom);
    QVBoxLayout *layoutBottom = new QVBoxLayout(wBottom);
    wBottom->setLayout(layoutBottom);
    dockBottom->setWidget(wBottom);
    addDockWidget(Qt::LeftDockWidgetArea,dockBottom);

    // 1.2.1. Sim Color Image
    RGBCap =new SimCamera(this);
    RGBCap->insert(new GLScence);
    RGBCap->setSceneRadius(500);
    RGBCap->setFixedSize(640,480);

    RGBCap->depthImage=new QLabel(wBottom);
    RGBCap->depthImage->setText("Depth Image");
    RGBCap->depthImage->setFixedSize(640,480);
    layoutBottom->addWidget(RGBCap);
    layoutBottom->addWidget(RGBCap->depthImage,1);

    // 2. Right pannel
    QDockWidget* dockRight=new QDockWidget(wLeft);
    dockRight->setAllowedAreas(Qt::AllDockWidgetAreas);
    QWidget*  wRight=new QWidget(dockRight);
    QVBoxLayout *layoutRight = new QVBoxLayout(wRight);
    wRight->setLayout(layoutRight);
    dockRight->setWidget(wRight);
    addDockWidget(Qt::BottomDockWidgetArea,dockRight);

    // 2.1. Info table
    infoTab=new QTextEdit(wRight);
    layoutRight->addWidget(infoTab,1);

    return 0;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
//    int     key;

//    key  = event->key();
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
        std::stringstream cmdArg(cmds.front());

        std::string cmd;
        cmdArg>>cmd;
        if("show"==cmd) show();
        else if(cmd=="SetCameraPose")
        {
            pi::SE3f pose;cmdArg>>pose;
            RGBCap->setPose(pose);
        }
        else if(cmd=="UpdateText")
        {
            infoTab->setText(SvarWithType<QString>::instance()["SystemInfo"]);
        }
        else
            scommand.Call(cmd);
        cmds.pop();
    }

}

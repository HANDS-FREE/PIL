#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QLabel>
#include <queue>
#include <QTableWidget>
#include <QTextEdit>

#include "SimDepthCamera.h"

class GLScence;
class MainWindow:public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow(){}

    virtual int setupLayout(void);

    pi::gl::Win3D* getWin3D(){return win3d;}
    SimCamera*     getSimCamera(){return RGBCap;}
    SPtr<GLScence>& getScence(){return scence;}
    QTextEdit*     getInfoTab(){return infoTab;}

    void    call(const std::string& cmd);

signals:
    void call_signal();

protected slots:
    void call_slot();

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void timerEvent(QTimerEvent *event);

    pi::gl::Win3D*                win3d;
    SimCamera*                    RGBCap;
    QTextEdit*                    infoTab;

    std::queue<std::string>       cmds;
    SPtr<GLScence>                scence;
};
#endif

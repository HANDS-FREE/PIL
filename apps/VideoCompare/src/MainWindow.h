#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <queue>

class MainWindow:public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow(){}

    virtual int setupLayout(void);

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

    std::queue<std::string>       cmds;
};

#endif // MAINWINDOW_H

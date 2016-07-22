#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <base/Thread/Thread.h>


//class OpenWidget: public QWidget
//{
//    Q_OBJECT
//public:
//};

/**
 * @brief The MatWidget class aim to show a SvarWithTypeMat
 */
class MatWidget:public QWidget
{
    Q_OBJECT
public:
    explicit MatWidget(const QString& MatName,QWidget *parent=NULL);
    virtual ~MatWidget(){}

    void setupUI();
    void updateImage();
    virtual void paintEvent( QPaintEvent * event);

protected slots:
    void btnLoad_clicked(bool checked=false);
    void edtFilter_editingFinished(void);
public:
    QString        matName;
    QLineEdit*     m_edtFilter;
    QLabel*        img;
    SPtr<pi::MutexRW> mutex;
    int&           imgWidth;
};

class FileChooseWidget: public QWidget
{
    Q_OBJECT
public:
    explicit FileChooseWidget(const QString& VarName,QWidget *parent=NULL);
    virtual ~FileChooseWidget(){}

    void setupUI();

protected slots:
    void btnLoad_clicked(bool checked=false);
    void edtFilter_editingFinished(void);
public:
    QString        varName;
    QLineEdit*     m_edtFilter;
};

class MainWindow:public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow(){}

    virtual int setupLayout(void);

    void call(QString cmd);

signals:
    void call_signal(QString cmd);

protected slots:
    void call_slot(QString cmd);

    void btnStart_clicked(bool checked=false);
    void btnPause_clicked(bool checked=false);
    void btnStop_clicked(bool checked=false);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void timerEvent(QTimerEvent *event);
    void closeEvent(QCloseEvent *event);

    pi::Mutex                  mutex;
    std::vector<QString>       MatNames;
    std::vector<MatWidget*>    mats;
};

#endif // MAINWINDOW_H

#include "Win3D.h"

#include <base/Svar/Svar.h>
#include <base/Svar/Scommand.h>

namespace pi{
namespace gl{

using namespace std;

Win3D::Win3D(QWidget *parent)
    : QGLViewer(parent),fx(0),bDrawWithNames(false)
{
    connect(this, SIGNAL(update_signal(void)), this, SLOT(update_slot(void)));
    connect(this, SIGNAL(show_signal(void)), this, SLOT(show_slot(void)));
    restoreStateFromFile();

    this->setBackgroundColor(QColor(0, 0, 0));

    event_handle = NULL;
//    setSceneRadius(10);

    infos.push_back(&info);
}

void Win3D::keyPressEvent(QKeyEvent *e)
{
    if( event_handle != NULL ) {
        if( event_handle->KeyPressHandle(e) == false )
            QGLViewer::keyPressEvent(e);
    }
    else
        QGLViewer::keyPressEvent(e);
}

void Win3D::mousePressEvent(QMouseEvent* e)
{
    if( event_handle != NULL ) {
        if( event_handle->MousePressHandle(e) == false )
            QGLViewer::mousePressEvent(e);
    }
    else
        QGLViewer::mousePressEvent(e);
}

void Win3D::drawWithNames()
{
    bDrawWithNames=true;
    //Draw scence & objects
    {
        pi::ScopedMutex lock(m_mutex);
        scence.draw();
        for(size_t i=0;i<draw_opengl.size();i++)
            draw_opengl[i]->Draw_Something();
    }
    bDrawWithNames=false;
}

void Win3D::endSelection(const QPoint& point)
{
    Q_UNUSED(point);

    // Flush GL buffers
    glFlush();

    // Get the number of objects that were seen through the pick matrix frustum. Reset GL_RENDER mode.
    GLint nbHits = glRenderMode(GL_RENDER);
    cout<<"NbHits="<<nbHits<<endl;

    if (nbHits <= 0)
        setSelectedName(-1);
    else
    {
        // Interpret results: each object created 4 values in the selectBuffer().
        // selectBuffer[4*i+1] is the object minimum depth value, while selectBuffer[4*i+3] is the id pushed on the stack.
        // Of all the objects that were projected in the pick region, we select the closest one (zMin comparison).
        // This code needs to be modified if you use several stack levels. See glSelectBuffer() man page.
        GLuint zMin = (selectBuffer())[1];
        setSelectedName((selectBuffer())[3]);
        for (int i=1; i<nbHits; ++i)
            if ((selectBuffer())[4*i+1] < zMin)
            {
                zMin = (selectBuffer())[4*i+1];
                setSelectedName((selectBuffer())[4*i+3]);
            }
    }

    int slcted=selectedName();
    if(slcted>0)
    {
        svar.i["Win3D.SelectedName"]=slcted;
        scommand.Call("selectedName","Win3D.SelectedName");
    }
}

QString Win3D::helpString() const
{
    QString text("<h2>I n t e r f a c e</h2>");
    text += "A GUI can be added to a QGLViewer widget using Qt's <i>Designer</i>. Signals and slots ";
    text += "can then be connected to and from the viewer.<br><br>";
    text += "You can install the QGLViewer designer plugin to make the QGLViewer appear as a ";
    text += "standard Qt widget in the Designer's widget tabs. See installation pages for details.";
    return text;
}

void Win3D::initializeGL()
{
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    // Default colors
    setForegroundColor(QColor(svar.GetString("Win3D.ForegroundColor","#B4B4B4").c_str()));
    setBackgroundColor(QColor(svar.GetString("Win3D.BackgroundColor","#333333").c_str()));

    // Clear the buffer where we're going to draw
    if (format().stereo())
    {
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    else
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calls user defined method. Default emits a signal.
    init();

    // Give time to glInit to finish and then call setFullScreen().
    if (isFullScreen())
        QTimer::singleShot( 100, this, SLOT(delayedFullScreen()) );
}

void Win3D::loadProjectionMatrix()
{
    if(fx<=0||fy<=0)
    {
        camera()->loadProjectionMatrix();;
        return ;
    }

    double left,right,top,bottom,near,far;
    {
        near=camera()->zNear();
        far=camera()->zFar();

        left=-cx/fx*near;
        right=(width()-cx)/fx*near;
        bottom=-(height()-cy)/fy*near;
        top=(cy)/fy*near;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(left,right,bottom,top,near,far);
}

void Win3D::preDraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // GL_PROJECTION matrix
    if(fx>0)
    {
        loadProjectionMatrix();
    }
    else
    {
        camera()->loadProjectionMatrix();
    }
    // GL_MODELVIEW matrix
    camera()->loadModelViewMatrix();
    Q_EMIT drawNeeded();
}

void Win3D::draw()
{
    //Draw text
    if( svar.GetInt("Win3D.DrawInfoText", 1) ) {
        int j=1;
        for(size_t i=0;i<infos.size();i++)
        {
            stringstream& strs=*(infos[i]);
            if(strs.str().size()<2) continue;
            string str;
            stringstream tmp(strs.str());

            glColor3ub(0x00, 0xFF, 0xFF);
            for(;getline(tmp,str);j++)
                this->drawText(10, 25*j, QString::fromStdString(str),QFont("",12,2,false));
        }
    }

    //Draw scence & objects
    {
        pi::ScopedMutex lock(m_mutex);
        scence.draw();
        for(size_t i=0;i<draw_opengl.size();i++)
            draw_opengl[i]->Draw_Something();
    }
}

void Win3D::setPose(SE3f pose)
{
    pi::Point3f& t=pose.get_translation();
    pi::SO3f& r=pose.get_rotation();
    camera()->setPosition(qglviewer::Vec(t.x,t.y,t.z));
    camera()->setOrientation(qglviewer::Quaternion(r.w,r.z,-r.y,-r.x));
}

void Win3D::setCamera(int w,int h,double _fx,double _fy,double _cx,double _cy)
{
//    cout<<"SetCamera:"<<w<<h<<fx<<fy,
    setFixedSize(QSize(w,h));
    fx=_fx;fy=_fy;cx=_cx;cy=_cy;
    camera()->setType(qglviewer::Camera::PERSPECTIVE);

}

bool   Win3D::drawingWithNames()
{
    return bDrawWithNames;
}

}
}


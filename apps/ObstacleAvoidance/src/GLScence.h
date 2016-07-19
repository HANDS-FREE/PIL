#ifndef GLSCENCE_H
#define GLSCENCE_H

#include <gui/gl/GL_Object.h>
#include <base/types/SPtr.h>

class GLScenceImpl;
class GLScence:public pi::gl::GL_Object
{
public:
    GLScence();
    virtual void draw();
    void setViewPoint(double x,double y,double z);

private:
    SPtr<GLScenceImpl> impl;
};

#endif // GLSCENCE_H

#ifndef GLHELPER_H
#define GLHELPER_H

#include <base/Types/SE3.h>
#include "OpenGL.h"

inline void glVertex(const pi::Point3d& pt)
{
    glVertex3d(pt.x,pt.y,pt.z);
}

inline void glVertex(const pi::Point3f& pt)
{
    glVertex3f(pt.x,pt.y,pt.z);
}

inline void glColor(const pi::Point3ub& color)
{
    glColor3ub(color.x,color.y,color.z);
}

void glMultMatrix(const pi::SE3f& pose);
#endif // GLHELPER_H

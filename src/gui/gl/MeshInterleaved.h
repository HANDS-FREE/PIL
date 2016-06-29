#ifndef MESHINTERLEAVED_H
#define MESHINTERLEAVED_H
#include "GL_Object.h"
#include "base/Types/Point.h"
#include <vector>
#include <QImage>

namespace pi{
namespace gl{

typedef pi::Point3f Vertex3f;
typedef pi::Point2f Vertex2f;
typedef pi::Point3ub Color3b;
typedef unsigned char uchar;

class MeshInterleaved : public GL_Object
{
public:
    enum DisplayMode
    {
        MeshMode=0,
        LineMode=1,
        PointCloudMode=2
    };
    MeshInterleaved(unsigned int verticesPerFace=3);

    MeshInterleaved &operator+=(const MeshInterleaved &mesh);

    void clear();

    bool loadPLY(std::string  filename);
    bool loadOBJ(std::string  filename);

    bool writeOBJ(std::string filename);
    bool writePLY(std::string filename, bool binary = true);

    virtual void draw();

    std::vector<Vertex3f>       vertices;
    std::vector<unsigned int>   faces;
    std::vector<Vertex3f>       normals;
    std::vector<Color3b>        colors;
    std::vector<unsigned int>   edges;
    std::vector<Vertex2f>       texcoords;

    uchar                       _verticesPerFace;

    // gl buffer things
    unsigned int                _currentNV,_currentNF;
    uint                        _vertexBuffer;
    uint                        _faceBuffer;
    uint                        _edgeBuffer;
    uint                        _normalBuffer;
    uint                        _colorBuffer;

    bool                        _colorEnabled,_updatingMesh;
    int                         _displayMode;

protected:
    void generateBuffers();

    std::vector<unsigned int>   materialIndices;
    std::vector<QImage>         textures;
};

}}

#endif // MESHINTERLEAVED_H

#include "MeshInterleaved.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdint.h>
#include "OpenGL.h"
#include "ply/TinyPly.h"

namespace pi {
namespace gl{

using namespace std;

MeshInterleaved::MeshInterleaved(unsigned int verticesPerFace)
:_verticesPerFace(verticesPerFace),_currentNV(0),_currentNF(0),_vertexBuffer(0),
  _colorEnabled(true),_updatingMesh(false),_displayMode(LineMode){}


void MeshInterleaved::clear()
{
    vertices.clear();
    faces.clear();
    normals.clear();
    colors.clear();
    edges.clear();
    texcoords.clear();
    materialIndices.clear();
    textures.clear();
}

bool MeshInterleaved::writeOBJ(std::string filename)
{
    std::string prefix = filename.substr(0,filename.find_last_of('/')+1);
    std::string stem = filename.substr(prefix.size(),filename.size());
    std::string filenameObject = filename + ".obj";
    std::string filenameMaterial = filename + ".mtl";
    std::string filenameMaterialRelative = stem + ".mtl";


    std::fstream file;
    file.open(filenameObject.c_str(),std::ios::out);
    if(!file.is_open()){
        fprintf(stderr,"\nERROR: Could not open File %s for writing!",filenameObject.c_str());
        return false;
    }
    else
        fprintf(stderr,"\nOpen File %s for writing!",filenameObject.c_str());


    std::vector<std::string> materialNames;
    bool useTexture = textures.size() > 0
            && texcoords.size() == faces.size();
    if(useTexture){
        fprintf(stderr,"\nThe Mesh has %li Textures of the following Sizes:\n",textures.size());
        for(size_t i=0;i<textures.size();i++){
            fprintf(stderr," %i*%i",textures[i].width(),textures[i].height());
            char buffer[500];
            sprintf(buffer,"material%.5li",i);
            materialNames.push_back(buffer);
        }
    }


    file << "\n#Written from Volume";
    std::cerr<<"#Written from Volume\n";
    flush(std::cerr);
    if(useTexture) file << "\nmtllib " << filenameMaterialRelative;
    for(unsigned int i=0;i<vertices.size();i++){
        file << "\nv " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z;
        if(colors.size()){
            file << " " << (float)(colors[i].x)/255.0f << " " << (float)(colors[i].y)/255.0f << " " << (float)(colors[i].z)/255.0f;
        }
    }

    //Normals
    std::cerr<<"#Writting normals.\n";
    flush(std::cerr);
    for(unsigned int i=0;i<normals.size();i++){
        file << "\nvn " << normals[i].x << " " << normals[i].y << " " << normals[i].z;
    }


    //Texture
    std::cerr<<"#Writting Texture.\n";
    flush(std::cerr);
    if(useTexture){
        for(unsigned int t=0;t<texcoords.size();t++){
            file << "\nvt " << texcoords[t].x << " " << texcoords[t].y;
        }
//		file << "\nusemtl material0";
    }

    if(normals.size()){
        //Normals are given
        for(unsigned int i=0;i<faces.size();i+=_verticesPerFace){
            file << "\nf";
            for(unsigned int j=0;j<_verticesPerFace;j++)
                file << " " << faces[i+j]+1 << "//" << faces[i+j];
        }
    }
    else{
        //No normals
        if(useTexture){
            unsigned int currentMaterialNumber = 0;
            unsigned int nextMaterialIndex = 0;
            //No Normals, Texture Coordinates
            for(unsigned int i=0;i<faces.size();i+=_verticesPerFace){
                if(nextMaterialIndex==i){
                    file << "\nusemtl " << materialNames[currentMaterialNumber++];
                    nextMaterialIndex =
                            materialIndices.size()>currentMaterialNumber ?
                                    materialIndices[currentMaterialNumber] : faces.size();
                }
                file << "\nf";
                for(unsigned int j=0;j<_verticesPerFace;j++)
                    file << " " << faces[i+j]+1 << "/" << (i+j+1);
            }
        }
        else{
            //No Texture, No Normals
            for(unsigned int i=0;i<faces.size();i+=_verticesPerFace){
                file << "\nf";
                for(unsigned int j=0;j<_verticesPerFace;j++)
                    file << " " << faces[i+j]+1;
            }
        }
    }


    file.close();

    if(useTexture){
        std::fstream materialfile;
        materialfile.open(filenameMaterial.c_str(),std::ios::out);
        for(unsigned int i=0;i<textures.size();i++){
            char filenameTexture[500]; sprintf(filenameTexture,"%s_%.5i.png",filename.c_str(),i);
            char filenameTextureRelative[500]; sprintf(filenameTextureRelative,"%s_%.5i.png",stem.c_str(),i);
            if(!materialfile.is_open()){
                fprintf(stderr,"\nERROR: Could not open Material File %s for writing!",filenameMaterial.c_str());
                return false;
            }
            if(i) materialfile << "\n";
            materialfile << "newmtl " << materialNames[i]
                         << "\nKa 1.000000 1.000000 1.000000"
                     << "\nKd 1.000000 1.000000 1.000000"
                     << "\nKs 0.000000 0.000000 0.000000"
                     << "\nTr 1.000000"
                     << "\nillum 1"
                     << "\nNs 0.000000"
                     << "\nmap_Kd " << filenameTextureRelative;
//            cv::imwrite(filenameTexture,textures[i]);
        }
        materialfile.close();
    }

    return true;
}


bool MeshInterleaved::writePLY(std::string filename, bool binary){
    std::fstream file;
    file.open((filename+".ply").c_str(),std::ios::out|std::ios::binary);
    if(!file.is_open()){
        fprintf(stderr,"\nERROR: Could not open File %s for writing!",(filename+".ply").c_str());
        return false;
    }

    file << "ply";
    if(binary)file << "\nformat binary_little_endian 1.0";
    else file << "\nformat ascii 1.0";
    file << "\nelement vertex " << vertices.size();
    file << "\nproperty float32 x\nproperty float32 y\nproperty float32 z";
    if(colors.size())
        file << "\nproperty uchar red\nproperty uchar green\nproperty uchar blue";
    if(faces.size()){
        file << "\nelement face " << faces.size()/_verticesPerFace;
        file << "\nproperty list uint8 int32 vertex_indices";
    }
    if(edges.size()){
        file << "\nElement edge " << edges.size()/2;
        file << "\nproperty int vertex1\nproperty int vertex2";
    }
    file << "\nend_header";
    if(binary) file << "\n";

    for(unsigned int i=0;i<vertices.size();i++){
        if(binary){
            file.write((char*)(&(vertices[i])),sizeof(Vertex3f));
        }
        else file << "\n" << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z;
        if(colors.size()){
            if(binary){
                file.write((char*)(&(colors[i])),sizeof(Color3b));
            }
            else file << " " << (int)(colors[i].x) << " " << (int)(colors[i].y) << " " << (int)(colors[i].z);
        }
    }
    for(unsigned int i=0;i<faces.size();i+=_verticesPerFace){
        if(binary){
            file.write((char*)(&_verticesPerFace),sizeof(uchar));
        }
        else file << "\n" << (int)_verticesPerFace;
        for(unsigned int j=0;j<_verticesPerFace;j++)
            if(binary){
                unsigned int idx = faces[i+j];
                file.write((char*)(&idx),sizeof(unsigned int));
            }
            else file << " " << (faces[i+j]);
    }
    for(unsigned int i=0;i<edges.size();i+=2){
        if(binary){
            unsigned int idx = edges[i];
            file.write((char*)(&idx),sizeof(unsigned int));
            idx = edges[i+1]; file.write((char*)(&idx),sizeof(unsigned int));
        }
        else file << "\n " << edges[i] << " " << edges[i+1];
    }

    file.close();
    return true;
}


bool MeshInterleaved::loadPLY(std::string filename)
{
    try{
        // Read the file and create a std::istringstream suitable
        // for the lib -- tinyply does not perform any file i/o.
        std::ifstream ss(filename.c_str(), std::ios::binary);

        // Parse the ASCII header fields
        pi::PlyFile file(ss);

//        for (PlyElement& e : file.get_elements())
//        {
//            std::cout << "element - " << e.name << " (" << e.size << ")" << std::endl;
//            for (PlyProperty& p : e.properties)
//            {
//                std::cout << "\tproperty - " << p.name << " (" << PropertyTable[p.propertyType].str << ")" << std::endl;
//            }
//        }
//        std::cout << std::endl;

        // Define containers to hold the extracted data. The type must match
        // the property type given in the header. Tinyply will interally allocate the
        // the appropriate amount of memory.
        std::vector<float> verts;
        std::vector<float> norms;
        std::vector<uint8_t> colors;

        std::vector<uint32_t> faces;
        std::vector<float> uvCoords;
        std::vector<uint8_t> faceColors;

        uint32_t vertexCount, normalCount, colorCount, faceCount, faceTexcoordCount, faceColorCount;
        vertexCount = normalCount = colorCount = faceCount = faceTexcoordCount = faceColorCount = 0;

        // The count returns the number of instances of the property group. The vectors
        // above will be resized into a multiple of the property group size as
        // they are "flattened"... i.e. verts = {x, y, z, x, y, z, ...}
        vertexCount = file.request_properties_from_element("vertex", {"x", "y", "z"}, verts);
        normalCount = file.request_properties_from_element("vertex", {"nx", "ny", "nz"}, norms);
        colorCount = file.request_properties_from_element("vertex", {"red", "green", "blue", "alpha"}, colors);

        // For properties that are list types, it is possibly to specify the expected count (ideal if a
        // consumer of this library knows the layout of their format a-priori). Otherwise, tinyply
        // defers allocation of memory until the first instance of the property has been found
        // as implemented in file.read(ss)
        faceCount = file.request_properties_from_element("face", {"vertex_indices"}, faces, 3);
        faceTexcoordCount = file.request_properties_from_element("face", {"texcoord"}, uvCoords, 6);
        faceColorCount = file.request_properties_from_element("face", {"red", "green", "blue", "alpha"}, faceColors);

        file.read(ss);

        // Good place to put a breakpoint!
//        std::cout << "\tRead " << verts.size() << " total vertices (" << vertexCount << " properties)." << std::endl;
//        std::cout << "\tRead " << norms.size() << " total normals (" << normalCount << " properties)." << std::endl;
//        std::cout << "\tRead " << colors.size() << " total vertex colors (" << colorCount << " properties)." << std::endl;
//        std::cout << "\tRead " << faces.size() << " total faces (triangles) (" << faceCount << " properties)." << std::endl;
//        std::cout << "\tRead " << uvCoords.size() << " total texcoords (" << faceTexcoordCount << " properties)." << std::endl;
//        std::cout << "\tRead " << faceColors.size() << " total face colors (" << faceColorCount << " properties)." << std::endl;


        if(verts.size()!=vertexCount*3||norms.size()!=normalCount*3||colors.size()!=colorCount*3
                ||faces.size()!=faceCount*3||uvCoords.size()!=faceTexcoordCount*2||faceColors.size()!=faceColorCount*3)
        {
            return false;
        }
        vertices.resize(vertexCount);
        memcpy(vertices.data(),verts.data(),sizeof(Vertex3f)*vertexCount);

        normals.resize(normalCount);
        memcpy(normals.data(),normals.data(),sizeof(Vertex3f)*normalCount);

        this->faces=faces;

        this->colors.resize(colorCount);
        memcpy(this->colors.data(),colors.data(),sizeof(Color3b)*colorCount);
    }
    catch(std::exception& e)
    {
        clear();
        cerr<<"Failed to load mesh "<<filename<<" since "<<e.what()<<endl;
        return false;
    }
    return false;
}

bool MeshInterleaved::loadOBJ(std::string filename)
{
    return false;
}

MeshInterleaved &MeshInterleaved::operator+=(const MeshInterleaved &mesh)
{

    unsigned int oldVertexSize = vertices.size();
    unsigned int oldFaceSize = faces.size();

    for(unsigned int i=0;i<mesh.vertices.size();i++){
        vertices.push_back(mesh.vertices[i]);
    }
    if(mesh.colors.size()){
        for(unsigned int i=0;i<mesh.colors.size();i++){
            colors.push_back(mesh.colors[i]);
        }
    }
    if(mesh.faces.size()){
        for(unsigned int i=0;i<mesh.faces.size();i++){
            faces.push_back(mesh.faces[i]+oldVertexSize);
        }
    }
    if(mesh.edges.size()){
        for(unsigned int i=0;i<mesh.edges.size();i++){
            edges.push_back(mesh.edges[i]+oldVertexSize);
        }
    }

    if(mesh.textures.size()){
        for(unsigned int i=0;i<mesh.textures.size();i++){
            materialIndices.push_back(mesh.materialIndices[i]+oldFaceSize);
            textures.push_back(mesh.textures[i]);
        }
    }
    if(mesh.texcoords.size()){
        for(unsigned int i=0;i<mesh.texcoords.size();i++){
            texcoords.push_back(mesh.texcoords[i]);
        }
    }

    return *this;
}

void MeshInterleaved::generateBuffers(){
    glewInit();
    glGenBuffers(1, &_vertexBuffer);
    glGenBuffers(1, &_faceBuffer);
    glGenBuffers(1, &_colorBuffer);
    glGenBuffers(1, &_normalBuffer);
}

void MeshInterleaved::draw()
{
    if(faces.size()<1) return;

    if(vertices.size() != _currentNV ||
            faces.size() != _currentNF){
        if(!_vertexBuffer){
            generateBuffers();
        }
        _currentNV = vertices.size();
        _currentNF = faces.size();

        glBindBuffer(GL_ARRAY_BUFFER,_vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(Vertex3f),vertices.data(), GL_STATIC_DRAW);
        if(colors.size()){
            glBindBuffer(GL_ARRAY_BUFFER,_colorBuffer);
            glBufferData(GL_ARRAY_BUFFER,colors.size()*3,colors.data(), GL_STATIC_DRAW);
        }

        if(faces.size()){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_faceBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size()*sizeof(unsigned int),
                         faces.data(), GL_STATIC_DRAW);
        }

        std::vector<bool> checks(vertices.size(),false);
        for(size_t i=0;i<faces.size();i++)
            checks[faces[i]] = true;

        bool loneVertex = false;
        for(size_t i=0;i<checks.size();i++) loneVertex |= !checks[i];
        if(loneVertex){
            fprintf(stderr,"\nThere were lone Vertices!");
        }
        printf("\nMesh Check done");
    }


    glBindBuffer(GL_ARRAY_BUFFER,_vertexBuffer);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    if(colors.size()){
        glBindBuffer(GL_ARRAY_BUFFER,_colorBuffer);
        glColorPointer(3, GL_UNSIGNED_BYTE, 0, 0);
    }
    else{
        glColor3f(0.5f,0.5f,0.5f);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    if(_colorEnabled&&colors.size()==vertices.size()) {
        glEnableClientState(GL_COLOR_ARRAY);
    }
    else{
        glColor3f(0.5f,0.5f,0.5f);
    }


    if(_displayMode==LineMode){
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);
        glLineWidth(0.5f);
    }
    else{
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    }

    if(_displayMode==PointCloudMode){
        glPointSize(2.0);
        glBindBuffer(GL_ARRAY_BUFFER,_vertexBuffer);
        glDrawArrays(GL_POINTS,0,vertices.size());
    }
    else{
        glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT,0);
    }

    if(_colorEnabled&&colors.size()==vertices.size()) glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

}}

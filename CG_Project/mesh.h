#ifndef MESH_H_sdfjlasdfjfsdfjljfasdf
#define MESH_H_sdfjlasdfjfsdfjljfasdf

#include "Vertex.h"
#include <vector>
#include <map>
#include <string>
#include "sphere.h"
#include "plane.h"
#include "material.h"



/************************************************************
 * Triangle Class
 ************************************************************/
class Triangle {
public:
    inline Triangle () {
        v[0] = v[1] = v[2] = 0;
    }
    inline Triangle (const Triangle & t2) {
        v[0] = t2.v[0];
        v[1] = t2.v[1];
        v[2] = t2.v[2];

        t[0] = t2.t[0];
        t[1] = t2.t[1];
        t[2] = t2.t[2];

    }
    inline Triangle (unsigned int v0, unsigned int t0, unsigned int v1, unsigned int t1, unsigned int v2, unsigned int t2) {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;

        t[0] = t0;
        t[1] = t1;
        t[2] = t2;
    }
    inline virtual ~Triangle () {}
    inline Triangle & operator= (const Triangle & t2) {
        v[0] = t2.v[0];
        v[1] = t2.v[1];
        v[2] = t2.v[2];
        t[0] = t2.v[0];
        t[1] = t2.v[1];
        t[2] = t2.v[2];
        return (*this);
    }
    unsigned int v[3];
    unsigned int t[3];
};

/************************************************************
 * Class de maillage basique
 ************************************************************/
class Mesh {
public:
	Mesh(){}
    inline Mesh (const std::vector<Vertex> & v, const std::vector<Triangle> & t) : vertices (v), triangles (t)  {}
    bool loadMesh(const char * filename, bool randomizeTriangulation);
	bool loadMtl(const char * filename, std::map<std::string, unsigned int> & materialIndex);
    void computeVertexNormals ();
    void draw();
    void drawSmooth();

	//this is relevant for you:
	//Vertices are the vertex positions, and normals of the mesh.
	std::vector<Vertex> vertices;
	//this is relevant for you:
	//texCoords are the texture coordinates, these are DIFFERENT indices in triangles.
	//in the current version, if you use textures, then you have to use texture coords everywhere...
	//I might send an update of the code to change this.
	//for convenience, Vec3Df is used, although only 2D tex coordinates are read corresponding to the x,y entry of Vec3Df.
	std::vector<Vec3Df> texcoords;
	//Triangles are the indices of the vertices involved in a triangle.
	//a triplet corresponds to one triangle. 
	//A Triangle contains the indeces of the three vertices that are neighboring
    std::vector<Triangle> triangles;
	//These are the material properties
	//each triangle (!) has a material. Use the triangle index to receive a material INDEX
	std::vector<unsigned int> triangleMaterials;
	//using the material index, you can then recover the material from this vector
	//the class material is defined just above
	std::vector<Material> materials;
    
    std::vector<Plane> planes;
    std::vector<Sphere> spheres;
    
    
    void addPlane(const Plane &plane) {
        planes.push_back(plane);
    }
    
    void addSphere(const Sphere &sphere) {
        spheres.push_back(sphere);
    }
};

#endif // MESH_H

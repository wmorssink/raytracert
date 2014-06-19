#include <stdio.h>
#include <float.h>
#include <algorithm>
#ifdef WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "raytracing.h"

bool Ambient = false;
bool Reflection = false;
bool Shadows = false;
bool Specular = false;

std::vector<Vec3Df> normals;
//temporary variables
Vec3Df testRayOrigin;
Vec3Df testRayDestination;

//use this function for any preprocessing of the mesh.
void init()
{
	//load the mesh file
	//feel free to replace cube by a path to another model
	//please realize that not all OBJ files will successfully load.
	//Nonetheless, if they come from Blender, they should.
    //MyMesh.loadMesh("dodgeColorTest.obj", true);
    #ifdef __APPLE__
        /*
        * I wanted to set the path variable correctly or at least store it in some
        * global variable / function, but I couldn't manage to get this done.
        * This way, at least the Windows users have no problems...
        */
        MyMesh.loadMesh("/Users/jgmeligmeyling/git/ti1805raytracer/CG_project/cube.obj", true);
    #else
        MyMesh.loadMesh("cube.obj", true);
    #endif
	
	MyMesh.computeVertexNormals();
    calculateNormals();
	//one first move: initialize the first light source
	//at least ONE light source has to be in the scene!!!
	//here, we set it to the current location of the camera
	MyLightPositions.push_back(MyCameraPosition);
}


void calculateNormals(){
    
    
    for (int i=0; i<MyMesh.triangles.size();i++){
    Vec3Df edge01 = MyMesh.vertices[MyMesh.triangles[i].v[1]].p - MyMesh.vertices[MyMesh.triangles[i].v[0]].p;
    Vec3Df edge02 = MyMesh.vertices[MyMesh.triangles[i].v[2]].p - MyMesh.vertices[MyMesh.triangles[i].v[0]].p;
    Vec3Df normal = Vec3Df::crossProduct(edge01, edge02);
    normals.push_back(normal);
    }
}


/*
returns true if a vector the NULL vector.
else returns false
*/
bool isNullVector(Vec3Df v){
	return v[0] == 0 && v[1] == 0 && v[2] == 0;
}


/*
returns true if there is an intersect and outputs the intersection point as intersectOut.
*/
bool rayIntersectTriangle(Vec3Df R[], Vec3Df T[], Vec3Df* intersectOut){
	const float SMALL_NUM = 0.00001f;
	// R[0] = add(R[0], R[1]);
	Vec3Df u, v, n, dir, w0, w;
	float r, a, b;

	// get triangle edge vectors and plane normal
	u = T[1] - T[0];
	v = T[2] - T[0];
	n = Vec3Df::crossProduct(u, v); // cross product
	if (isNullVector(n)) // triangle is degenerate
		return NULL; // do not deal with this case
	dir = R[1] - R[0]; // ray direction vector
	w0 = R[0] - T[0];
	b = Vec3Df::dotProduct(n, dir);
	a = -Vec3Df::dotProduct(n, w0);
	if (abs(b) < SMALL_NUM) { // ray is parallel to triangle plane
		/*
		* if (a == 0) // ray lies in triangle plane else // ray disjoint
		* from plane
		*/
		return NULL;
	}

	// get intersect point of ray with triangle plane
	r = a / b;
	if (r < 0) { // ray goes away from triangle
		return NULL; // => no intersect
	}
	// for a segment, also test if (r > 1.0) => no intersect

	Vec3Df I = R[0] + r * dir; // intersect point of ray and
	// plane
	// is I inside T?
	float uu, uv, vv, wu, wv, D;
	uu = Vec3Df::dotProduct(u, u);
	uv = Vec3Df::dotProduct(u, v);
	vv = Vec3Df::dotProduct(v, v);
	w  = I - T[0];
	wu = Vec3Df::dotProduct(w, u);
	wv = Vec3Df::dotProduct(w, v);
	D  = uv * uv - uu * vv;

	// get and test parametric coords
	float s, t;
	s = (uv * wv - vv * wu) / D;
	if (s < 0 || s > 1) { // I is outside T
		return NULL;
	}
	t = (uv * wu - uu * wv) / D;
	if (t < 0 || (s + t) > 1) { // I is outside T
		return NULL;
	}
	memcpy(intersectOut, &I, sizeof(Vec3Df));
	return true; // I is in T
}

/*
returns the index of the triangle of the intersection in the mesh structure.
returns -1 if no intersect is found.
Also writes the intersection point of the triangle to intersectOut.
*/
int intersectMesh(Vec3Df origin, Vec3Df dest, Vec3Df* intersectOut){
	Vec3Df intersect; //intersection point of closest triangle
	int index = -1;	  //index of closest triangle
	float dist = FLT_MAX;

	Vec3Df R[] = { origin, dest };
	for (unsigned int i = 0; i < MyMesh.triangles.size(); i++){
		Vec3Df tempIntersect;
		Triangle triangle = MyMesh.triangles[i];
		Vec3Df T[3] = { MyMesh.vertices[triangle.v[0]].p, MyMesh.vertices[triangle.v[1]].p, MyMesh.vertices[triangle.v[2]].p };


		if (rayIntersectTriangle(R, T, &tempIntersect)){
			//ray intersects with the current triangle
			float tempDist = Vec3Df::distance(origin, tempIntersect);
			if (tempDist < dist){
				dist = tempDist;
				index = i;
				intersect = tempIntersect;
			}
		}
	}
	memcpy(intersectOut, &intersect, sizeof(Vec3Df));
	return index;
}

Vec3Df diffuseOnly(const Vec3Df& vertexPos, Vec3Df& normal, Material& material) {
    // Return the color of the material at the triangle hit
    // No shading etc. taken into account
    Vec3Df Kd = material.Kd();
    
    for(int light_index = 0; light_index < MyLightPositions.size(); light_index++) {
        // Calculate the normalized vector from the vertex to the light source
        Vec3Df lightDirection = MyLightPositions[light_index] - vertexPos;
        lightDirection.normalize();
        // Calculate the dot product between the normal and the lightVector
        float dot = Vec3Df::dotProduct(normal, lightDirection);
        // Clamp the dotproduct
        if (dot < 0) {
            dot = 0;
        }
        return Kd * dot;
    }
    
    return Kd;
}

// Phong (!) Shading Specularity (http://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model)
// Follow the course, only calculate Ks pow(dot(V,R),shininess), where V is the view vector and R is the Reflection vector of the light (like in pool billard computed from the LightPos, vertexPos and normal).
// When computing specularities like this, verify that the light is on the right side of the surface, with respect to the normal
// E.g., for a plane, the light source below the plane cannot cast light on the top, hence, there can also not be any specularity.
Vec3Df phongSpecularOnly(const Vec3Df & vertexPos, Vec3Df & normal, Material& material)
{
	// Vector between vertex and camera
	Vec3Df cameraVector = MyCameraPosition - vertexPos;
	normal.normalize();
	cameraVector.normalize();
    
    for(unsigned int light_index = 0; light_index < MyLightPositions.size(); light_index++) {
        Vec3Df lightPos = MyLightPositions[light_index];
        // Vector between light and vertex
        Vec3Df lightVector = lightPos - vertexPos;
        // Normalize the vectors
        lightVector.normalize();
    
        // Calculate the dot product
        float dotProduct = Vec3Df::dotProduct(normal, lightVector);
        // Clamp the dot product
        if (dotProduct < 0) {
            // Clamp to zero
            return Vec3Df(0, 0, 0);
        }
        else {
            // Reflection on mirror can be calculated through
            // r = v - 2 * dot(n, v) * n
            // where v is the viewpoint vector, r is the reflection vector and n is hte normal
            // with all vectors normalized. (According to slide 74 Ray Tracing)
            Vec3Df R = 2 * dotProduct * normal - lightVector;
            return Vec3Df(1,1,1) * powf(Vec3Df::dotProduct(cameraVector, R), 1);
            //return material.Ks() * powf(Vec3Df::dotProduct(cameraVector, R), material.Ns());
        }
	 }
    
    return Vec3Df(0,0,0);
}

//return the color of your pixel.
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest)
{
	Vec3Df intersectOut;
	int index = intersectMesh(origin, dest, &intersectOut);

	if (index == -1)//no intersection with triangle.
		return Vec3Df(0, 0, 0);
    
    Vec3Df normal = normals[index];
    int materialIndex = MyMesh.triangleMaterials[index];
    Material material = MyMesh.materials[materialIndex];

    Vec3Df diffusePart = diffuseOnly(intersectOut, normal, material);
    Vec3Df specularPart = phongSpecularOnly(intersectOut, normal, material);
    
    // Just diffuse
    return diffusePart;
    
    // Diffuse and specular, clipped between 0 and 1
    // return Vec3Df(fmax(fmin(diffusePart[0] + specularPart[0], 1), 0),
    //            fmax(fmin(diffusePart[1] + specularPart[1], 1), 0),
    //            fmax(fmin(diffusePart[2] + specularPart[2], 1), 0));
    
    // Diffuse + specular
    // return diffuseOnly(diffusePart, normal, material) + phongSpecularOnly(intersectOut, normal, material);
    
    // Everything white (to test intersect)
    //return Vec3Df(1,1,1);
}




void yourDebugDraw()
{
	//draw open gl debug stuff
	//this function is called every frame

	//as an example: 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glColor3f(0,1,1);
	glBegin(GL_LINES);
	glVertex3f(testRayOrigin[0], testRayOrigin[1], testRayOrigin[2]);
	glVertex3f(testRayDestination[0], testRayDestination[1], testRayDestination[2]);
	glEnd();
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex3fv(MyLightPositions[0].pointer());
	glEnd();
	glPopAttrib();

}

void yourKeyboardFunc(char key, int x, int y){
    
    switch(key){
        case '1':
            Ambient=!Ambient;
            break;
            
        case '2':
            Reflection=!Reflection;
            break;
            
        case '3':
            Shadows=!Shadows;
            break;
            
        case '4':
            Specular=!Specular;
            break;
    }
    
    // Activate ambient lighting.
    if(Ambient){
        std::cout<<std::endl<<("DEBUG --- Ambient on")<<std::endl;
    }
    else{
        std::cout<<std::endl<<("DEBUG --- Ambient off")<<std::endl;
    }
    
    // Activate reflection.
    if(Reflection){
        std::cout<<std::endl<<("DEBUG --- Reflection on")<<std::endl;
    }
    else{
        std::cout<<std::endl<<("DEBUG --- Reflection off")<<std::endl;
    }
    
    // Activate shadow.
    if(Shadows){
        std::cout<<std::endl<<("DEBUG --- Shadows on")<<std::endl;
    }
    else{
        std::cout<<std::endl<<("DEBUG --- Shadows off")<<std::endl;
    }
    
    // Activate specularity.
    if(Specular){
        std::cout<<std::endl<<("DEBUG --- Speculartiy on")<<std::endl;
    }
    else{
        std::cout<<std::endl<<("DEBUG --- Specularity off")<<std::endl;
    }
	// do what you want with the keyboard input t.
	// x, y are the screen position

	//here I use it to get the coordinates of a ray, which I then draw in the debug function.
	produceRay(x, y, testRayOrigin, testRayDestination);

	std::cout<<" pressed! The mouse was in location "<<x<<","<<y<<"!"<<std::endl;
}

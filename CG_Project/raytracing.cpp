#include <stdio.h>
#include <float.h>
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

#define pixelfactor 3
unsigned int pixelfactorX = pixelfactor;
unsigned int pixelfactorY = pixelfactor;

std::vector<Vec3Df> normals;
//temporary variables
Vec3Df testRayOrigin;
Vec3Df testRayDestination;

//use this function for any preprocessing of the mesh.
void init(char* fileName)
{
	//load the mesh file
	//feel free to replace cube by a path to another model
	//please realize that not all OBJ files will successfully load.
	//Nonetheless, if they come from Blender, they should.

	if (fileName == NULL){
		#ifdef __APPLE__
			/*
			* I wanted to set the path variable correctly or at least store it in some
			* global variable / function, but I couldn't manage to get this done.
			* This way, at least the Windows users have no problems...
			*/
			MyMesh.loadMesh("/Users/jgmeligmeyling/git/ti1805raytracer/CG_Project/cube.obj", true);
		#else
			//set default model
			fileName = "cube.obj";//"dodgeColorTest.obj"
		#endif
	}
	MyMesh.loadMesh(fileName, true);
	
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


Vec3Df getDiffuseComponent(const int index, const Vec3Df intersection) {
    // Return the color of the material at the triangle hit
    // No shading etc. taken into account
    int materialIndex = MyMesh.triangleMaterials[index];
    Vec3Df Kd = MyMesh.materials[materialIndex].Kd();
    
    Vec3Df normal=normals[index];
    
    for(int light_index = 0; light_index < MyLightPositions.size(); light_index++) {
        // Calculate the normalized vector from the vertex to the light source
        Vec3Df lightDirection = MyLightPositions[light_index] - intersection;
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

//return the color of your pixel.
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest)
{
	Vec3Df intersectOut;
	int index = intersectMesh(origin, dest, &intersectOut);

	if (index == -1)//no intersection with triangle.
		return Vec3Df(0, 0, 0);

    return getDiffuseComponent(index, intersectOut);
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
		case '+':
			pixelfactorX++;
			pixelfactorY++;
			printf("pixelfactorX = %i\npixelfactorY = %i\n", pixelfactorX, pixelfactorY);
			break;
		case '-':
			pixelfactorX--;
			pixelfactorY--;
			if (pixelfactorX < 1)
				pixelfactorX = 1;
			if (pixelfactorY < 1)
				pixelfactorY = 1;
			printf("pixelfactorX = %i\npixelfactorY = %i\n", pixelfactorX, pixelfactorY);
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



/*
returns true if there is an intersect and outputs the intersection point as intersectOut.
*/
bool rayIntersectRectangle(Vec3Df R[], Vec3Df T[], Vec3Df* intersectOut){
	const float SMALL_NUM = 0.00001f;
	// R[0] = add(R[0], R[1]);
	Vec3Df u, v, n, dir, w0, w;
	float r, a, b;

	// get triangle edge vectors and plane normal
	u = T[1] - T[0];
	v = T[2] - T[0];
	n = Vec3Df::crossProduct(u, v); // cross product
	if (isNullVector(n)) // triangle is degenerate
		return false; // do not deal with this case
	dir = R[1] - R[0]; // ray direction vector
	w0 = R[0] - T[0];
	b = Vec3Df::dotProduct(n, dir);
	a = -Vec3Df::dotProduct(n, w0);
	if (abs(b) < SMALL_NUM) { // ray is parallel to triangle plane
		/*
		* if (a == 0) // ray lies in triangle plane else // ray disjoint
		* from plane
		*/
		return false;
	}

	// get intersect point of ray with triangle plane
	r = a / b;
	if (r < 0) { // ray goes away from triangle
		return false; // => no intersect
	}
	// for a segment, also test if (r > 1.0) => no intersect

	Vec3Df I = R[0] + r * dir; // intersect point of ray and
	// plane
	// is I inside T?
	float uu, uv, vv, wu, wv, D;
	uu = Vec3Df::dotProduct(u, u);
	uv = Vec3Df::dotProduct(u, v);
	vv = Vec3Df::dotProduct(v, v);
	w = I - T[0];
	wu = Vec3Df::dotProduct(w, u);
	wv = Vec3Df::dotProduct(w, v);
	D = uv * uv - uu * vv;

	// get and test parametric coords
	float s, t;
	s = (uv * wv - vv * wu) / D;
	if (s < 0 || s > 1) { // I is outside T
		return false;
	}
	t = (uv * wu - uu * wv) / D;
	if (t < 0 || t > 1) { // I is outside T
		return false;
	}
	memcpy(intersectOut, &I, sizeof(Vec3Df));
	return true; // I is in T
}

bool rayIntersectRectangle(Vec3Df R[], Vec3Df v0, Vec3Df v1, Vec3Df v2, Vec3Df* intersectOut){
	Vec3Df T[] = { v0, v1, v2 };
	return rayIntersectRectangle(R, T, intersectOut);
}

/**
* boxIntersect
*
* @param ray
* 			Vector3f {origin, destination}
* @param loc
*            Location of the box (corner with the smallest coordinates)
* @param w
*            Width of the box (>0) (x-coord)
* @param l
*            Length of the box (>0) (z-coord)
* @param h
*            Height of the box (>0) (y-coord)
* @param returnIntersect
* 			closest Intersection point with the box
* @return Closest intersection with the box from the ray start
*/
float rayIntersectBox(Vec3Df ray[], Vec3Df loc, float w, float l, float h, Vec3Df* returnIntersect) {
	Vec3Df c = loc;
	// Calculate points of box
	c[0] += w;
	c[1] += h;
	c[2] += l;
	Vec3Df rec[] = { loc, loc, loc, loc, c, c, c, c };
	rec[1][0] += w;
	rec[2][1] += h;
	rec[3][2] += l;
	rec[5][0] -= w;
	rec[6][1] -= h;
	rec[7][2] -= l;

	// Calculate closest rectangle intersection
	float returnDistance = FLT_MAX;
	Vec3Df inmem;
	Vec3Df* in = &inmem;
	if (rayIntersectRectangle(ray, rec[0], rec[1], rec[2], returnIntersect)) {
		returnDistance = Vec3Df::distance(ray[0], *returnIntersect);
	}
	if (rayIntersectRectangle(ray, rec[0], rec[1], rec[3], in)) {
		float distance = Vec3Df::distance(ray[0], *in);
		if (distance < returnDistance) {
			returnDistance = distance;
			returnIntersect = in;
		}
	}
	if (rayIntersectRectangle(ray, rec[0], rec[2], rec[3], in)) {
		float distance = Vec3Df::distance(ray[0], *in);
		if (distance < returnDistance) {
			returnDistance = distance;
			returnIntersect = in;
		}
	}
	if (rayIntersectRectangle(ray, rec[4], rec[5], rec[6], in)) {
		float distance = Vec3Df::distance(ray[0], *in);
		if (distance < returnDistance) {
			returnDistance = distance;
			returnIntersect = in;
		}
	}
	if (rayIntersectRectangle(ray, rec[4], rec[5], rec[7], in)) {
		float distance = Vec3Df::distance(ray[0], *in);
		if (distance < returnDistance) {
			returnDistance = distance;
			returnIntersect = in;
		}
	}
	if (rayIntersectRectangle(ray, rec[5], rec[6], rec[7], in)) {
		float distance = Vec3Df::distance(ray[0], *in);
		if (distance < returnDistance) {
			returnDistance = distance;
			returnIntersect = in;
		}
	}

	return (returnDistance);
}

Vec3Df boxIntersectTest(Vec3Df ray[], float x, float y, float z, float w, float h, float l){
	Vec3Df b = Vec3Df(0, 0, 0);
	rayIntersectBox(ray, Vec3Df(x, y, z), w, h, l, &b);
	if (!isNullVector(b)){
		return Vec3Df(1, 0, 0);
	}
	return Vec3Df(0, 1, 0);
}
#include <stdio.h>
#include <float.h>
#include <algorithm>
#ifdef WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <windows.h>
#include <GL/glut.h>
#endif
#include "raytracing.h"
#include "sphere.h"

bool Ambient = true;
bool Diffuse = true;
bool Reflection = true;
bool Shadows = true;
bool Specular = true;
bool Refraction = true;
bool WireFrame = false;

#define pixelfactor 3	//use 3 for good looking, 1 for fast performance
unsigned int pixelfactorX = pixelfactor;
unsigned int pixelfactorY = pixelfactor;

#define BLACK Vec3Df(0, 0, 0);

int max_lvl = 15;//max recursive depth

using namespace std;

vector<Vec3Df> normals;
vector<Sphere> spheres;

//temporary variables
Vec3Df testRayOrigin;
Vec3Df testRayDestination;
//for ray debugging
vector<Vec3Df> o, d;
bool DebugMode = false;
bool IntersectMode = false;
bool ShowNormalMode = false;
//////////

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
			fileName = "/Users/jgmeligmeyling/git/ti1805raytracer/CG_Project/ cube.obj";
		#else
			//set default model
			fileName = "cube.obj";//"dodgeColorTest.obj"
		#endif
	}
    // Skip loading mesh for now, just spheres
	MyMesh.loadMesh(fileName, true);
	MyMesh.computeVertexNormals();

	//calculate normals for triangles at startup
    calculateNormals();
	
	//one first move: initialize the first light source
	//at least ONE light source has to be in the scene!!!
	//here, we set it to the current location of the camera
	MyLightPositions.push_back(MyCameraPosition);
    
    spheres.push_back(Sphere(Vec3Df(0, -1006,0), 1000.0f, Material::SoftPink));
    //spheres.push_back(Sphere(Vec3Df(1,1,1), .25f, Material::BlueSemiTransparent));
    spheres.push_back(Sphere(Vec3Df(0,0,0), .5f, Material::DiffuseWhite));
    spheres.push_back(Sphere(Vec3Df(0,1,0), .5f, Material::DiffuseWhite));
}

/*
For each triangle in MyMesh.triangles it calculates the normal and adds it to the normals vector.
*/
void calculateNormals(){
    for (int i=0; i<MyMesh.triangles.size();i++){
		Vec3Df edge01 = MyMesh.vertices[MyMesh.triangles[i].v[1]].p - MyMesh.vertices[MyMesh.triangles[i].v[0]].p;
		Vec3Df edge02 = MyMesh.vertices[MyMesh.triangles[i].v[2]].p - MyMesh.vertices[MyMesh.triangles[i].v[0]].p;
		Vec3Df normal = Vec3Df::crossProduct(edge01, edge02);
		normal.normalize();
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
bool rayIntersectTriangle(const Ray ray, Vec3Df T[], Vec3Df* intersectOut){
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
	dir = ray.destination - ray.origin; // ray direction vector
	w0 = ray.origin - T[0];
	b = Vec3Df::dotProduct(n, dir);
	a = -Vec3Df::dotProduct(n, w0);
	if (abs(b) < SMALL_NUM) { // ray is parallel to triangle plane
		// ray lies in triangle plane else // ray disjoint  from plane
		return false;
	}

	// get intersect point of ray with triangle plane
	r = a / b;
	if (r < 0) { // ray goes away from triangle
		return false; // => no intersect
	}
	// for a segment, also test if (r > 1.0) => no intersect

	Vec3Df I = ray.origin + r * dir; // intersect point of ray and
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
		return false;
	}
	t = (uv * wu - uu * wv) / D;
	if (t < 0 || (s + t) > 1) { // I is outside T
		return false;
	}
	memcpy(intersectOut, &I, sizeof(Vec3Df));
	return true; // I is in T
}

/*
 returns true if the triangle intersects in the mesh structure.
 returns false if no intersect is found.
 Also writes the intersection point, its normal and the material of the triangle to intersectOut.
 */
bool intersect(const Ray &ray, Vec3Df* intersectOut, Vec3Df* normalOut, Material* materialOut) {
    Vec3Df intersect, normal, tempIntersect;
    Material material, tempMaterial;
    float dist = FLT_MAX; // Find the object with the shortest distance (first hit)
    
    for(unsigned long i = 0, s = MyMesh.triangles.size(); i < s; i++) {
        Triangle triangle = MyMesh.triangles[i];
		Vec3Df T[3] = { MyMesh.vertices[triangle.v[0]].p, MyMesh.vertices[triangle.v[1]].p, MyMesh.vertices[triangle.v[2]].p };
        if (rayIntersectTriangle(ray, T, &tempIntersect)){
			//ray intersects with the current triangle
			float tempDist = Vec3Df::distance(ray.origin, tempIntersect);
			if (tempDist < dist){
				dist = tempDist;
                normal = normals[i];
                int materialIndex = MyMesh.triangleMaterials[i];
                material = MyMesh.materials[materialIndex];
				intersect = tempIntersect;
			}
		}
    }
    
    for(unsigned long i = 0, s = spheres.size(); i < s; i++) {
        Sphere sphere = spheres[i];
        if(sphere.intersect(ray, &tempIntersect)) {
			//ray intersects with the current triangle
			float tempDist = Vec3Df::distance(ray.origin, tempIntersect);
			if (tempDist < dist){
				dist = tempDist;
                normal = sphere.getNormalAt(tempIntersect);
                material = sphere.material;
				intersect = tempIntersect;
			}
        }
    }
    
	memcpy(intersectOut, &intersect, sizeof(Vec3Df));
	memcpy(normalOut, &normal, sizeof(Vec3Df));
	memcpy(materialOut, &material, sizeof(Material));
    return dist != FLT_MAX;
}

/*
	Calculates the lambertian shading color.
*/
Vec3Df diffuseOnly(const Vec3Df & vertexPos, Vec3Df & normal, Material* material, Vec3Df lightpos){
	Vec3Df Diffuse = BLACK;
	normal.normalize();
	lightpos.normalize();
	//calculate diffuse color for current light source.
	Diffuse += material->Kd() * max(Vec3Df::dotProduct(normal, lightpos), 0.0f);
	
	return Diffuse;
}

/*
	Calculates the Blinn-Phong Shading Specularity color.
*/
Vec3Df blinnPhongSpecularOnly(const Vec3Df & vertexPos, Vec3Df & normal, Material* material, Vec3Df lightpos){
	Vec3Df Specularity = BLACK;
	Vec3Df V = MyCameraPosition - vertexPos;//calculate view vector
	normal.normalize();
	V.normalize();

	Vec3Df L = lightpos - vertexPos;//calculate light vector
		
	L.normalize();
		
	//Calculate the half vector between the light vector and the view vector.
	Vec3Df H = V + L;
	H.normalize();

	//Calc specular term, if normal is > 90 degrees away from light then use 0.
	float SpecularTerm = max(Vec3Df::dotProduct(H, normal), 0.0f);
	SpecularTerm = pow(SpecularTerm, material->Ns());
	//printf("ks = %i\n", material->Ks());
	Specularity += material->Ks() * SpecularTerm;
	

	return Specularity;
}


/*
	Determens if a triangle is between a point and a light position.
	This is used to check if a point is in a shadow.
	returns true if the point is in shadow.
	returns false if the point is not in shadow.
*/
bool isShadow(Vec3Df intersection, Vec3Df light_pos){
	Vec3Df intersectOut, normalOut;
    Material materialOut;
	//adding offset for depth bias
	intersection = intersection + Vec3Df(0.1, 0.1, 0.1);
    return Shadows // Shadows should be enabled, otherwise false
            && intersect(Ray(intersection, light_pos), &intersectOut, &normalOut, &materialOut) // Find object between light and intersection
            && (!( materialOut.has_Tr() && materialOut.Tr() < 1.0f )); // If intersection, but material of intersect is transparent, then false
}

/*
	Function to add offsets towards the direction of the towardspoint
*/
void addOffset(Vec3Df* point, Vec3Df* towardsPoint){
	Vec3Df vector = (*towardsPoint) - (*point);
	vector.normalize();
	vector *= 0.01;
	*point += vector;
}

/*
	Function to calculate the reflection Vector and then trace it further recursively
*/

Vec3Df reflection(Vec3Df ray, const Vec3Df & vertexPos, Vec3Df & normal, int lvl){
	ray.normalize();
	//calculate reflection vector
	Vec3Df R = ray -(2 * Vec3Df::dotProduct(normal, ray)*normal);
	Vec3Df point = vertexPos;
	Vec3Df dest = vertexPos + R;
	addOffset(&point, &dest);
	return trace(point, dest, lvl);
}

/*
	Calculate refraction vector and trace it further
*/
Vec3Df refraction(Vec3Df ray, const Vec3Df & vertexPos, Vec3Df & normal, Material* material, int lvl){
	float ni = material->Ni();
	ray.normalize();
	float check = Vec3Df::dotProduct(ray, (normal));
	//check if ray is going inside the material or coming out of it
	if (check < 0){
		float angle = acosf(check);
		//hardcoded reflection angle to simulate some sort of total reflection at a low angle
		if (angle <= 2 && angle > 0){
			return material->Ks() * reflection(ray, vertexPos, normal, lvl + 1);
		}
		float nr = 1 / ni;
		float root = 1 - powf(nr, 2)*(1 - powf(Vec3Df::dotProduct(normal, ray), 2));
		//check if there is not a total internal reflection
		if (root >= 0.0){
			root = sqrt(root);
			//calculate transition Vector
			Vec3Df T = nr*(ray-Vec3Df::dotProduct(normal, ray)*normal)- normal * root;
			Vec3Df point = vertexPos;
			Vec3Df dest = vertexPos + T;
			addOffset(&point, &dest);
			return (1-material->Tr()) * trace(point, dest, lvl+1);
		}
	}
	else{
		float nr = ni;
		float root = 1 - powf(nr, 2)*(1 - powf(Vec3Df::dotProduct((-normal), ray), 2));
		//check if there is not a total internal reflection
		if (root >= 0.0){
			root = sqrt(root);
			//calculate transition Vector
			Vec3Df T = nr*(ray - Vec3Df::dotProduct((-normal), ray)*(-normal)) - (-normal) * root;
			Vec3Df point = vertexPos;
			Vec3Df dest = point + T;
			addOffset(&point, &dest);
			return (1 - material->Tr()) * trace(point, dest, lvl + 1);
			
		}
	}
	return Vec3Df(0,0,0);
}

/*
	Shading function that returns the resulting color
*/
Vec3Df shade(Vec3Df ray, const Vec3Df & vertexPos, Vec3Df & normal, Material* material, int lvl){
	Vec3Df pixelcolor = BLACK;
	if (Ambient && material->has_Ka()){
		//add ambient color
		pixelcolor += material->Ka();
	}
	//loop for going through all light sources
	for (unsigned int i = 0; i < MyLightPositions.size(); i++){
		Vec3Df L = MyLightPositions[i];
		//check if poin is in shadow
		if (!isShadow(vertexPos, L))
		{
            float tr = material->has_Tr() ? material->Tr() : 1;
			if (Diffuse && material->has_Kd()){
				//add diffuse color
				pixelcolor += tr * diffuseOnly(vertexPos, normal, material, L);
			}
			if (Specular && material->has_Ks() && material->has_Ns()){
				//add Specular part
				pixelcolor += tr * blinnPhongSpecularOnly(vertexPos, normal, material, L);
			}
		}
	}
	if (Refraction  && lvl < max_lvl && material->has_Tr() && material->Tr()<1 ){
		//calculate refraction
		pixelcolor += refraction(ray, vertexPos, normal, material, lvl +1);
	}
	else if (Reflection && material->has_Ks() && lvl < max_lvl) {
		//calculate reflection we take Ks() as same aproximate reflection coefficient. So a mirror wpuld have Ks(1,1,1)
		pixelcolor += material->Ks() * reflection(ray, vertexPos, normal, lvl + 1);
	}

	return pixelcolor;

}

/*
	Returns the Material object corresponding to the triangle with index index.
*/
Material getMaterial(int index){
	int materialIndex = MyMesh.triangleMaterials[index];
	return MyMesh.materials[materialIndex];
}

/*
	Trace function that is called recursively
*/
Vec3Df trace(const Vec3Df & origin, const Vec3Df & dest, int lvl){
	Vec3Df pixelcolor = BLACK;	Vec3Df intersectOut, normalOut;
    Material materialOut;
    
    
	//check for intersection
    if(intersect(Ray(origin, dest), &intersectOut, &normalOut, &materialOut)) {
        Vec3Df ray = origin - dest;
        
        if (DebugMode){
            o.push_back(origin);
            d.push_back(intersectOut);
        }
        
        if(ShowNormalMode) {
            return normalOut;
        } else if (IntersectMode) {
            return Vec3Df(1,1,1);
        }
        
        pixelcolor = shade(ray, intersectOut, normalOut, &materialOut, lvl);
    }
    
    return pixelcolor;
}


//return the color of your pixel.
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest)
{
	Vec3Df pixelcolor = BLACK;
	int lvl = 0;
	pixelcolor = trace(origin, dest, lvl);
	return pixelcolor;
}

/*
Draws the lines representing a light ray
*/
void drawRayDebugger(){
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	for (int i = 0; i < o.size(); i++){
		glColor3f(1, 0, 0);
		glVertex3f(o[i][0], o[i][1], o[i][2]);
		glColor3f(0, 1, 0);
		glVertex3f(d[i][0], d[i][1], d[i][2]);
	}
	glEnd();
}

void yourDebugDraw()
{
	//draw open gl debug stuff
	//this function is called every frame

	//as an example: 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);

	drawRayDebugger();

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
			Diffuse = !Diffuse;
			break;
		case '3':
			Specular = !Specular;
			break;
		case '4':
			Reflection = !Reflection;
			break;
		case '5':
			Shadows = !Shadows;
			break;
		case '6':
			Refraction = !Refraction;
			break;

		case '+':
			pixelfactorX++;
			pixelfactorY++;

			break;
		case '-':
			pixelfactorX--;
			pixelfactorY--;
			if (pixelfactorX < 1)
				pixelfactorX = 1;
			if (pixelfactorY < 1)
				pixelfactorY = 1;
			break;
		case '0':
			DebugMode = !DebugMode;
			cout << "Debug Mode:\n 0 to enable / disable debug mode\n d to shoot & draw a ray trace.\n c to clear ray trace history.\n";
			break;

		case 'd': //Shoot ray to mouse position
			if (DebugMode){
				Vec3Df origin, dest, intersectOut, normalOut;
                Material materialOut;
				produceRay(x, y, origin, dest);

				int i = intersect(Ray(origin, dest), &intersectOut, &normalOut, &materialOut);

				//add origin and intersection to vectors
				o.push_back(origin);
				d.push_back(intersectOut);

				Vec3Df pixelcolor = BLACK;
				int lvl = 0;
				pixelcolor = trace(origin, dest, lvl);

				char buffer[128];
				cout << "Ray trace color = " << pixelcolor.toString(buffer, sizeof(buffer)) << endl;
			}
			return;
        case 'n': // Normal mode: Don't shade, but show normals
            if(!ShowNormalMode) IntersectMode = false;
            ShowNormalMode = !ShowNormalMode;
            break;
        case 'i': // Intersect mode: Don't shade, but show intersections
            if(!IntersectMode) ShowNormalMode = false;
            IntersectMode = !IntersectMode;
            break;
		case 'c'://clear ray debugger
			o.clear();
			d.clear();
			cout << "Ray trace history cleared\n";
			return;

		case 'w':
			//Sets the openGl fill mode to lines.
			WireFrame = !WireFrame;
			if (WireFrame){
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				cout << "WireFrame enabled\n";
			}
			else{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				cout << "WireFrame disabled\n";
			}
			break;

		default:
			break;
	}

	cout << endl << "------SETTINGS------" << endl
		<< "Ammbient " << (Ambient ? "ON" : "OFF") << endl
		<< "Diffuse " << (Diffuse ? "ON" : "OFF") << endl
		<< "Specular " << (Specular ? "ON" : "OFF") << endl
		<< "Reflection " << (Reflection ? "ON" : "OFF") << endl
		<< "Shadow " << (Shadows ? "ON" : "OFF") << endl
		<< "Refraction " << (Refraction ? "ON" : "OFF") << endl
		<< "pixelfactorX = " << pixelfactorX << endl
		<< "pixelfactorY = " << pixelfactorY << endl
		<< "DebugMode " << (DebugMode ? "ON" : "OFF") << endl
		<< "WireFrame " << (WireFrame ? "ON" : "OFF") << endl
        << "IntersectMode " << (IntersectMode ? "ON" : "OFF") << endl
        << "ShowNormals " << (ShowNormalMode ? "ON" : "OFF") << endl
		<< "--------------------" << endl;

	// do what you want with the keyboard input t.
	// x, y are the screen position


	cout << " pressed! The mouse was in location " << x << "," << y << "!" << std::endl;
}

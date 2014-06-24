#ifndef RAYTRACING_Hjdslkjfadjfasljf
#define RAYTRACING_Hjdslkjfadjfasljf
#include <vector>
#include "mesh.h"
//Welcome to your assignment...
//this is an important file, raytracing.cpp is what you need to fill out

extern Mesh MyMesh; //Main mesh
extern std::vector<Vec3Df> MyLightPositions;
extern Vec3Df MyCameraPosition; //currCamera
extern unsigned int WindowSize_X;//window resolution width
extern unsigned int WindowSize_Y;//window resolution height
extern unsigned int RayTracingResolutionX;  // largeur fenetre
extern unsigned int RayTracingResolutionY;  // largeur fenetre
extern unsigned int pixelfactorX;
extern unsigned int pixelfactorY;

//use this function for any preprocessing of the mesh.
void init(char* fileName);

//you can use this function to transform a click to an origin and destination
//the last two values will be changed. There is no need to define this function.
//it is defined elsewhere
void produceRay(int x_I, int y_I, Vec3Df & origin, Vec3Df & dest);

//function to get material of the triangle with index index
Material getMaterial(int index);

//function to trace the rays (needed for recursion)
Vec3Df trace(const Vec3Df & origin, const Vec3Df & dest, int lvl);

//your main function to rewrite
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest);

//a function to debug --- you can draw in OpenGL here
void yourDebugDraw();

//want keyboard interaction? Here it is...
void yourKeyboardFunc(char key, int x, int y);

void calculateNormals();

bool rayIntersectRectangle(Vec3Df R[], Vec3Df T[], Vec3Df* intersectOut);
bool rayIntersectRectangle(Vec3Df R[], Vec3Df v0, Vec3Df v1, Vec3Df v2, Vec3Df* intersectOut);
float rayIntersectBox(Vec3Df ray[], Vec3Df loc, float w, float l, float h, Vec3Df* returnIntersect);

int getTeller();
#endif
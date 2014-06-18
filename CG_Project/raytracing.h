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

bool Ambient = false;
bool Reflection = false;
bool Shadows = false;
bool Specular = false;


//use this function for any preprocessing of the mesh.
void init();

//you can use this function to transform a click to an origin and destination
//the last two values will be changed. There is no need to define this function.
//it is defined elsewhere
void produceRay(int x_I, int y_I, Vec3Df & origin, Vec3Df & dest);


//your main function to rewrite
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest);

//a function to debug --- you can draw in OpenGL here
void yourDebugDraw();

//want keyboard interaction? Here it is...
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
};

#endif
//
//  Sphere.h
//  CG_Project
//
//  Created by Jan-Willem Gmelig Meyling on 19/06/14.
//  Copyright (c) 2014 TU Delft. All rights reserved.
//

#ifndef CG_Project_Sphere_h
#define CG_Project_Sphere_h
#include "Vec3D.h"

class Sphere {
    Vec3Df center;
    float radius;
    Material material;
    
    public :
    
    inline Sphere () {
        center = Vec3Df(0,0,0);
        radius = 1;
        material = Material();
    }
    
    inline Sphere (Vec3Df _center, float _radius, Material _material) {
        center = _center;
        radius = _radius;
        material = _material;
    }
    
    virtual Vec3Df getNormalAt(Vec3Df& intersection) {
        Vec3Df result = intersection - center;
        result.normalize();
        return result;
    }
    
    virtual float findIntersection(Vec3Df& origin, Vec3Df& destination) {
        return -1;
    }
    
}

#endif

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
#include <math.h> 

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
        Vec3Df ray = center - origin;
        double eps = 1e-4;
        double dot = Vec3Df::dotProduct(ray, destination);
        double det = dot * dot - Vec3Df::dotProduct(center, center) + radius * radius;
        
        if ( det < 0 ) {
            return 0;
        }
        else {
            double t;
            return (t=dot-det)>eps ? t : ((t=dot+det)>eps ? t : 0);

        }
    }
    
}

#endif

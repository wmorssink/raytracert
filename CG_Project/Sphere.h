//
//  sphere.h
//  CG_Project
//
//  Created by Jan-Willem Gmelig Meyling on 24/06/14.
//  Copyright (c) 2014 TU Delft. All rights reserved.
//

#ifndef CG_Project_sphere_h
#define CG_Project_sphere_h

#include "Vec3D.h"
#include "mesh.h"
#include "ray.h"

class Sphere {
    
public:
    
    Vec3Df center;
    float radius;
    Material material;
    
    Sphere() {
        center = Vec3Df(0.0f, 0.0f, 0.0f);
        radius = 1.0f;
        
        material = Material();
        material.set_Kd(0.8, 0.8, 0.8);
        material.set_Ks(0.2, 0.2, 0.2);
        material.set_Ka(0.0, 0.0, 0.0);
        material.set_Ns(1);
        material.set_Ni(1);
    }
    
    Sphere(const Vec3Df &_origin, float _radius) {
        center = _origin; radius = _radius;
        
        
        material = Material();
        material.set_Kd(0.8, 0.8, 0.8);
        material.set_Ks(0.2, 0.2, 0.2);
        material.set_Ka(0.0, 0.0, 0.0);
        material.set_Ns(1);
        material.set_Ni(1);
    }
    
    Sphere(const Vec3Df &_origin, float _radius, const Material &_material) {
        center = _origin; radius = _radius; material = _material;
    }
    
    inline Vec3Df getNormalAt(const Vec3Df &intersection) {
        Vec3Df normal = center - intersection;
        normal.normalize();
        return normal;
    }
    
    inline bool intersect(const Ray &ray, Vec3Df* intersection) {
        Vec3Df direction = ray.origin - ray.destination;
        Vec3Df op = ray.origin - center; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
        
        float eps=1e-4;
        float b = 2 * Vec3Df::dotProduct(op, direction);
        float a = Vec3Df::dotProduct(direction, direction);
        float c = Vec3Df::dotProduct(op, op) - (radius * radius);
        float disc = b*b - 4*a*c;
        
        if ( disc <  0 ) {
            return false;
        } else {
            disc = sqrt(disc);
            float t = (t=-b-disc)>eps ? t : ((t=-b+disc)>eps ? t : 0);
            Vec3Df I = ray.destination + t * direction;
            memcpy(intersection, &I, sizeof(Vec3Df));
            return true;
        }
    }
    
};

#endif

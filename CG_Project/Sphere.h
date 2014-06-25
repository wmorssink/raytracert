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
#include "material.h"
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
        Vec3Df normal =  intersection - center;
        normal.normalize();
        return normal;
    }
    
    inline bool intersect(const Ray &ray, Vec3Df* intersectOut) {
        Vec3Df direction = ray.destination - ray.origin; // ray direction vector
        direction.normalize(); // We need the unit vector for the direction
        Vec3Df op = center - ray.origin; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
        double t, eps=1e-4, b=Vec3Df::dotProduct(op, direction), det= b * b- Vec3Df::dotProduct(op, op) + radius*  radius;
        if (det<0) return false; // There are no intersections, return false
        det=sqrt(det); // Take the square root of the determinant
        t = (t=b-det) > eps ? t : ((t = b + det) > eps ? t : 0);
        if (t <= 0) return false; // Point is  behind camera
        Vec3Df intersection = ray.origin + direction * t;
        memcpy(intersectOut, &intersection, sizeof(Vec3Df));
        return true;
    }
    

};

#endif

//
//  pane.h
//  CG_Project
//
//  Created by Jan-Willem Gmelig Meyling on 23/06/14.
//  Copyright (c) 2014 TU Delft. All rights reserved.
//

#ifndef CG_Project_pane_h
#define CG_Project_pane_h
#include "Vec3D.h"
#include "mesh.h"
#include "ray.h"

class Plane {
    public :
    Vec3Df point, normal;
    Material material;
    
    Plane() {
        point = Vec3Df(-2,0,0);
        normal = Vec3Df(0,1,0);
        
        material = Material();
        material.set_Kd(0.2, 0.2, 0.2);
        material.set_Ks(0.7, 0.7, 0.7);
        material.set_Ka(0.1, 0.1, 0.1);
        material.set_Ns(1.0);
        material.set_Ni(1.0);
    }
    
    Plane(const Vec3Df &_point, const Vec3Df &_normal, const Material &_material) {
        point = _point;
        normal = _normal;
        material = _material;
    }
    
    inline bool intersect(const Ray &ray, Vec3Df* intersection) {
        Vec3Df direction = ray.origin - ray.destination;
        // (2)	dot(point_in_plane,plane_normal) = distance_to_origin
        // or	dot(P,N) = k
        float k = Vec3Df::dotProduct(point, normal);
        // t=(k-dot(C,N))/dot(D,N)
        float dn = Vec3Df::dotProduct(direction, normal);
        if(dn > 0) {
            float t = (k - Vec3Df::dotProduct(ray.origin, normal)) / dn;
            Vec3Df i = ray.origin + t * direction;
            memcpy(intersection, &i, sizeof(Vec3Df));
            return true;
        }
        return false;
    }
    
};

#endif

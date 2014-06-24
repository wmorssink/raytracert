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
    
    inline bool intersect(const Vec3Df &origin, const Vec3Df &destination, Vec3Df* intersection) {
        Vec3Df direction = origin - destination;
        // (2)	dot(point_in_plane,plane_normal) = distance_to_origin
        // or	dot(P,N) = k
        float k = Vec3Df::dotProduct(point, normal);
        // t=(k-dot(C,N))/dot(D,N)
        float dn = Vec3Df::dotProduct(direction, normal);
        if(dn > 0) {
            float t = (k - Vec3Df::dotProduct(origin, normal)) / dn;
            Vec3Df i = origin + t * direction;
            memcpy(intersection, &i, sizeof(Vec3Df));
            return true;
        }
        return false;
    }
    
};

class Rectangle {
    public:
    Vec3Df a, b, c, d, normal;
    Material material;
    
    Rectangle() {
        a = Vec3Df(0,0,0);
        b = Vec3Df(0,1,0);
        c = Vec3Df(1,1,0);
        d = Vec3Df(1,0,0);
        
        Vec3Df V1 = b - a, V2 = d - a;
        normal = V1 - V2;
        
        material = Material();
        material.set_Kd(0.2, 0.2, 0.2);
        material.set_Ks(0.7, 0.7, 0.7);
        material.set_Ka(0.1, 0.1, 0.1);
        material.set_Ns(1.0);
        material.set_Ni(1.0);
    }
    
    Rectangle(const Vec3Df &_a, const Vec3Df &_b, const Vec3Df &_c, const Vec3Df &_d, const Material &_material) {
        a = _a; b = _b; c = _c; d = _d; material = _material;
    }
    
    inline bool intersect(const Vec3Df &origin, const Vec3Df &destination, Vec3Df* intersection) {
        const float SMALL_NUM = 0.00001f;
        // R[0] = add(R[0], R[1]);
        Vec3Df u, v, n, dir, w0, w;
        float r, p, q;
        
        // get triangle edge vectors and plane normal
        u = b - a;
        v = d - a;
        n = Vec3Df::crossProduct(u, v); // cross product
        if (n[0] == 0 && n[1] == 0 && n[2] == 0) // triangle is degenerate
            return false; // do not deal with this case
        dir = origin - destination; // R[1] - R[0]; // ray direction vector
        w0 = destination - a;
        q = Vec3Df::dotProduct(n, dir);
        p = -Vec3Df::dotProduct(n, w0);
        if (abs(q) < SMALL_NUM) { // ray is parallel to triangle plane
            /*
             * if (a == 0) // ray lies in triangle plane else // ray disjoint
             * from plane
             */
            return false;
        }
        
        // get intersect point of ray with triangle plane
        r = p / q;
        if (r < 0) { // ray goes away from triangle
            return false; // => no intersect
        }
        // for a segment, also test if (r > 1.0) => no intersect
        
        Vec3Df I = destination + r * dir; // intersect point of ray and
        // plane
        // is I inside T?
        float uu, uv, vv, wu, wv, D;
        uu = Vec3Df::dotProduct(u, u);
        uv = Vec3Df::dotProduct(u, v);
        vv = Vec3Df::dotProduct(v, v);
        w = I - a;
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
        
        memcpy(intersection, &I, sizeof(Vec3Df));
        return true; // I is in T
    }
};

class Box {
    
};

class Sphere {
    
    public:
    
    Vec3Df center;
    float radius;
    Material material;
    
    Sphere() {
        center = Vec3Df(0,0,0);
        radius = 0.5f;
        
        material = Material();
        material.set_Kd(0.2, 0.2, 0.2);
        material.set_Ks(0.7, 0.7, 0.7);
        material.set_Ka(0.1, 0.1, 0.1);
        material.set_Ns(1.0);
        material.set_Ni(1.0);
    }
    
    Sphere(const Vec3Df &_origin, float _radius, const Material &_material) {
        center = _origin; radius = _radius; material = _material;
    }
    
    inline bool intersect(const Vec3Df &origin, const Vec3Df &destination, Vec3Df* intersection) {
        Vec3Df direction = origin - destination;
        Vec3Df op = center - origin; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
        float eps=1e-4, b = Vec3Df::dotProduct(op, destination), det = b*b - Vec3Df::dotProduct(op, op) + radius * radius;
        if (det<0) return false; else det=sqrt(det);
        float t = (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0);
        Vec3Df I = origin + t * direction;
        memcpy(intersection, &I, sizeof(Vec3Df));
        return true;
    }
    
};

#endif

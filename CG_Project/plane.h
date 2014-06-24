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

struct Ray {
    Vec3Df origin, destination;

    Ray(Vec3Df _origin, Vec3Df _destination)
        : origin(_origin), destination(_destination) {}
};


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
    
    inline bool intersect(const Ray &ray, Vec3Df* intersection) {
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
        dir = ray.origin - ray.destination; // R[1] - R[0]; // ray direction vector
        w0 = ray.destination - a;
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
        
        Vec3Df I = ray.destination + r * dir; // intersect point of ray and
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
     */
};

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

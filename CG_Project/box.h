//
//  box.h
//  CG_Project
//
//  Created by Jan-Willem Gmelig Meyling on 24/06/14.
//  Copyright (c) 2014 TU Delft. All rights reserved.
//

#ifndef CG_Project_box_h
#define CG_Project_box_h

#include "Vec3D.h"
#include "mesh.h"
#include "ray.h"

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


#endif

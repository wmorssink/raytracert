//
//  ray.h
//  CG_Project
//
//  Created by Jan-Willem Gmelig Meyling on 24/06/14.
//  Copyright (c) 2014 TU Delft. All rights reserved.
//

#ifndef CG_Project_ray_h
#define CG_Project_ray_h
#include "Vec3D.h"

struct Ray {
    Vec3Df origin, destination, direction;
    
    Ray(Vec3Df _origin, Vec3Df _destination)
        : origin(_origin), destination(_destination) {
            direction = destination - origin;
            direction.normalize();
    }
    
};

#endif

#ifndef RAY_H
#define RAY_H

#include "vec3.h" 

class Ray {
public:
    Vec3 origin;
    Vec3 direction;

    Ray(Vec3& origin, Vec3& direction) : origin(origin), direction(direction) {}

    Vec3 pointAt(double t) const {
        return origin + direction * t;
    }
};

#endif
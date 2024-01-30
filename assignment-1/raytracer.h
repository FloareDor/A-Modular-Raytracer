#ifndef RAYTRACER_H
#define RAYTRACER_H

#include<cmath>
#include "vec3.h"

class Ray {
public:
    Vec3 origin;
    Vec3 direction;

    Ray(const Vec3& origin, const Vec3& direction) : origin(origin), direction(direction) {}

    Vec3 pointAt(double t) const {
        return origin + direction * t;
    }
};

double intersect_Sphere(const Ray& ray, const Vec3 center, double radius) {
    // quadratic equation
    double a = ray.direction.dot(ray.direction);
    double b = (ray.direction * 2).dot(ray.origin - center);
    double c = (ray.origin - center).dot((ray.origin - center)) - (radius * radius);

    double discriminant = (b * b) - (4 * a * c);

    double t1 = (-b - (sqrt(discriminant))) / (2 * a);

	// we don't care about negative roots because they are not visible through the viewing plane
    if(discriminant >= 0){
        return t1;
    }
    return -1.0;
}

Color diffuse_shading(double intensity, double coefficient, Vec3 normal, Vec3 lightDirection ){
    return Color(0, 0, 0);
}

Color traceRay(const Ray& ray) {
    Vec3 center = Vec3(0, 0, -1);
    double t1 = intersect_Sphere(ray, center, 0.33);
    if(t1 > 0.0)
    {
        Vec3 intersectionPoint = ray.pointAt(t1);
        Vec3 normal = (intersectionPoint - center).unit_vector();
        double Ld = 0.5 * 1 * std::max(1.0, normal.dot(Vec3(2, 1, 1)));
        return Color(0, 108, 69) * Ld;
        // return Color(normal.x + 1, normal.y + 1, normal.z + 1) * 100 * Ld;
        // return Color(0, 108, 0);
    }
    return Color(0, 0, 0);
}

#endif
#ifndef RAYTRACER_H
#define RAYTRACER_H

#include<cmath>
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

double intersect_Sphere(Ray& ray, Vec3 center, double radius) {
    // quadratic equation
    double a = ray.direction.dot(ray.direction);
    double b = (ray.direction * 2).dot(ray.origin - center);
    double c = (ray.origin - center).dot((ray.origin - center)) - (radius * radius);

    double discriminant = (b * b) - (4 * a * c);

    double t1 = (-b - (sqrt(discriminant))) / (2 * a);

    // std::cout << "Centre: " << std::endl;
    // center.print();

    // std::cout << "RayOrigin: " << std::endl;
    // ray.origin.print();

    // std::cout << "RayDirection: " << std::endl;
    // ray.direction.print();

    // we don't care about negative roots because they are not visible through the viewing plane
    if(discriminant >= 0){
        // std::cout << "OUT: " << t1 << std::endl;
        return t1;
    }
    return -1.0;
}

double diffuse_shading(double coefficient, double intensity, Vec3 normal, Vec3 lightDirection ){
    double Ld = coefficient * intensity * std::max(1.0, normal.dot(lightDirection));
    return Ld;
}

Color traceRay(Ray& ray) {
    Vec3 sphere_center = Vec3(0, 2, -2);
    double t1 = intersect_Sphere(ray, sphere_center, 0.33);

    Vec3 light_source_pos = Vec3(2, 1, 0.5);

    if(t1 > 0.0)
    {
        Vec3 intersectionPoint = ray.pointAt(t1);
        Vec3 normal = (intersectionPoint - sphere_center).unit_vector();
        double Ld = 0.5 * 1 * std::max(1.0, normal.dot(light_source_pos - intersectionPoint));
        return Color(0, 108, 69) * Ld;
        // return Color(normal.x + 1, normal.y + 1, normal.z + 1) * 100 * Ld;
        // return Color(0, 108, 0);
    }
    return Color(0, 0, 0);
}

#endif
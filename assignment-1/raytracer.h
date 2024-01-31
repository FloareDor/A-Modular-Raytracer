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

    // we don't care about negative roots because they are not visible through the viewing plane
    if(discriminant >= 0){
        // std::cout << "OUT: " << t1 << std::endl;
        return t1;
    }
    return -1.0;
}


double diffuse_shading(double coefficient, double intensity, Vec3 normal, Vec3 lightDirection ){
    double Ld = coefficient * intensity * std::max(0.1, normal.dot(lightDirection));
    return Ld;
}

double blinnPhong_shading(double coefficient, double intensity, Vec3 normal, Vec3 VL, Vec3 VE, double n){
    // bisector
    Vec3 VH = (VL + VE) / (VL + VE).magnitude();
    double Ls = coefficient * intensity * std::pow(std::max(0.1, normal.dot(VH)), n);
    return Ls;
}

Color traceRay(Ray& ray, Vec3 lightsource_pos) {


    Vec3 sphere1_center = Vec3(-0.5, 0, -1);
    double sphere1_radius = 0.33;

    Vec3 sphere2_center = Vec3(0.4, 0, -1);
    double sphere2_radius = 0.33;

    double t1 = intersect_Sphere(ray, sphere1_center, sphere1_radius);
    double t2 = intersect_Sphere(ray, sphere2_center, sphere2_radius);

    if(t1 > 0.0)
    {
        Vec3 intersectionPoint = ray.pointAt(t1);
        Vec3 normal = (intersectionPoint - sphere1_center).unit_vector();
        Vec3 VL = (lightsource_pos - intersectionPoint).unit_vector();
        Vec3 VE = (ray.origin - intersectionPoint).unit_vector();

        double La = 10;
        double Ld = 65 * std::max(0.1, normal.dot(VL));
        double Ls = blinnPhong_shading(50,1, normal, VL, VE, 9);

        double L = La + Ld + Ls;
        // std::cout << La << ',' << Ld << ',' << Ls << std::endl;
        // Color shadeColor = Color(normal.x + 1, normal.y + 1, normal.z + 1) * Ld * 30;
        Color shadeColor = Color(1,2,1) * L;
        return shadeColor;
        // How do we apply the light color on top of the sphere???
        // return Color(normal.x + 1, normal.y + 1, normal.z + 1) * 100 * Ld;
        // return Color(0, 108, 0);
    }

    if(t2 > 0.0)
    {
        Vec3 intersectionPoint = ray.pointAt(t2);
        Vec3 normal = (intersectionPoint - sphere2_center).unit_vector();
        Vec3 VL = (lightsource_pos - intersectionPoint).unit_vector();
        double La = 10;
        double Ld =  65 * std::max(0.1, normal.dot(VL));
        double L = La + Ld;
        // Color shadeColor = Color(normal.x + 1, normal.y + 1, normal.z + 1) * Ld * 30;
        Color shadeColor = Color(1,2,1) * L;
        return shadeColor;
        // How do we apply the light color on top of the sphere???
        // return Color(normal.x + 1, normal.y + 1, normal.z + 1) * 100 * Ld;
        // return Color(0, 108, 0);
    }

    return Color(0, 0, 0);
}

#endif
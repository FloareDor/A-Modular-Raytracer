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
        Color shadeColor = Color(1,2,1) * L;
        shadeColor = shadeColor.clamp(0.0, 255.0);
        return shadeColor;
    }else if(t2 > 0.0)
    {
        Vec3 intersectionPoint = ray.pointAt(t2);
        Vec3 normal = (intersectionPoint - sphere2_center).unit_vector();
        Vec3 VL = (lightsource_pos - intersectionPoint).unit_vector();
        double La = 10;
        double Ld =  65 * std::max(0.1, normal.dot(VL));

        double L = La + Ld;
        Color shadeColor = Color(1,2,1) * L;
        shadeColor = shadeColor;
        return shadeColor;
    }else{
        Vec3 plane_normal = Vec3(0, -0.2, 0); // Normal vector of the plane (pointing upwards)
        double plane_distance = 0.5; // Distance from the origin along the normal vector

        double t_plane = (plane_distance - ray.origin.y) / ray.direction.y;

        if (t_plane > 0) {
            Vec3 intersectionPoint = ray.pointAt(t_plane);
            Vec3 VL = (lightsource_pos - intersectionPoint);
            Ray reverse_lightray(intersectionPoint, VL);
            if(intersect_Sphere(reverse_lightray, sphere1_center, sphere1_radius) > 0.0 || intersect_Sphere(reverse_lightray, sphere2_center, sphere2_radius) > 0.0){
                return Color(33,108,132) * 0.2;
            }

            // Checkered pattern
            // bool is_blue = ((int)(intersectionPoint.x * 10) % 2 == 0) ^ ((int)(intersectionPoint.z * 10) % 2 == 0);
            // Color plane_color = is_blue ? Color(0, 0, 1)*100 : Color(0.5, 0.5, 0.5)*100;

            return Color(33,108,132);
        }
    }

    return Color(0, 0, 0);
}

#endif
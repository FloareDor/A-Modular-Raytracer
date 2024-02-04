#ifndef RAYTRACER_H
#define RAYTRACER_H

#include<cmath>
#include "vec3.h"
#include "shader.h"


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

class Sunlight {
public:
    Vec3 position;
    double intensity;

    Sunlight(const Vec3& position, double intensity)
        : position(position), intensity(intensity) {}

};

class Obj {
public:
    Color color;

    Obj() : color(Color(0, 0, 0)) {}

    virtual double hit(const Ray& ray) const = 0;

    virtual Color objColor() const = 0;

    virtual Color shade(const Ray &ray, Sunlight lightsource, double t1) const = 0;
};

class Sphere : public Obj {
public:
    Vec3 center;
    double radius;
    Color color;
    Shader shader;

    // Constructor with arguments
    Sphere(const Vec3& center, double radius, const Color& color, const Shader& shader)
        : center(center), radius(radius), color(color), shader(shader) {}

    double hit(const Ray& ray) const override {
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

    Color objColor() const override {
        return color;
    }

    Color shade(const Ray& ray, Sunlight lightsource, double t1)const override{
        Vec3 intersectionPoint = ray.pointAt(t1);
        Vec3 normal = (intersectionPoint - center).unit_vector();
        Vec3 VL = (lightsource.position - intersectionPoint).unit_vector();
        Vec3 VE = (ray.origin - intersectionPoint).unit_vector();
        // Shader shader;
        Color postShadingColor = shader.calculateShading(lightsource.intensity, color, intersectionPoint, normal, VL, VE);
        // std::cout << postShadingColor << std::endl;
        return postShadingColor;
    }

};

class Plane : public Obj {
public:
    Vec3 normal;
    double height;
    Color color;
    Shader shader;

    // Constructor with arguments
    Plane(const Vec3& normal, double height, const Color& color, const Shader& shader)
        : normal(normal), height(height), color(color), shader(shader) {}

    double hit(const Ray& ray) const override  {
        double t = (height - ray.origin.y) / ray.direction.y;
        if(t>0.0){
            return t;
        }

        return -1.0;
    }

    Color objColor() const override {
        return color;
    }

    Color shade(const Ray& ray, Sunlight lightsource, double t1)const override{
        
        Color postShadingColor = shader.ambientShading(lightsource.intensity, color/10);
        return postShadingColor;
    }
   
};

class Objects {
private:
    std::vector<std::shared_ptr<Obj>> objects;

public:
    Sunlight lightsource;

    Objects() : lightsource(Sunlight(Vec3(0, 0, 0),0.0)) {}

    void add(const std::shared_ptr<Obj>& obj) {
        objects.push_back(obj);
    }

    Color hit_anything(const Ray& ray, double t_max) const {
        bool hit_anything = false;

        double closest_t = t_max;
        std::shared_ptr<Obj> closest_object;

        for (const auto& object : objects) {
            double t = object->hit(ray);
            if (t > 0.0 && t < closest_t){
                closest_t = t;
                closest_object = object;
                hit_anything = true;
            }
        }
        if(hit_anything == true){

            Color objColor = closest_object->objColor();
            Color shadedColor = closest_object->shade(ray, lightsource, closest_t);
            return shadedColor;

        }else{
            return Color(0, 0, 0);
        }
    }

};

Color castRay(Ray& ray, Objects world, Vec3 lightsource_pos){
    // Color t = world.hit_anything(ray, 1000);
    return world.hit_anything(ray, 1000);
}

Color traceRay(Ray& ray, Vec3 lightsource_pos, bool reflected) {

    Vec3 sphere1_center = Vec3(0, 0, -1.1);
    double sphere1_radius = 0.8;

    Vec3 sphere2_center = Vec3(0.9, 0.5, -0.2);
    double sphere2_radius = 0.3;

    Vec3 plane_normal = Vec3(0, 1, 0); // Normal vector of the plane (pointing upwards)
    double plane_distance = 0.8; // Distance from the origin along the normal vector
    double t_plane = (plane_distance - ray.origin.y) / ray.direction.y;

    double t1 = intersect_Sphere(ray, sphere1_center, sphere1_radius);
    double t2 = intersect_Sphere(ray, sphere2_center, sphere2_radius);

    if(t1 > 0.0 && (t2 <= 0.0 || t1 < t2))
    {
        Vec3 intersectionPoint = ray.pointAt(t1);
        Vec3 normal = (intersectionPoint - sphere1_center).unit_vector();
        Vec3 VL = (lightsource_pos - intersectionPoint).unit_vector();
        Vec3 VE = (ray.origin - intersectionPoint).unit_vector();

        double La = 15;
        double Ld = 8 * 10 * std::max(0.1, normal.dot(VL));
        double Ls = blinnPhong_shading(2.5, 10, normal, VL, VE, 35);

        double L = La + Ld + Ls;
        Color shadeColor = Color(1,2,1) * L;
        shadeColor = shadeColor.clamp(0.0, 255.0);
        Ray reverse_lightray(intersectionPoint, VL);

        if(intersect_Sphere(reverse_lightray, sphere2_center, sphere2_radius) > 0.0){
            return shadeColor * 0.4;
        }
        return shadeColor;

    }else if(t2 > 0.0)
    {
        Vec3 intersectionPoint = ray.pointAt(t2);
        Vec3 normal = (intersectionPoint - sphere2_center).unit_vector();
        Vec3 VL = (lightsource_pos - intersectionPoint).unit_vector();
        Vec3 VE = (ray.origin - intersectionPoint).unit_vector();

        double La = 15;
        double Ld = 8 * 10 * std::max(0.1, normal.dot(VL));
        double Ls = blinnPhong_shading(5, 10, normal, VL, VE, 35);

        double L = La + Ld + Ls;
        Color shadeColor = Color(0.5,1.68,1.52) * L;
        shadeColor = shadeColor.clamp(0.0, 255.0);
        Ray reverse_lightray(intersectionPoint, VL);
        if(intersect_Sphere(reverse_lightray, sphere1_center, sphere1_radius) > 0.0){
            return shadeColor * 0.4;
        }
        return shadeColor;

    }else if (t_plane > 0){
            Vec3 intersectionPoint = ray.pointAt(t_plane);
            Vec3 VL = (lightsource_pos - intersectionPoint);
            Ray reverse_lightray(intersectionPoint, VL);
            Ray upwards_ray(intersectionPoint, plane_normal);

            Vec3 reflected_dir = ray.direction -  plane_normal * (ray.direction.dot(plane_normal)) * 2;

            Ray reflected_ray(intersectionPoint, reflected_dir);

            Color returnColor = Color(132,132,133);

            if(intersect_Sphere(reflected_ray, sphere1_center, sphere1_radius) >= 0.0 || intersect_Sphere(reflected_ray, sphere2_center, sphere2_radius) >= 0.0){

                // std::cout << "Caught someone: " << std::endl;
                // intersectionPoint.print();
                // reflected_dir.print();
                returnColor = (returnColor + traceRay(reflected_ray, lightsource_pos, true) * 0.4).clamp(0,255);
            }
            // Shadow on plane
            if(intersect_Sphere(reverse_lightray, sphere1_center, sphere1_radius) >= 0.0 || intersect_Sphere(reverse_lightray, sphere2_center, sphere2_radius) >= 0.0){
                returnColor = returnColor * 0.4;
            }

            return returnColor;

            // Checkered pattern
            // bool is_blue = ((int)(intersectionPoint.x * 10) % 2 == 0) ^ ((int)(intersectionPoint.z * 10) % 2 == 0);
            // Color plane_color = is_blue ? Color(0, 0, 1)*100 : Color(0.5, 0.5, 0.5)*100;

            
        }

    return Color(0, 0, 0);

}

#endif
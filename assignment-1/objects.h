#ifndef OBJECTS_H
#define OBJECTS_H

#include "raytracer.h"
#include "vec3.h"
#include "shader.h"

#include "ray.h"
#include <iostream>

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

    Obj() : color(Vec3(0,0,0)) {}
    virtual double hit(const Ray& ray) const = 0;

    virtual Color objColor() const = 0;
    virtual Vec3 getCenter() const = 0;
    virtual Vec3 getNormal(const Ray& ray, Vec3 intersectionPoint) const = 0;
    virtual bool isGlazed() const = 0;

    virtual double shade(const Ray& ray, Sunlight lightsource, Vec3 intersectionPoint, Vec3 VL) const = 0;
};

class Triangle : public Obj{
public:
    Vec3 vertices[3];
    Color color;
    Shader shader;
    bool glazed;

    // Constructor
    Triangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, const Color &color)
        : vertices{v0, v1, v2}, color(color){};

    Color objColor() const override {
        return color;
    }

    double hit(const Ray& ray) const override {
        Vec3 p = ray.origin;
        Vec3 d = ray.direction;
        Vec3 a = vertices[0];
        Vec3 n = a.cross(vertices[2]).unit_vector();
        double t = ((a - p).dot(n)) / (d.dot(n));
        if(t > 0.001){
            return t;
        }
        return -1.0;
    }

    double shade(const Ray& ray, Sunlight lightsource, Vec3 intersectionPoint, Vec3 VL)const override{
        // Vec3 a = vertices[0];
        // Vec3 normal = a.cross(vertices[2]).unit_vector();
        // Vec3 VE = (ray.origin - intersectionPoint).unit_vector();
        // double postShadingColor = shader.calculateShading(lightsource.intensity, color, intersectionPoint, normal, VL, VE);
        // return postShadingColor;
        return 1;
    }

    Vec3 getCenter() const override {
        return Vec3((vertices[0].x + vertices[1].x, vertices[2].x)/ 3, (vertices[0].y + vertices[1].y + vertices[2].y)/3, (vertices[0].z + vertices[1].z + vertices[2].z)/3);
    };

    Vec3 getNormal(const Ray& ray, Vec3 intersectionPoint) const override {
        Vec3 a = vertices[0];
        Vec3 normal = a.cross(vertices[2]).unit_vector();
        return normal;
    }

    bool isGlazed() const override {
        return glazed;
    };
};

class Sphere : public Obj {
public:
    Vec3 center;
    double radius;
    Color color;
    Shader shader;
    bool glazed;

    // Constructor with arguments
    Sphere(const Vec3& center, double radius, const Color& color, const Shader& shader, const bool& glazed)
        : center(center), radius(radius), color(color), shader(shader), glazed(glazed) {}

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

    double shade(const Ray& ray, Sunlight lightsource, Vec3 intersectionPoint, Vec3 VL)const override{
        Vec3 normal = (intersectionPoint - center).unit_vector();
        Vec3 VE = (ray.origin - intersectionPoint).unit_vector();
        // Shader shader;
        double postShadingColor = shader.calculateShading(lightsource.intensity, color, intersectionPoint, normal, VL, VE);
        // std::cout << postShadingColor << std::endl;
        return postShadingColor;
    }

    Vec3 getCenter() const override {
        return center;
    };

    Vec3 getNormal(const Ray& ray, Vec3 intersectionPoint) const override {
        return (intersectionPoint - center).unit_vector();
    }

    bool isGlazed() const override {
        return glazed;
    };

};

class Plane : public Obj {
public:
    Vec3 normal;
    double height;
    Color color;
    Shader shader;
    bool glazed;

    // Constructor with arguments
    Plane(const Vec3& normal, double height, const Color& color, const Shader& shader,  const bool& glazed)
        : normal(normal), height(height), color(color), shader(shader), glazed(glazed) {}

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

    double shade(const Ray& ray, Sunlight lightsource, Vec3 intersectionPoint, Vec3 VL)const override{
        double shadingCoefficient = shader.ambientShading(lightsource.intensity, color/10);
        return shadingCoefficient;
    }

    // double shade(const Ray& ray, Sunlight lightsource, Vec3 intersectionPoint, Vec3 VL)const override{
    //     Vec3 normal = (intersectionPoint - Vec3(0,height,0)).unit_vector();
    //     Vec3 VE = (ray.origin - intersectionPoint).unit_vector();
    //     // Shader shader;
    //     double postShadingColor = shader.calculateShading(lightsource.intensity, color, intersectionPoint, normal, VL, VE);
    //     // std::cout << postShadingColor << std::endl;
    //     return postShadingColor;
    // }

    Vec3 getCenter() const override {
        return Vec3(0,height,0);
    };

    Vec3 getNormal(const Ray& ray, Vec3 intersectionPoint) const override {
        return normal;
    }

    bool isGlazed() const override {
        return glazed;
    };
   
};

struct HitResult {
    bool hit_anything;
    double closest_t;
    std::shared_ptr<Obj> closest_object;
    
};

class Objects {

public:
    std::vector<std::shared_ptr<Obj>> objects;
    Sunlight lightsource;
    std::vector < Sunlight> lights;

    Objects() : lightsource(Sunlight(Vec3(0, 0, 0),0.0)) {}

    void addObject(const std::shared_ptr<Obj>& obj) {
        objects.push_back(obj);
    }
    void addLight(const Sunlight& light) {
        lights.push_back(light);
    }

    bool hit_anything_for_shadows(const Ray& ray) const {
        bool hit_anything = false;

        for (const auto& object : objects) {
            double t = object->hit(ray);
            if (t > 0.001){
                // std::cout << t << std::endl;
                hit_anything = true;
                break;
            }
        }
        return hit_anything;
    }

    Color applyShading(const Ray& ray, double t, std::shared_ptr<Obj> closest_object, bool is_reflected_ray) const {
        Color objColor = closest_object->objColor();
        Vec3 intersectionPoint = ray.pointAt(t);
        Color sum = Color(0, 0, 0);

        if(closest_object->isGlazed() && is_reflected_ray == false) {
            sum = sum + applyGlaze(ray, t, closest_object);
        }

        for (const auto& light : lights){
            Vec3 VL = (light.position - intersectionPoint).unit_vector();

            Color shadedColor = (objColor * closest_object->shade(ray, light, intersectionPoint, VL)).clamp(0,255);

            // shadows
            Ray reverse_lightray(intersectionPoint, VL);
            if(hit_anything_for_shadows(reverse_lightray)){
                shadedColor = shadedColor * 0.4;
            }
            sum = (sum + shadedColor).clamp(0,255);
        }
        if(objColor.x == 132){
            sum.print();
        }



        // if(is_reflected_ray){
        //     shadedColor.print();
        // }
        


        return sum.clamp(0,255);
    }

    HitResult hit_anything(const Ray& ray, double t_max) const {
        bool hit_anything = false;

        double closest_t = t_max;
        std::shared_ptr<Obj> closest_object;

        for (const auto& object : objects) {
            double t = object->hit(ray);
            if (t > 0.01 && t < closest_t){
                closest_t = t;
                closest_object = object;
                hit_anything = true;
            }
        }
        return {hit_anything, closest_t, closest_object};
    }

    Color applyGlaze( const Ray& ray, double t, std::shared_ptr<Obj> closest_object) const {
        Vec3 intersectionPoint = ray.pointAt(t);
        Vec3 VL = (lightsource.position - intersectionPoint);
        Ray reverse_lightray(intersectionPoint, VL);

        Vec3 normal = closest_object->getNormal(ray, intersectionPoint);

        Vec3 reflected_dir = ray.direction -  normal * (ray.direction.dot(normal)) * 2;

        Ray reflected_ray(intersectionPoint, reflected_dir);

        Color returnColor = closest_object->objColor();
        // returnColor = Color(0, 0, 0);

        if(hit_anything_for_shadows(reflected_ray)){
            Color glazeColor = (castRay(reflected_ray, *this, lightsource.position, true) * 0.4).clamp(0, 255);
            return glazeColor;
        }
        return returnColor;
    }

    Color castRay(const Ray& ray, Objects world, Vec3 lightsource_pos, bool is_reflected_ray) const {
        // Color t = world.hit_anything(ray, 1000);
        auto hitResult = world.hit_anything(ray, 1000);
        if(hitResult.hit_anything){
            return world.applyShading(ray, hitResult.closest_t, hitResult.closest_object, is_reflected_ray);
        }
        return Color(0, 0, 0);  
    }
};

#endif
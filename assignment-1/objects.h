#ifndef OBJECTS_H
#define OBJECTS_H

#include "raytracer.h"
#include "Vec3.h"
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

struct HitResult {
    double t;
    Vec3 normal;
};

class Obj {
public:
    Color color;

    Obj() : color(Vec3(0,0,0)) {}
    virtual HitResult hit(const Ray& ray) = 0;

    virtual Color objColor() const = 0;
    virtual Vec3 getCenter() const = 0;
    virtual Vec3 getNormal(const Ray& ray, Vec3 intersectionPoint) const = 0;
    virtual bool isGlazed() const = 0;

    virtual double shade(const Ray& ray, Sunlight lightsource, Vec3 intersectionPoint, Vec3 normal, Vec3 VL) const = 0;
};

struct HitAnythingResult {
    bool hit_anything;
    double closest_t;
    std::shared_ptr<Obj> closest_object;
    Vec3 normal;
};



class Triangle : public Obj{
public:
    Vec3 a, b, c;
    Color color;
    Shader shader;
    bool glazed;

    // Constructor
    Triangle(const Vec3 &a, const Vec3 &b, const Vec3 &c, const Color &color, const Shader& shader, const bool& glazed)
        : a(a), b(b), c(c), color(color), shader(shader), glazed(glazed) {};

    Color objColor() const override {
        return color;
    }

    double alt_hit(const Ray& ray) {
                // compute the plane's normal
        Vec3 ab = b - a;
        Vec3 ac = c - a;
        // no need to unit_vector
        Vec3 N = ab.cross(ac); // N
        float area2 = N.length();
    
        // Step 1: finding P
        
        // check if the ray and plane are parallel.
        float NdotRayDirection = N.dot(ray.direction);
        if (fabs(NdotRayDirection) < 0.000000000000001) // almost 0
            return -1.0; // they are parallel, so they don't intersect! 

        // compute d parameter using equation 2
        float d = -N.dot(a);

        // compute t (equation 3)
        double t = -(N.dot(ray.origin) + d) / NdotRayDirection;
        
        // check if the triangle is behind the ray
        if (t < 0) return -1.0; // the triangle is behind
    
        // compute the intersection point using equation 1
        Vec3 P = ray.origin + ray.direction*t;
    
        // Step 2: inside-outside test
        // Vec3 C; // vector perpendicular to triangle's plane
    
        // edge 0
        Vec3 edge0 = b - a; 
        Vec3 vp0 = P - a;
        Vec3 C = edge0.cross(vp0);
        if (N.dot(C) < 0) return -1.0; // P is on the right side
    
        // edge 1
        Vec3 edge1 = c - b; 
        Vec3 vp1 = P - b;
        C = edge1.cross(vp1);
        if (N.dot(C) < 0)  return -1.0; // P is on the right side
    
        // edge 2
        Vec3 edge2 = a - c; 
        Vec3 vp2 = P - c;
        C = edge2.cross(vp2);
        if (N.dot(C) < 0) return -1.0; // P is on the right side;
        // std::cout << t << std::endl;
        return t; // this ray hits the triangle

    }

    HitResult hit(const Ray& ray) override {

        float epsilon = std::numeric_limits<float>::epsilon();
        // epsilon is almost 0.0;

        Vec3 edge1 = b - a;
        Vec3 edge2 = c - a;
        Vec3 ray_dir = ray.direction;
        Vec3 ray_cross_2 = ray_dir.cross(edge2);
        float det = edge1.dot(ray_cross_2);

        if (det > -epsilon && det < epsilon)
            return {-1.0, Vec3(0,0,0)};    // This ray is parallel to this triangle.
        
        float inv_det = 1.0 / det;
        Vec3 s = ray.origin - a;
        float u = s.dot(ray_cross_2)*inv_det;

        if (u < 0 || u > 1)
            return {-1.0, Vec3(0,0,0)};

        Vec3 s_cross_e1 = s.cross(edge1);
        float v =  ray_dir.dot(s_cross_e1)*inv_det;

        if (v < 0 || u + v > 1)
            return {-1.0, Vec3(0,0,0)};

        // At this stage we can compute t to find out where the intersection point is on the line.
        float t =  edge2.dot(s_cross_e1) * inv_det;


        if (t > epsilon) // ray intersection
        {
            double Nx = a.y * b.z - a.z * b.y;
            double Ny = a.z * b.x - a.x * b.z;
            double Nz = a.x * b.y - a.y * b.x;
            Vec3 normal(Nx, Ny, Nz);
            return {t, normal};
        }
        else // This means that there is a line intersection but not a ray intersection.
            return {-1.0, Vec3(0,0,0)};
    }

    double shade(const Ray& ray, Sunlight lightsource, Vec3 intersectionPoint, Vec3 normal, Vec3 VL)const override{
        Vec3 VE = (ray.origin - intersectionPoint).unit_vector();
        double postShadingColor = shader.calculateShading(lightsource.intensity, color, intersectionPoint, normal, VL, VE);
        return postShadingColor;
    }

    Vec3 getCenter() const override {
        return Vec3((a.x + a.x, a.x)/ 3, (a.y + b.y + c.y)/3, (a.z + b.z + c.z)/3);
    };

    Vec3 getNormal(const Ray& ray, Vec3 intersectionPoint) const override {
        double Nx = a.y * b.z - a.z * b.y;
        double Ny = a.z * b.x - a.x * b.z;
        double Nz = a.x * b.y - a.y * b.x;
        return Vec3(Nx, Ny, Nz);
    }

    bool isGlazed() const override {
        return glazed;
    };
};

class Tetrahedron : public Obj {
public:
    Vec3 a, b, c, d;
    Color color;
    Shader shader;
    bool glazed;

    // Constructor
    Tetrahedron(const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &d, const Color &color, const Shader& shader, const bool& glazed)
        : a(a), b(b), c(c), d(d), color(color), shader(shader), glazed(glazed) {};

    Color objColor() const override {
        return color;
    }

    HitResult hit(const Ray& ray) override {
        double t1 = hit_triangle(ray, a, b, c);
        if(t1 > 0.01){
            double Nx = a.y * b.z - a.z * b.y;
            double Ny = a.z * b.x - a.x * b.z;
            double Nz = a.x * b.y - a.y * b.x;
            Vec3 normal(Nx, Ny, Nz);
            return {t1,normal};
        }
        t1 = hit_triangle(ray, a, b, d);
        if (t1 > 0.01){
            double Nx = a.y * b.z - a.z * b.y;
            double Ny = a.z * b.x - a.x * b.z;
            double Nz = a.x * b.y - a.y * b.x;
            Vec3 normal(Nx, Ny, Nz);
            return {t1, normal};
        }
        t1 = hit_triangle(ray, a, c, d);
        if (t1 > 0.01){
            double Nx = a.y * c.z - a.z * c.y;
            double Ny = a.z * c.x - a.x * c.z;
            double Nz = a.x * c.y - a.y * c.x;
            Vec3 normal(Nx, Ny, Nz);
            return {t1, normal};
        }
        t1 = hit_triangle(ray, b, c, d);
        if (t1 > 0.01){
            double Nx = c.y * b.z - c.z * b.y;
            double Ny = c.z * b.x - c.x * b.z;
            double Nz = c.x * b.y - c.y * b.x;
            Vec3 normal(Nx, Ny, Nz);
            return {t1, normal};
        }
        return {-1.0, Vec3(0,0,0)};
    }

    double hit_triangle(const Ray& ray, const Vec3& a, const Vec3& b, const Vec3& c) {

        float epsilon = std::numeric_limits<float>::epsilon();
        // epsilon is almost 0.0;

        Vec3 edge1 = b - a;
        Vec3 edge2 = c - a;
        Vec3 ray_dir = ray.direction;
        Vec3 ray_cross_2 = ray_dir.cross(edge2);
        float det = edge1.dot(ray_cross_2);

        if (det > -epsilon && det < epsilon)
            return -1.0;    // This ray is parallel to this triangle.
        
        float inv_det = 1.0 / det;
        Vec3 s = ray.origin - a;
        float u = s.dot(ray_cross_2)*inv_det;

        if (u < 0 || u > 1)
            return -1.0;

        Vec3 s_cross_e1 = s.cross(edge1);
        float v =  ray_dir.dot(s_cross_e1)*inv_det;

        if (v < 0 || u + v > 1)
            return -1.0;

        // At this stage we can compute t to find out where the intersection point is on the line.
        float t =  edge2.dot(s_cross_e1) * inv_det;


        if (t > epsilon) // ray intersection
        {   
            return t;
        }
        else // line intersection but not a ray intersection.
            return -1.0;

        return -1.0;
    }

    double shade(const Ray& ray, Sunlight lightsource, Vec3 intersectionPoint, Vec3 normal, Vec3 VL)const override{
        Vec3 VE = (ray.origin - intersectionPoint).unit_vector();
        double postShadingColor = shader.calculateShading(lightsource.intensity, color, intersectionPoint, normal, VL, VE);
        return postShadingColor;
        // return 1;
    }

    Vec3 getCenter() const override {
        return Vec3((a.x + b.x + c.x + d.x)/ 4, (a.y + b.y + c.y + d.y)/4, (a.z + b.z + c.z + d.z)/4);
    };

    Vec3 getNormal(const Ray& ray, Vec3 intersectionPoint) const override {
        return Vec3(0, 0, 0);
    // // Determine which face the intersection point belongs to
    // if (isPointInTriangle(intersectionPoint, a, b, c)) {
    //     return (b - a).cross(c - a).unit_vector();;
    // } else if (isPointInTriangle(intersectionPoint, a, c, d)) {
    //     return (c - a).cross(d - a).unit_vector();;
    // } else if (isPointInTriangle(intersectionPoint, a, d, b)) {
    //     return (d - a).cross(b - a).unit_vector();;
    // } else {
    //     return (b - a).cross(d - b).unit_vector();;
    // }
    // }
    
    }

    bool isGlazed() const override {
        return glazed;
    }
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

    HitResult hit(const Ray& ray) override {
        // quadratic equation
        double a = ray.direction.dot(ray.direction);
        double b = (ray.direction * 2).dot(ray.origin - center);
        double c = (ray.origin - center).dot((ray.origin - center)) - (radius * radius);
        

        double discriminant = (b * b) - (4 * a * c);

        double t1 = (-b - (sqrt(discriminant))) / (2 * a);

        // we don't care about negative roots because they are not visible through the viewing plane
        if(discriminant >= 0){
            Vec3 normal = (ray.origin + (ray.direction*t1) - center).unit_vector();
            // std::cout << "OUT: " << t1 << std::endl;
            return {t1, normal};
        }
        return {-1.0, Vec3(0,0,0)};
    }

    Color objColor() const override {
        return color;
    }

    double shade(const Ray& ray, Sunlight lightsource, Vec3 intersectionPoint, Vec3 normal, Vec3 VL)const override{
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

    HitResult hit(const Ray& ray) override  {
        double t = (height - ray.origin.y) / ray.direction.y;
        if(t>0.01){
            return {t, normal};
        }

        return {-1.0, normal};
    }

    Color objColor() const override {
        return color;
    }

    double shade(const Ray& ray, Sunlight lightsource, Vec3 intersectionPoint, Vec3 normal, Vec3 VL)const override{
            Vec3 VE = (ray.origin - intersectionPoint).unit_vector();
            // Shader shader;
            double postShadingColor = shader.calculateShading(lightsource.intensity, color, intersectionPoint, normal, VL, VE);
            // std::cout << postShadingColor << std::endl;
            return postShadingColor;
        }

    // double shade(const Ray& ray, Sunlight lightsource, Vec3 intersectionPoint, Vec3 normal, Vec3 VL)const override{
    //     double shadingCoefficient = shader.ambientShading(lightsource.intensity, color/10);
    //     return shadingCoefficient;
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
            HitResult hitResult = object->hit(ray);
            if (hitResult.t > 0.001){
                // std::cout << t << std::endl;
                hit_anything = true;
                break;
            }
        }
        return hit_anything;
    }

    Color applyShading(const Ray& ray, double t, std::shared_ptr<Obj> closest_object, Vec3 normal, bool is_reflected_ray) const {
        Color objColor = closest_object->objColor();
        Vec3 intersectionPoint = ray.pointAt(t);
        Color sum = Color(0, 0, 0);

        if(closest_object->isGlazed() && is_reflected_ray == false) {
            sum = sum + applyGlaze(ray, t, closest_object, normal);
        }

        for (const auto& light : lights){
            Vec3 VL = (light.position - intersectionPoint).unit_vector();

            Color shadedColor = (objColor * closest_object->shade(ray, light, intersectionPoint, normal, VL)).clamp(0,255);

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

    HitAnythingResult hit_anything(const Ray& ray, double t_max) const {
        bool hit_anything = false;
        Vec3 surface_normal(0, 0, 0);

        double closest_t = t_max;
        std::shared_ptr<Obj> closest_object;

        for (const auto& object : objects) {
            HitResult hitResult = object->hit(ray);
            if (hitResult.t > 0.01 && hitResult.t < closest_t){
                closest_t = hitResult.t;
                closest_object = object;
                hit_anything = true;
                surface_normal = hitResult.normal;
            }
        }
        return {hit_anything, closest_t, closest_object, surface_normal};
    }

    Color applyGlaze( const Ray& ray, double t, std::shared_ptr<Obj> closest_object, Vec3 normal) const {
        Vec3 intersectionPoint = ray.pointAt(t);
        Vec3 reflected_dir = ray.direction - normal * (ray.direction.dot(normal)) * 2;

        Ray reflected_ray(intersectionPoint, reflected_dir);

        Color returnColor = closest_object->objColor();
        // returnColor = Color(0, 0, 0);

        if(hit_anything_for_shadows(reflected_ray)){
            Color glazeColor = (castRay(reflected_ray, *this, lightsource.position, true) * 0.4).clamp(0, 255);
            return glazeColor;
        }
        return Color(0, 0, 0);
    }

    Color castRay(const Ray& ray, Objects world, Vec3 lightsource_pos, bool is_reflected_ray) const {
        // Color t = world.hit_anything(ray, 1000);
        auto hitResult = world.hit_anything(ray, 1000);
        
        if(hitResult.hit_anything){
            return world.applyShading(ray, hitResult.closest_t, hitResult.closest_object, hitResult.normal, is_reflected_ray);
        }
        return Color(0, 0, 0);  
    }
};

#endif
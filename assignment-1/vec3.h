#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include <bits/stdc++.h> 

// class Color {
// public:
//     double r, g, b;

//     Color(double r, double g, double b) : r(r), g(g), b(b) {}

//     Color operator+(const Color& other) const {
//         return Color(r + other.r, g + other.g, b + other.b);
//     }

//     Color operator-(const Color& other) const {
//         return Color(r - other.r, g - other.g, b - other.b);
//     }

//     Color operator*(double t) const {
//         return Color(r * t, g * t, b * t);
//     }

//     Color operator/(double t) const {
//         return Color(r / t, g / t, b / t);
//     }

//     Color unit_vector() const{
//         double l = length();
//         return Color(r / l, g / l, b / l);
//     }

//     double length() const {
//         return sqrt(x * x + y * y + z * z);
//     }
// };

class Vec3 {
public:
    double x, y, z;

    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    void print() {
        std::cout << '[' << x << ", " << y << ", " << z << ']' << std::endl;
    }

    Vec3& operator=(const Vec3& other) {
        x = other.x; 
        y = other.y;
        z = other.z;
        return *this;
    }

    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator*(double t) const {
        return Vec3(x * t, y * t, z * t);
    }

    Vec3 operator/(double t){
        return Vec3(x / t, y / t, z / t);
    }

    double length() const {
        return sqrt(x * x + y * y + z * z);
    }
    Vec3 unit_vector() const{
        double l = length();
        return Vec3(x / l, y / l, z / l);
    }
    Vec3 clamp(double low, double high) {
        return Vec3(std::clamp(x,low,high), std::clamp(y,low,high), std::clamp(z,low,high));
    }
    double dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    double magnitude() const {
        return sqrt(x * x + y * y + z * z);
    }

};

using Color = Vec3;

using Point3 = Vec3;

#endif

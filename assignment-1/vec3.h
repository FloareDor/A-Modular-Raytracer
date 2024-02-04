#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include <bits/stdc++.h> 

class Vec3 {
public:
    double x, y, z;

    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    void print() {
        std::cout << '[' << x << ", " << y << ", " << z << ']' << std::endl;
    }

    Vec3& operator=(const Vec3& v2) {
        x = v2.x; 
        y = v2.y;
        z = v2.z;
        return *this;
    }

    Vec3 operator+(const Vec3& v2) const {
        return Vec3(x + v2.x, y + v2.y, z + v2.z);
    }

    Vec3 operator-(const Vec3& v2) const {
        return Vec3(x - v2.x, y - v2.y, z - v2.z);
    }

    Vec3 operator*(double t) const {
        return Vec3(x * t, y * t, z * t);
    }

    Vec3 operator/(double t) const{
        return Vec3(x / t, y / t, z / t);
    }

    double length() const {
        return sqrt(x * x + y * y + z * z);
    }
    Vec3 unit_vector() const {
        double l = length();
        return Vec3(x / l, y / l, z / l);
    }
    Vec3 clamp(double low, double high) {
        return Vec3(std::clamp(x,low,high), std::clamp(y,low,high), std::clamp(z,low,high));
    }
    double dot(const Vec3& v2) const {
        return x * v2.x + y * v2.y + z * v2.z;
    }

    double magnitude() const {
        return sqrt(x * x + y * y + z * z);
    }

    Vec3 cross(const Vec3& v2) {
        return Vec3(y * v2.z - v2.y * z, v2.x * z - x * v2.z, x * v2.y - v2.x * y);
    }
};

using Color = Vec3;

using Point3 = Vec3;

#endif


#ifndef SHADER_H
#define SHADER_H

#include "vec3.h"


class Shader {
public:
    // Calculate shading given intersection point, normal, light direction, and view direction
    static Color calculateShading(Color objectColor, const Vec3& intersectionPoint, const Vec3& normal, const Vec3& VL, const Vec3& VE) {

		double lightIntensity = 10.0;
		double ambientCoefficient = 1.5;  // Ambient intensity
        double diffuseCoefficient = 8.0; // Diffuse coefficient
        double specularCoefficient = 2.5; // Specular coefficient
        double shininess = 35.0;         // Shininess factor

		// ambient shading model
		double ambientIntensity = ambientCoefficient * lightIntensity;
        // Calculate diffuse component
        double diffuseIntensity = diffuseCoefficient * lightIntensity * std::max(0.1, normal.dot(VL));

        // Calculate specular component using Blinn-Phong model
		Vec3 VH = (VL + VE) / (VL + VE).magnitude();
		double specularIntensity = specularCoefficient * lightIntensity * std::pow(std::max(0.1, normal.dot(VH)), shininess);

        return (objectColor * (ambientIntensity + diffuseIntensity + specularIntensity)).clamp(0.0, 255.0);
    }
};

#endif
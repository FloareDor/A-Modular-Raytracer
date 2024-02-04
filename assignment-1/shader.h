
#ifndef SHADER_H
#define SHADER_H

#include "vec3.h"


class Shader {
public:
    // Calculate shading given intersection point, normal, light direction, and view direction
    static Color calculateShading(double lightIntensity, Color objectColor, const Vec3& intersectionPoint, const Vec3& normal, const Vec3& VL, const Vec3& VE) {

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

    static Color ambientShading(double lightIntensity, Color objectColor) {

		double ambientCoefficient = 1.5;  // Ambient intensity

		// ambient shading model
		double ambientIntensity = ambientCoefficient * lightIntensity;


        return (objectColor * (ambientIntensity)).clamp(0.0, 255.0);
    }

    static Color diffuseShading(double lightIntensity, Color objectColor, const Vec3& intersectionPoint, const Vec3& normal, const Vec3& VL, const Vec3& VE) {

		double ambientCoefficient = 1.5;  // Ambient intensity
        double diffuseCoefficient = 8.0; // Diffuse coefficient
        double specularCoefficient = 2.5; // Specular coefficient
        double shininess = 35.0;         // Shininess factor

        double diffuseIntensity = diffuseCoefficient * lightIntensity * std::max(0.1, normal.dot(VL));

        return (objectColor * (diffuseIntensity)).clamp(0.0, 255.0);
    }
};

#endif
#version 330 core

out vec4 FragColor;

flat in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

float near = 0.1;
float far  = 3.0;

float ambientShading(float lightIntensity, vec3 objectColor) {
    float ambientCoefficient = 0.000001; // Ambient intensity
    // ambient shading model
    float ambientIntensity = ambientCoefficient * lightIntensity;
    return ambientIntensity;
}

vec3 diffuseShading(float lightIntensity, vec3 objectColor, vec3 intersectionPoint, vec3 normal, vec3 VL, vec3 VE) {
    float diffuseCoefficient = 0.9;

    float diffuseIntensity = diffuseCoefficient * lightIntensity * max(0.1, dot(normal, VL));
    return clamp(objectColor * diffuseIntensity, 0.0, 1.0);
}

void main()
{

    float lightIntensity = 1.0;
    // Ambient lighting
    float ambientIntensity = ambientShading(lightIntensity, objectColor);
    vec3 ambient = ambientIntensity * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 diffuse = diffuseShading(lightIntensity, objectColor, FragPos, norm, lightDir, viewDir);

    vec3 result = (ambient + diffuse);
    FragColor = vec4(result, 1.0);
}
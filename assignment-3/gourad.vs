#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 rotationMatrix;
uniform mat4 translationMatrix;
uniform mat4 scalingMatrix;
uniform mat4 viewportMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 cameraMatrix;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 cameraPos;

out vec3 vertexColor; 

void main()
{
    mat4 modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
    gl_Position = viewportMatrix * perspectiveMatrix * cameraMatrix * modelMatrix * vec4(aPos, 1.0);

    vec3 FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
    vec3 Normal = mat3(transpose(inverse(modelMatrix))) * aNormal;

    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    float diffuseStrength = 2.0;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // Specular
    float specularStrength = 2;
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * lightColor;

    vertexColor = (ambient + diffuse + specular) * objectColor;
}
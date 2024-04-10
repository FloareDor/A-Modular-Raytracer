#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 1) in vec3 aNormal;

uniform mat4 rotationMatrix;
uniform mat4 translationMatrix;
uniform mat4 scalingMatrix;
uniform mat4 viewportMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 cameraMatrix;

out vec3 ourColor;
out vec3 Normal;
out vec3 FragPos;  

void main()
{
    mat4 modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
    gl_Position = viewportMatrix * perspectiveMatrix * cameraMatrix * modelMatrix * vec4(aPos, 1.0);
    FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    ourColor = aColor;
}
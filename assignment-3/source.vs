#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 rotationMatrix;
uniform mat4 translationMatrix;
uniform mat4 scalingMatrix;
uniform mat4 viewportMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 orthographicMatrix;
uniform mat4 cameraMatrix;

out vec3 ourColor;

void main()
{
    gl_Position = viewportMatrix * perspectiveMatrix * cameraMatrix * translationMatrix * rotationMatrix * scalingMatrix * vec4(aPos, 1.0);
    ourColor = aColor;
}
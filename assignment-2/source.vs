#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 1
uniform mat4 rotationMatrix;
uniform mat4 translationMatrix;
uniform mat4 scalingMatrix;
uniform mat4 viewportMatrix;

out vec3 ourColor; // output a color to the fragment shader

void main()
{
    // gl_Position = vec4(aPos, 1.0);

    // comment this and uncomment the matrix * vector operation in ebo.cpp or main.cpp to use CPU instead of GPU for matrix transformations
    gl_Position = viewportMatrix * translationMatrix * scalingMatrix * rotationMatrix * vec4(aPos, 1.0);
    ourColor = aColor; // setting ourColor to the input color we got from the vertex data
}

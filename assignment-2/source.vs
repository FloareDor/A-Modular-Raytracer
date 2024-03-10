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
    gl_Position = translationMatrix * viewportMatrix * scalingMatrix * rotationMatrix *  vec4(aPos, 1.0);
    // gl_Position =    viewportMatrix * vec4(aPos, 1.0);
    // gl_Position = vec4(aPos, 1.0);
    ourColor = aColor; // set ourColor to the input color we got from the vertex data
}

// we're using the GPU now for transformations!!

// GPU is clearly faster by 2 times approx!
#version 330 core

out vec4 FragColor;
in vec3 ourColor;

void main()
{
    // Outputs the raw depth value (z coordinate)
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}
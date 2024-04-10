#version 330 core
out vec4 FragColor;
in vec3 ourColor;

float near = 0.1; 
float far  = 3.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{             
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far
    FragColor = vec4(vec3(depth), 1.0);
}
## [RayTracer C++](https://github.com/FloareDor/A-Modular-Raytracer)

github: https://github.com/FloareDor/A-Modular-Raytracer

### This is an assignment as part of the CAP 4730 course of Computational Structures for Computer Graphics.

#### The topics in the course include:
Enhancing ray tracing by ray reconstruction (AI)
Games with and without ray tracing
Real-time ray tracing with NVIDIA
Hardware-accelerated ray tracing in iPhone 15 pro.
3D Math Primer For Graphics and Game Development
Rendering realistic faces by learning maps from 3D object coordinates to surface coordinates
Spatial aliasing, Temporal aliasing
Weierstrass's approximation theorem
Computed Tomography
Volume Rendering

#### Summarized topics:
Introduction, basic linear algebra, geometric transformations, shading and illumination, rendering algorithms, anti-aliasing techniques, modeling curves and surfaces, advance topics (time permitting).

## Assignment-1

### Overview

This assignment involves implementing a basic rendering system using C++, OpenGL and GLFW. The project includes files for camera management, object representation, ray tracing, shaders, and vector operations. I have used the GLFW library for window management and GLEW for OpenGL extension loading.

### Project Structure

    assignment-1: This directory contains the source files for the assignment.
        main.cpp: Main file containing the rendering logic and GLFW initialization.
        camera.h: Header file defining the camera class and its methods.
        objects.h: Header file defining object representations and their specific properties like shading logic.
        ray.h: Header file containing the definition of rays and related operations.
        shader.h: Header file defining shaders and shader management utilities.
        vec3.h: Header file containing the definition of a 3D vector class and its operations.

    Dependencies: This directory contains external dependencies required for the project.
        glew-2.1.0: GLEW library version 2.1.0, used for OpenGL extension loading.
        glfw-3.3.9.bin.WIN64: GLFW library version 3.3.9 for window management.

### Build Instructions

To compile and run the project, use the following command:

bash
```
g++ main.cpp -o img -I path/to/glew-2.1.0/include -I path/to/glfw-3.3.9.bin.WIN64/include \
-L path/to/glfw-3.3.9.bin.WIN64/lib-mingw-w64 -L path/to/glew-2.1.0/lib/Release/x64 \
-lglfw3dll -lglew32 -lopengl32 && ./img
```

Make sure to replace path/to/glew-2.1.0 and path/to/glfw-3.3.9.bin.WIN64 with the actual paths where you have stored the dependencies.
Usage

    Press the 'O' key to enable orthogonal mode for rendering.
    Press the 'P' key to disable orthogonal mode and revert to perspective rendering.
    Press the 'N' key to render the next frame (according to the Movie 2).

Increase the output resolution if needed by changing the "200" to a higher value in the 391st line in main.cpp
here ->
```
setupCameraAndTexture(finalRenderName, orthogonal, 200, Vec3(x, y, z), Vec3(0, -1, 0), look_at, Vec3(-3, -2, 1), Vec3(1, 0, -2), Vec3(0.8, -0.3, -1), Vec3(sphere_x+2, 0.2, sphere_z+2));
```

### Additional Notes

    Ensure that you have GLEW and GLFW libraries installed and accessible in the specified directories.
    This assignment assumes basic knowledge of OpenGL, shaders, and computer graphics concepts.
    Feel free to explore and modify the source code to experiment with different rendering techniques and effects.

### Report Documentation

[Report Link](https://docs.google.com/document/d/1jGkdgDSwsnVtV0JDLlxvXoNwAoLGbHrM1XjqVxrgt94/edit?usp=sharing)

Author

Sai Ravi Teja Gangavarapu

12/02/2024
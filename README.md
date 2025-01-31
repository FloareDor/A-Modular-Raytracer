## [RayTracer C++](https://github.com/FloareDor/A-Modular-Raytracer)

github: https://github.com/FloareDor/A-Modular-Raytracer

### This is an assignment as part of the CAP 4730 course of Computational Structures for Computer Graphics.

#### The topics in the course include:
- Enhancing ray tracing by ray reconstruction (AI)
- Games with and without ray tracing
- Real-time ray tracing with NVIDIA
- Hardware-accelerated ray tracing in iPhone 15 pro.
- 3D Math Primer For Graphics and Game Development
- Rendering realistic faces by learning maps from 3D object coordinates to surface coordinates
- Spatial aliasing, Temporal aliasing
- Weierstrass's approximation theorem
- Computed Tomography
- Volume Rendering

#### Summarized topics:
Introduction, basic linear algebra, geometric transformations, shading and illumination, rendering algorithms, anti-aliasing techniques, modeling curves and surfaces, advance topics (time permitting).

## Assignment-3

## [Report Link](https://docs.google.com/document/d/1DTAsHPCa5sUCt-7-Fp10aKigKIGVX1oMG3Pj3SDiDgs/edit?usp=sharing)

### Overview

This assignment involves extending the basic 3D rendering system from Assignment 2 using C++, OpenGL, and GLFW. The project includes new functionality for implementing perspective projection, visualizing the z-buffer, and adding lighting effects using Gouraud and Phong shading. The GLFW library is used for window management, and GLEW is used for OpenGL extension loading.

### Project Structure

assignment-3: This directory contains the source files for the assignment.

    main.cpp: The main file containing the rendering loop, OpenGL setup, and input handling logic. It also includes functions for loading OBJ files, reading shader source files, and handling window resize events.

    objLoader.cpp: Contains functions for loading OBJ files and preparing vertex and index data for rendering.

    flat.vs: Vertex shader source code for flat shading.
    flat.fs: Fragment shader source code for flat shading.

    gourad.vs: Vertex shader source code for Gouraud shading.
    gourad.fs: Fragment shader source code for Gouraud shading.

    phong.vs: Vertex shader source code for Phong shading.
    phong.fs: Fragment shader source code for Phong shading.

    z.fs: Fragment shader source code for visualizing raw depth values (z coordinate).
    z-prime.fs: Fragment shader source code for visualizing linearized depth values.

Dependencies: This directory contains external dependencies required for the project (same as Assignment 2).

    glew-2.1.0: GLEW library version 2.1.0, used for OpenGL extension loading.

    glfw-3.3.9.bin.WIN64: GLFW library version 3.3.9 for window management.

    glm: GLM (OpenGL Mathematics Library): Used for matrix operations and vector types.

### Build Instructions

To compile and run the project, use the same instructions as Assignment 2:

```bash

cd assignment-3

g++ main.cpp -o viewer -I path/to/glew-2.1.0/include -I path/to/glfw-3.3.9.bin.WIN64/include \
-I path/to/glm \
-L path/to/glfw-3.3.9.bin.WIN64/lib-mingw-w64 -L path/to/glew-2.1.0/lib/Release/x64 \
-lglfw3dll -lglew32 -lopengl32 && ./viewer

```

Replace path/to/glew-2.1.0, path/to/glfw-3.3.9.bin.WIN64, and path/to/glm with the actual paths where you have stored the dependencies.

### Usage

    Use 'W', 'A', 'S', 'D' for camera movement (FPS-style controls)
    Use the mouse to rotate the camera
    Press 'F' or 'G' to change the field of view (perspective projection)
    Use 'U', 'J', 'Y', 'H', 'I', 'K' to control the light position
    Press 'N' or 'M' for scaling the object
    Press 'X' or 'Y' or 'Z' for rotation around the respectives axises
    Use 'Up', 'Down', 'Left', 'Right' arrow keys for moving the object around

Note: The 'C', 'V', 'B' controls for switching between shading techniques are not currently working. To change the type of shading, modify line 266 in main.cpp:

```cpp
unsigned int currentShaderProgram = shaderProgram;
```

Replace shaderProgram with one of the following options:

    `gouradShaderProgram` for Gouraud shading
    `flatShaderProgram` for flat shading
    `phongShaderProgram` for Phong shading

The project extends the previous assignment by adding perspective projection, z-buffer visualization (using z.fs and z-prime.fs shaders), and lighting effects using Gouraud and Phong shading. The report should document and describe each implemented functionality, including the effects of changing perspective matrix parameters, verifying depth values, and comparing the different shading techniques.

## Assignment-2

## [Report Link](https://docs.google.com/document/d/1d7grJcv-Z8uK306yEp6ZM9QUHI4miLZKDYFT4ripV4Q/edit?usp=sharing) 
### Overview

This assignment involves implementing a basic 3D rendering system using C++, OpenGL, and GLFW. The project includes functionality for loading and rendering 3D models from OBJ files, applying transformations (rotation, scaling, translation), and handling user input. The GLFW library is used for window management, and GLEW is used for OpenGL extension loading.

### Project Structure

    assignment-2: This directory contains the source files for the assignment.

        main.cpp: The main file containing the rendering loop, OpenGL setup, and input handling logic. It also includes functions for       loading OBJ files, reading shader source files, and handling window resize events.
        ebo.cpp: The same as main.cpp but by using Element Buffer Objects (instead of seperate triangles data stucture).
        source.vs: The vertex shader source code, responsible for transforming vertex positions and passing color data to the fragment shader.
        source.fs: The fragment shader source code, responsible for determining the final color of each pixel.

    Dependencies: This directory contains external dependencies required for the project.
        glew-2.1.0: GLEW library version 2.1.0, used for OpenGL extension loading.
        glfw-3.3.9.bin.WIN64: GLFW library version 3.3.9 for window management.
        glm: GLM (OpenGL Mathematics Library): Used for matrix operations and vector types.

### Build Instructions

To compile and run the project, use the following command:
bash
```
cd assignment-2
```

bash
```
g++ main.cpp -o img -I C:\UFL\ComputerGraphics\Graphics\assignment\Dependencies\glfw-3.3.9.bin.WIN64\include -I C:\UFL\ComputerGraphics\Graphics\assignment\Dependencies\glew-2.1.0\include -I C:\UFL\ComputerGraphics\Graphics\assignment\Dependencies\glm -L C:\UFL\ComputerGraphics\Graphics\assignment\Dependencies\glfw-3.3.9.bin.WIN64\lib-mingw-w64 -L C:\UFL\ComputerGraphics\Graphics\assignment\Dependencies\glew-2.1.0\lib\Release\x64 -lglfw3dll -lglew32 -lopengl32; & ./img
```
```
g++ ebo.cpp -o img -I C:\UFL\ComputerGraphics\Graphics\assignment\Dependencies\glfw-3.3.9.bin.WIN64\include -I C:\UFL\ComputerGraphics\Graphics\assignment\Dependencies\glew-2.1.0\include -I C:\UFL\ComputerGraphics\Graphics\assignment\Dependencies\glm -L C:\UFL\ComputerGraphics\Graphics\assignment\Dependencies\glfw-3.3.9.bin.WIN64\lib-mingw-w64 -L C:\UFL

If you're using MacOS or Linux make sure to replace path/to/glew-2.1.0, path/to/glfw-3.3.9.bin.WIN64 and path/to/glm with the actual paths where you have stored the dependencies.
bash
```
g++ main.cpp -o img -I path/to/glew-2.1.0/include -I path/to/glfw-3.3.9.bin.WIN64/include \
-I path/to/glm \
-L path/to/glfw-3.3.9.bin.WIN64/lib-mingw-w64 -L path/to/glew-2.1.0/lib/Release/x64 \
-lglfw3dll -lglew32 -lopengl32 && ./img
```

Usage

    Use 'W', 'A', 'S', 'D' for translation
    Press 'X' or 'Y' or 'Z' for rotation around the respectives axises
    Press 'Up arrow' or 'Down arrow' for scaling the object

The project loads a 3D model from an OBJ file, applies transformations based on user input using CPU or GPU, and renders the transformed model in a window using OpenGL.

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
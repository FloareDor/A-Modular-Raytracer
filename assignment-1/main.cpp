// Based on templates from learnopengl.com
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdint.h>

#include<cmath>
#include <iostream>
#include "raytracer.h"
#include "vec3.h"
#include "shader.h"
#include "objects.h"
#include "camera.h"
#include <string>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1200;

std::string renderName = "fisheye";

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
	"gl_Position = vec4(aPos, 1.0);\n"
	"ourColor = aColor;\n"
	"TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(texture1, TexCoord);\n"
    "}\n\0";

void saveImageToPPM(const char* filename, unsigned char* imageData, const int width, const int height) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
        return;
    }

    // Write PPM header
    file << "P3\n" << width << " " << height << "\n255\n";

    // Write pixel values
    for (int i = 0; i < width * height * 3; i += 3) {
        file << static_cast<int>(imageData[i]) << " " << static_cast<int>(imageData[i + 1]) << " " << static_cast<int>(imageData[i + 2]) << "\n";
    }

    file.close();
}

void tga_write(std::string filename, uint32_t width, uint32_t height, uint8_t *dataBGRA, uint8_t dataChannels=4, uint8_t fileChannels=3)
{
    char * string_filename = filename.data();
	FILE *fp = NULL;
	fopen_s(&fp, string_filename, "wb");
	if (fp == NULL) return;

	uint8_t header[18] = { 0,0,2,0,0,0,0,0,0,0,0,0, (uint8_t)(width%256), (uint8_t)(width/256), (uint8_t)(height%256), (uint8_t)(height/256), (uint8_t)(fileChannels*8), 0x20 };
	fwrite(&header, 18, 1, fp);

	// Write image data row by row, but start from the bottom row and move upward
	for (int32_t y = height - 1; y >= 0; --y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			// Calculate the index of the pixel in the dataBGRA array
			uint32_t pixelIndex = (y * width + x) * dataChannels;

			// Loop through each channel of the pixel and write it to the file
			for (uint8_t channel = 0; channel < fileChannels; ++channel)
			{
				// Adjust the channel index if reading from BGRA to RGBA
				uint8_t adjustedChannel = (channel == 0) ? 2 : (channel == 2) ? 0 : channel;

				// Write the pixel channel to the file
				fputc(dataBGRA[pixelIndex + adjustedChannel], fp);
			}
		}
	}

	fclose(fp);
}

void setupCameraAndTexture(std::string filename, bool orthogonal, int width, Vec3 cameraPosition, Vec3 camera_up, Vec3 look_at, Vec3 light_pos, Vec3 sphere1_centre, Vec3 sphere2_centre, Vec3 sphere3_centre){
    auto aspect_ratio = 16.0 / 9.0;

	int height = static_cast<int>( width / aspect_ratio);
	if(height < 1){ height = 1; }
    auto image = CameraAndScene(orthogonal, width, height, cameraPosition, camera_up, look_at, light_pos, sphere1_centre, sphere2_centre, sphere3_centre);
    unsigned char *data = &image[0];
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        GLint maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        std::string fullFilename = "tga_files/" + std::string(filename) + ".tga";
        // std::cout << "Texture dimensions maximum supported size: " << maxTextureSize << std::endl;
        tga_write(fullFilename.c_str(), width, height, data, 3, 3); // Assuming 3 data channels and 3 file channels for RGB
    }
} 

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Display RGB Array", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // // GLEW: load all OpenGL function pointers
    glewInit();

    // build and compile the shaders
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // float vertices[] = {
    //     // positions          // colors           // texture coords
    //      0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
    //      0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    //     -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    //     -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    // };
    float vertices[] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // load and create a texture 
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // setupCameraAndTexture(renderNameOrthogonal, true, 2560, Vec3(4, -1, 5), Vec3(0, -1, 0), Vec3(0, 0, -1), Vec3(-3, -2, 1), Vec3(1, 0, -2), Vec3(0.8, -0.3, -1), Vec3(2, 0.2, -0.5));

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        bool orthogonal = false;

        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);

        // render container
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
double x = 4;
double y = -1;
double z = 5;
double angle = 0.05;
double sphere_motion_radius = 3;

double initx = 2 + 5 * sin(angle) + 1;
double initz = -0.5 + 5 * cos(angle) - 0.5;
double inity = y;

double sphere_x = 2;
double sphere_z = -0.5;

int frame = 0;
bool orthogonal = false;
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    
    std::string persp = "_perspective";
    std::string renderNamePerspective = renderName + persp;
    std::string orth = "_orthogonal";
    std::string renderNameOrthogonal = renderName + orth;
    std::string finalRenderName = renderNameOrthogonal;

    // Checking if the 'O' key is pressed
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        orthogonal = true;
        std::cout << "Running with orthogonal set to true!" << std::endl;
    }

    // Checking if the 'P' key is pressed
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        orthogonal = false;
        std::cout << "Running with orthogonal set to false!" << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        z = z - 1;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        x = x - 1;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        z = z + 1;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        x = x + 1;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        y = y - 1;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        y = y + 1;
    }

    if(!orthogonal){
        finalRenderName = renderNamePerspective;
    }


    // animation loop
    if(glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        // Incrementing the angle for the circular motion
        angle += 0.015;

        // if(angle != 0.015+0.05 && x == initx && y == inity && z == initz){
        //     glfwSetWindowShouldClose(window, true);
        // }

        Vec3 spherebro(2, 0.2, -0.5);

        // Calculate the new x and z coordinates based on the angle
        x = 2 + 5 * sin(angle);
        z = -0.5 + 5 * cos(angle);

        double sphere_x = sphere_motion_radius * cos(angle);
        double sphere_z = sphere_motion_radius * sin(angle);

        const char* frame_str = std::to_string(frame).c_str();

        // std::cout << "next frame!" << std::endl;
        // cam Vec3(5, -1, 5)

        x = x + 1;
        z = z - 0.5;
        // y = y - 0.003;
        frame += 1;
    }

    Vec3 look_at = (Vec3(x,y,z) + Vec3(0,0,-1));
    look_at = Vec3(0, 0, -1);
    setupCameraAndTexture(finalRenderName, orthogonal, 200, Vec3(x, y, z), Vec3(0, -1, 0), look_at, Vec3(-3, -2, 1), Vec3(1, 0, -2), Vec3(0.8, -0.3, -1), Vec3(sphere_x+2, 0.2, sphere_z+2));
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
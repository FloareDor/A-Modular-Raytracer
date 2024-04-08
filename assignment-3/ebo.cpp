// template based on material from learnopengl.com
#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "objLoader.cpp"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include<./glm.hpp>
#include <./gtc/matrix_transform.hpp>

float rotationAngle = glm::radians(0.0f); // 45 degrees

// Rotation axis (x, y, z)
glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f);

glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);

glm::vec3 translationVector(0.0f, 0.0f, 0.0f);
glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);


glm::mat4 viewportMatrix = glm::mat4(
    1, 0.0f, 0.0f, 0.0f,
    0.0f, 1, 0.0f, 0.0f,
    0.0f, 0.0f, 1, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);

float sX = 1.0f;
float sY = 1.0f;
float sZ = 1.0f;

glm::mat4 scalingMatrix = glm::mat4(
    sX, 0.0f, 0.0f, 0.0f,
    0.0f, sY, 0.0f, 0.0f,
    0.0f, 0.0f, sZ, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightColor(1.0f, 0.5f, 1.0f);

glm::vec3 objectColor(1.0f, 0.5f, 0.31f);

glm::mat4 cameraMatrix = glm::lookAt(
    glm::vec3(0.0f, 0.0f, -1.0f), // Camera position
    glm::vec3(0.0f, 0.0f, 0.0f), // Look at position
    glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
);

struct Vertex {
    float x, y, z;
    float r, g, b;
};

char* readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file: " << filename << std::endl;
        return nullptr;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    char* shaderSource = new char[buffer.str().size() + 1];
    strcpy(shaderSource, buffer.str().c_str());

    return shaderSource;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float fov = glm::radians(120.0f); // Field of View in radians
float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT; // Aspect ratio of the window
float nearPlane = 0.1f; // Near clipping plane distance
float farPlane = 3.0f; // Far clipping plane distance

glm::mat4 perspectiveMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane);

float left = -10.0f;
float right = 10.0f;
float bottom = -10.0f;
float top = 10.0f;
float near = 0.1f;
float far = 3.0f;

glm::mat4 orthographicMatrix = glm::ortho(left, right, bottom, top, near, far);

// const char *vertexShaderSource = "#version 330 core\n"
//     "layout (location = 0) in vec3 aPos;\n"
//     "layout (location = 1) in vec3 aColor;\n" // Added color input
//     "out vec3 ourColor;\n" // Pass color to fragment shader
//     "void main()\n"
//     "{\n"
//     "   gl_Position = vec4(aPos, 1.0);\n"
//     "   ourColor = aColor;\n" // Set ourColor to the input color
//     "}\0";

// const char *fragmentShaderSource = "#version 330 core\n"
//     "in vec3 ourColor;\n" // Receive color from vertex shader
//     "out vec4 FragColor;\n"
//     "void main()\n"
//     "{\n"
//     "   FragColor = vec4(ourColor, 1.0);\n" // Use the color passed from the vertex shader
//     "}\n\0";

const char * vertexShaderSource = readFile("source.vs");
const char * fragmentShaderSource = readFile("source.fs");
// #define USE_PERSPECTIVE

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "viewGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // // glew: load all OpenGL function pointers
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);  
    // build and compile our shader program
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
    // float renderVertices[] = {
    //      0.5f,  0.5f, 0.0f,  // top right
    //      1.0f,  0.0f, 0.0f,  // red color
    //      0.5f, -0.5f, 0.0f,  // bottom right
    //      0.0f,  1.0f, 0.0f,  // green color
    //     -0.5f, -0.5f, 0.0f,  // bottom left
    //      0.0f,  0.0f, 1.0f,  // blue color
    //     -0.5f,  0.5f, 0.0f,  // top left 
    //      1.0f,  1.0f, 0.0f   // yellow color
    // };
    // unsigned int indices[] = {  // note that we start from 0!
    //     0, 1, 3,   // first triangle
    //     1, 2, 3    // second triangle
    // };
    // float vertices[sizeof(renderVertices)];

    // for (int i = 0; i < sizeof(renderVertices); i++)
    // {
    //     vertices[i] = renderVertices[i];
    // }

    objReturn obj = loadObjFile("data/data/pig.obj");
    float *renderVertices = obj.vertices;
    float *normals = obj.normals;
    int *indices = obj.indices;
    float vertices[obj.verticesSize];

    for (int i = 0; i < obj.verticesSize; i++)
    {
        vertices[i] = renderVertices[i];
    }

    // reference I used for implementing EBO: https://learnopengl.com/Getting-started/Hello-Triangle
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 


    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    double previousTime = glfwGetTime();
    while(!glfwWindowShouldClose(window))
    {   
        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - previousTime;
        // std::cout << "Time taken for this frame: " << elapsedTime << " seconds" << std::endl;
        previousTime = currentTime;

        processInput(window);
        // Clear the color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        for (int i = 0; i < obj.verticesSize/6; ++i) {
            // std::cout << vertices[i * 6] << " " << vertices[i * 6 + 1] << " " << vertices[i * 6 + 2] << std::endl;
            glm::vec4 vertexVec(renderVertices[i * 6], renderVertices[i * 6 + 1], renderVertices[i * 6 + 2], 1.0f);

            // uncomment this and comment the matrix * vector operation in source.vs to use CPU instead of GPU for matrix transformations
            // vertexVec = viewportMatrix * translationMatrix * scalingMatrix * rotationMatrix * vertexVec;
            vertices[i * 6] = vertexVec.x;
            vertices[i * 6 + 1] = vertexVec.y;
            vertices[i * 6 + 2] = vertexVec.z;
        }
        // the avg is approx 0.019 seconds!!
        GLuint MatrixID = glGetUniformLocation(shaderProgram, "rotationMatrix");
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &rotationMatrix[0][0]);

        GLuint MatrixID2 = glGetUniformLocation(shaderProgram, "translationMatrix");
        glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &translationMatrix[0][0]);

        GLuint MatrixID3 = glGetUniformLocation(shaderProgram, "scalingMatrix");
        glUniformMatrix4fv(MatrixID3, 1, GL_FALSE, &scalingMatrix[0][0]);

        GLuint MatrixID4 = glGetUniformLocation(shaderProgram, "viewportMatrix");
        glUniformMatrix4fv(MatrixID4, 1, GL_FALSE, &viewportMatrix[0][0]);

        GLuint orthographicMatrixID = glGetUniformLocation(shaderProgram, "orthographicMatrix");
        glUniformMatrix4fv(orthographicMatrixID, 1, GL_FALSE, &orthographicMatrix[0][0]);

        GLuint perspectiveMatrixID = glGetUniformLocation(shaderProgram, "perspectiveMatrix");
        glUniformMatrix4fv(perspectiveMatrixID, 1, GL_FALSE, &perspectiveMatrix[0][0]);

        GLint viewLoc = glGetUniformLocation(shaderProgram, "cameraMatrix");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &cameraMatrix[0][0]);

        GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
        glUniform3fv(lightColorLoc, 1, &lightColor[0]);

        GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
        glUniform3fv(objectColorLoc, 1, &objectColor[0]);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, obj.verticesSize * sizeof(float), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.indicesSize * sizeof(int), indices, GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, obj.indicesSize, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec3 newRotationAxis(0.0f);
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        newRotationAxis.x = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        newRotationAxis.y = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        newRotationAxis.z = 1.0f;
    }

    if (newRotationAxis != glm::vec3(0.0f))
    {
        rotationAxis = glm::normalize(newRotationAxis);
        rotationAngle += glm::radians(2.0f);
        glm::mat4 rotationStep = glm::rotate(glm::mat4(1.0f), glm::radians(0.75f), rotationAxis);
        rotationMatrix = rotationStep * rotationMatrix;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        sX *= 1.01;
        sY *= 1.01;
        sZ *= 1.01;
    }
    
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        sX *= 0.99;
        sY *= 0.99;
        sZ *= 0.99;

    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        translationVector.x -= 0.01f;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        translationVector.x += 0.01f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        translationVector.y += 0.01f;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        translationVector.y -= 0.01f;
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        if (fov < glm::radians(150.0f)){
            fov = fov +  glm::radians(1.0f);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        if (fov > glm::radians(-150.0f)){
            fov = fov -  glm::radians(1.0f);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        nearPlane = nearPlane + 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        farPlane = farPlane + 0.01f;
    }

    translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);
    scalingMatrix = glm::mat4(
        sX, 0.0f, 0.0f, 0.0f,
        0.0f, sY, 0.0f, 0.0f,
        0.0f, 0.0f, sZ, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);


    perspectiveMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
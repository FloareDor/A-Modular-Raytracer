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

glm::vec3 lightPos(6.5f, 15.0f, 75.3f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

glm::vec3 objectColor(0.255f, 0.205f, 0.231f);

// glm::mat4 cameraMatrix = glm::lookAt(
//     glm::vec3(0.0f, 0.0f, -1.0f), // Camera position
//     glm::vec3(0.0f, 0.0f, 0.0f), // Look at position
//     glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
// );

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;

glm::mat4 cameraMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

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
float farPlane = 10.0f; // Far clipping plane distance

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

const char * flatVertexShaderSource = readFile("flat.vs");
const char * flatFragmentShaderSource = readFile("flat.fs");

const char * gouradVertexShaderSource = readFile("gourad.vs");
const char * gouradFragmentShaderSource = readFile("gourad.fs");

const char * phongVertexShaderSource = readFile("phong.vs");
const char * phongFragmentShaderSource = readFile("phong.fs");
unsigned int currentShaderProgram;
unsigned int phongShaderProgram;
unsigned int gouraudShaderProgram;
unsigned int flatShaderProgram;
// #define USE_PERSPECTIVE

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
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
    glDepthFunc(GL_LESS);  
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

    // Flat shading shaders
    unsigned int flatVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(flatVertexShader, 1, &flatVertexShaderSource, NULL);
    glCompileShader(flatVertexShader);
    unsigned int flatFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(flatFragmentShader, 1, &flatFragmentShaderSource, NULL);
    glCompileShader(flatFragmentShader);

    // Gouraud shading shaders
    unsigned int gouradVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(gouradVertexShader, 1, &gouradVertexShaderSource, NULL);
    glCompileShader(gouradVertexShader);
    unsigned int gouradFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(gouradFragmentShader, 1, &gouradFragmentShaderSource, NULL);
    glCompileShader(gouradFragmentShader);

    // Phong shading shaders
    unsigned int phongVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(phongVertexShader, 1, &phongVertexShaderSource, NULL);
    glCompileShader(phongVertexShader);
    unsigned int phongFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(phongFragmentShader, 1, &phongFragmentShaderSource, NULL);
    glCompileShader(phongFragmentShader);

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    unsigned int flatShaderProgram = glCreateProgram();
    glAttachShader(flatShaderProgram, flatVertexShader);
    glAttachShader(flatShaderProgram, flatFragmentShader);
    glLinkProgram(flatShaderProgram);

    unsigned int gouradShaderProgram = glCreateProgram();
    glAttachShader(gouradShaderProgram, gouradVertexShader);
    glAttachShader(gouradShaderProgram, gouradFragmentShader);
    glLinkProgram(gouradShaderProgram);

    unsigned int phongShaderProgram = glCreateProgram();
    glAttachShader(phongShaderProgram, phongVertexShader);
    glAttachShader(phongShaderProgram, phongFragmentShader);
    glLinkProgram(phongShaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(flatVertexShader);
    glDeleteShader(flatFragmentShader);
    glDeleteShader(gouradVertexShader);
    glDeleteShader(gouradFragmentShader);
    glDeleteShader(phongVertexShader);
    glDeleteShader(phongFragmentShader);

    // Set default shading technique to flat shading
    unsigned int currentShaderProgram = shaderProgram;

    objReturn obj = loadObjFile("data/data/sphere.obj");
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
        std::cout << lightPos.x << " " << lightPos.y << " " << lightPos.z << std::endl;
        // std::cout << "fov: " << fov << " " << "nearPlane: " << nearPlane << " " << "farPlane: " << farPlane << std::endl;
        // std::cout << "Time taken for this frame: " << elapsedTime << " seconds" << std::endl;
        // std::cout << currentShaderProgram  << std::endl;
        previousTime = currentTime;

        processInput(window);
        // Clear the color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(currentShaderProgram);
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
        GLuint MatrixID = glGetUniformLocation(currentShaderProgram, "rotationMatrix");
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &rotationMatrix[0][0]);

        GLuint MatrixID2 = glGetUniformLocation(currentShaderProgram, "translationMatrix");
        glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &translationMatrix[0][0]);

        GLuint MatrixID3 = glGetUniformLocation(currentShaderProgram, "scalingMatrix");
        glUniformMatrix4fv(MatrixID3, 1, GL_FALSE, &scalingMatrix[0][0]);

        GLuint MatrixID4 = glGetUniformLocation(currentShaderProgram, "viewportMatrix");
        glUniformMatrix4fv(MatrixID4, 1, GL_FALSE, &viewportMatrix[0][0]);

        GLuint orthographicMatrixID = glGetUniformLocation(currentShaderProgram, "orthographicMatrix");
        glUniformMatrix4fv(orthographicMatrixID, 1, GL_FALSE, &orthographicMatrix[0][0]);

        GLuint perspectiveMatrixID = glGetUniformLocation(currentShaderProgram, "perspectiveMatrix");
        glUniformMatrix4fv(perspectiveMatrixID, 1, GL_FALSE, &perspectiveMatrix[0][0]);

        GLint viewLoc = glGetUniformLocation(currentShaderProgram, "cameraMatrix");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &cameraMatrix[0][0]);

        GLint lightColorLoc = glGetUniformLocation(currentShaderProgram, "lightColor");
        glUniform3fv(lightColorLoc, 1, &lightColor[0]);

        GLint objectColorLoc = glGetUniformLocation(currentShaderProgram, "objectColor");
        glUniform3fv(objectColorLoc, 1, &objectColor[0]);

        GLint lightPosLoc = glGetUniformLocation(currentShaderProgram, "lightPos");
        glUniform3fv(lightPosLoc, 1, &lightPos[0]);

        GLint cameraPosLoc = glGetUniformLocation(currentShaderProgram, "cameraPos");
        glUniform3fv(cameraPosLoc, 1, &cameraPos[0]);

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
    glDeleteProgram(currentShaderProgram);

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

    // glm::vec3 newRotationAxis(0.0f);
    // if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    // {
    //     newRotationAxis.x = 1.0f;
    // }
    // if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    // {
    //     newRotationAxis.y = 1.0f;
    // }
    // if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    // {
    //     newRotationAxis.z = 1.0f;
    // }

    // if (newRotationAxis != glm::vec3(0.0f))
    // {
    //     rotationAxis = glm::normalize(newRotationAxis);
    //     rotationAngle += glm::radians(2.0f);
    //     glm::mat4 rotationStep = glm::rotate(glm::mat4(1.0f), glm::radians(0.75f), rotationAxis);
    //     rotationMatrix = rotationStep * rotationMatrix;
    // }

    
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        currentShaderProgram = phongShaderProgram;
    }
    else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
    {
        currentShaderProgram = gouraudShaderProgram;
    }
    else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    {
        currentShaderProgram = flatShaderProgram;
    }

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        sX *= 1.01;
        sY *= 1.01;
        sZ *= 1.01;
    }
    
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        sX *= 0.99;
        sY *= 0.99;
        sZ *= 0.99;

    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        translationVector.x -= 0.01f;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        translationVector.x += 0.01f;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        translationVector.y += 0.01f;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
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
    // if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    // {
    //     nearPlane = nearPlane + 0.01f;
    // }
    // if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    // {
    //     farPlane = farPlane + 0.01f;
    // }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        lightPos.x = lightPos.x + 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    {
        lightPos.x = lightPos.x - 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        lightPos.y = lightPos.y + 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        lightPos.y = lightPos.y - 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {
        lightPos.z = lightPos.z + 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        lightPos.z = lightPos.z - 0.1f;
    }

    float cameraSpeed = 0.01f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
        lightPos.x = cameraPos.x;
        lightPos.y = cameraPos.y;
        lightPos.z = cameraPos.z;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
        lightPos.x = cameraPos.x;
        lightPos.y = cameraPos.y;
        lightPos.z = cameraPos.z;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        lightPos.x = cameraPos.x;
        lightPos.y = cameraPos.y;
        lightPos.z = cameraPos.z;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        lightPos.x = cameraPos.x;
        lightPos.y = cameraPos.y;
        lightPos.z = cameraPos.z;

    // Mouse input
    static bool firstMouse = true;
    static float lastX = SCR_WIDTH / 2.0f;
    static float lastY = SCR_HEIGHT / 2.0f;

    if (firstMouse)
    {
        lastX = SCR_WIDTH / 2.0f;
        lastY = SCR_HEIGHT / 2.0f;
        firstMouse = false;
    }

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.3f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

    translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);
    scalingMatrix = glm::mat4(
        sX, 0.0f, 0.0f, 0.0f,
        0.0f, sY, 0.0f, 0.0f,
        0.0f, 0.0f, sZ, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    perspectiveMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
    cameraMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
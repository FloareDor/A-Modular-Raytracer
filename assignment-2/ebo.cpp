// template based on material from learnopengl.com
#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include<./glm.hpp>
#include <./gtc/matrix_transform.hpp>

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

struct objReturn
{
    float* vertices;
    int* indices;
    int verticesSize;
    int indicesSize;
};

struct Vertex {
    float x, y, z;
    float r, g, b;
};

objReturn loadObjFile(const std::string &filename)
{
    std::ifstream file(filename);
    std::vector<Vertex> renderVertices;
    std::vector<float> vertices;
    std::vector<int> indices;
    objReturn obj;

    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();
    float maxZ = std::numeric_limits<float>::lowest();
    

    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file: " << filename << std::endl;
        return obj;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        char firstChar;
        iss >> firstChar;

        if (firstChar == 'v') {
            if (firstChar == 'v' && iss.peek() == ' ') {
                Vertex vertex;
                iss >> vertex.x >> vertex.y >> vertex.z;
                // Set color based on coordinates: red for x, green for y, blue for z
                vertex.r = (vertex.x + 1.0f) / 2.0f; // Map x from [-1, 1] to [0, 1]
                vertex.g = (vertex.y + 1.0f) / 2.0f; // Map y from [-1, 1] to [0, 1]
                vertex.b = (vertex.z + 1.0f) / 2.0f; // Map z from [-1, 1] to [0, 1]
                vertex.r = 0.45f;
                vertex.g = 0.3f;
                vertex.b = 0.7f;

                vertices.push_back(vertex.x);
                vertices.push_back(vertex.y);
                vertices.push_back(vertex.z);
                vertices.push_back(vertex.r);
                vertices.push_back(vertex.g);
                vertices.push_back(vertex.b);
            }
        } else if (firstChar == 'f') {

            // Read face indices
            std::string v1, v2, v3, v4;
            iss >> v1 >> v2 >> v3 >> v4;

            // Parse indices and subtract 1 since OBJ indices are 1-based
            int idx1, idx2, idx3, idx4;
            sscanf(v1.c_str(), "%d", &idx1);
            sscanf(v2.c_str(), "%d", &idx2);
            sscanf(v3.c_str(), "%d", &idx3);
            sscanf(v4.c_str(), "%d", &idx4);
            
            // If it's a quad
            if (!v4.empty()){
                indices.push_back(idx4);
                indices.push_back(idx2);
                indices.push_back(idx1);

                indices.push_back(idx4);
                indices.push_back(idx3);
                indices.push_back(idx2);

            }else {// If it's a triangle
                indices.push_back(idx1);
                indices.push_back(idx2);
                indices.push_back(idx3);
            }
            // std::cout << idx1 << " " << idx2 << " " << idx3 << " " << idx4 << std::endl;
        }
    }

    file.close();
    size_t numFloats = renderVertices.size() * 6; // 6 floats per Vertex

    // Calculate the scaling factors for x, y, and z coordinates
    float scaleX = 1 / maxX;
    float scaleY = 1 / maxY;
    float scaleZ = 1 / maxZ;

    viewportMatrix = glm::mat4(
        scaleX, 0.0f, 0.0f, 0.0f,
        0.0f, scaleY, 0.0f, 0.0f,
        0.0f, 0.0f, scaleZ, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    
    obj.indicesSize = indices.size();
    obj.verticesSize = vertices.size();

    float* verticesArray = new float[obj.verticesSize];
    for (size_t i = 0; i < vertices.size(); ++i) {
        verticesArray[i] = vertices[i];
    }

    // Allocate memory for indices
    int* indicesArray = new int[obj.indicesSize];
    for (size_t i = 0; i < indices.size(); ++i) {
        // std::cout << indices.size() << ": " << indices[i] << std::endl;
        indicesArray[i] = indices[i] - 1;
    }

    obj.indices = indicesArray;
    obj.vertices = verticesArray;

    return obj;
}


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

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glEnable(GL_DEPTH_TEST);
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

    objReturn obj = loadObjFile("data/data/cube.obj");
    float *renderVertices = obj.vertices;
    int *indices = obj.indices;
    float vertices[obj.verticesSize];

    for(int i = 0; i < obj.verticesSize / 6; i++){
        std::cout << obj.verticesSize/6 << ": " << renderVertices[i * 6] << " " << renderVertices[i * 6 + 1] << " " << renderVertices[(i * 6)+2] << " Colors: " << renderVertices[i * 6+3] << " " << renderVertices[i * 6 + 4] << " " << renderVertices[(i * 6)+5] << std::endl;
    }

    for(int i = 0; i < obj.indicesSize/3; i++){
        std::cout << obj.indicesSize/3 << ": " << indices[i * 3] << " " << indices[i * 3 + 1] << " " << indices[(i * 3)+2] << std::endl;
    }

    for (int i = 0; i < obj.verticesSize; i++)
    {
        vertices[i] = renderVertices[i];
    }

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

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 


    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        processInput(window);
        // Clear the color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        for (int i = 0; i < obj.verticesSize/6; ++i) {
            // std::cout << vertices[i * 6] << " " << vertices[i * 6 + 1] << " " << vertices[i * 6 + 2] << std::endl;
            glm::vec4 vertexVec(renderVertices[i * 6], renderVertices[i * 6 + 1], renderVertices[i * 6 + 2], 1.0f);
            vertexVec = scalingMatrix * vertexVec;
            vertices[i * 6] = vertexVec.x;
            vertices[i * 6 + 1] = vertexVec.y;
            vertices[i * 6 + 2] = vertexVec.z;
        }
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

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        sX *= 1.01;
        sY *= 1.01;
        sZ *= 1.01;
        scalingMatrix = glm::mat4(
            sX, 0.0f, 0.0f, 0.0f,
            0.0f, sY, 0.0f, 0.0f,
            0.0f, 0.0f, sZ, 0.0f, 
            0.0f, 0.0f, 0.0f, 1.0f);
    }
    
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        sX *= 0.99;
        sY *= 0.99;
        sZ *= 0.99;
        scalingMatrix = glm::mat4(
            sX, 0.0f, 0.0f, 0.0f,
            0.0f, sY, 0.0f, 0.0f,
            0.0f, 0.0f, sZ, 0.0f, 
            0.0f, 0.0f, 0.0f, 1.0f);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
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

float rotationAngle = glm::radians(0.0f); // 45 degrees

// Rotation axis (x, y, z)
glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f);

// Create the rotation matrix
glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);

glm::vec3 translationVector(0.0f, 0.0f, 0.0f);

// glm::mat4 translationMatrix = glm::mat4(
//     1, 0.0f, 0.0f, 0.0f,
//     0.0f, 1, 0.0f, 0.0f,
//     0.0f, 0.0f, 1, 0.0f,
//     0.0f, 0.0f, 0.0f, 1.0f
// );
glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);

glm::mat4 viewportMatrix = glm::mat4(
    1, 0.0f, 0.0f, 0.0f,
    0.0f, 1, 0.0f, 0.0f,
    0.0f, 0.0f, 1, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);

float sX = 1;
float sY = 1;
float sZ = 1;

float tX = 0.0f;
float tY = 0.0f;
float tZ = 0.0f;

glm::mat4 scalingMatrix = glm::mat4(
    sX, 0.0f, 0.0f, tX,
    0.0f, sY, 0.0f, tY,
    0.0f, 0.0f, sZ, tZ,
    0.0f, 0.0f, 0.0f, 1.0f
);

struct Vertex {
    float x, y, z;
    float r, g, b; // Color components
};

struct Triangle {
    Vertex v1, v2, v3;
};

struct objReturn
{
    float *vertices;
    int size;
};

objReturn loadObjFile(const std::string &filename)
{
    std::ifstream file(filename);
    std::vector<Vertex> vertices;
    std::vector<Vertex> renderVertices;
    objReturn obj;
    std::vector<Triangle> triangles;

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

                // glm::vec4 vertexVec(vertex.x, vertex.y, vertex.z, 1.0f);
                // vertexVec = rotationMatrix * vertexVec;
                // vertex.x = vertexVec.x;
                // vertex.y = vertexVec.y;
                // vertex.z = vertexVec.z;

                vertices.push_back(vertex);
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

        // Assuming vertices and face indices are consistent
        // You might need to adjust for different indexing conventions

        
        Vertex ver1, ver2, ver3, ver4, mid;
        ver1 = vertices[idx1 - 1];
        ver2 = vertices[idx2 - 1];
        ver3 = vertices[idx3 - 1];
        ver4 = vertices[idx4 - 1];
        ver1.r = 1.0;
        ver1.g = 0.5;
        ver1.b = 0.3;

        ver2.r = 1.0;
        ver2.g = 0.5;
        ver2.b = 1.0;

        ver3.r = 0.3;
        ver3.g = 0.5;
        ver3.b = 1.0;

        // ver4.r = 0.3;
        // ver4.g = 0.1;
        // ver4.b = 1.0;
        
        
        // If it's a quad
        if (!v4.empty())
        {
            // triangles[0].v1 = ver4;
            Triangle triangle;
            triangle.v1 = ver4;
            triangle.v2 = ver2;
            triangle.v3 = ver1;
            triangles.push_back(triangle);

            renderVertices.push_back(ver4);
            renderVertices.push_back(ver2);
            renderVertices.push_back(ver1);

            triangle.v1 = ver4;
            triangle.v2 = ver3;
            triangle.v3 = ver2;
            triangles.push_back(triangle);

            renderVertices.push_back(ver4);
            renderVertices.push_back(ver3);
            renderVertices.push_back(ver2);
        }
        // If it's a triangle
        else {

            Triangle triangle;
            triangle.v1 = ver1;
            triangle.v2 = ver2;
            triangle.v3 = ver3;
            triangles.push_back(triangle);
            renderVertices.push_back(ver1);
            renderVertices.push_back(ver2);
            renderVertices.push_back(ver3);
        }
            

            std::cout << idx1 << " " << idx2 << " " << idx3 << " " << idx4 << std::endl;
        }
    }

    file.close();
    size_t numFloats = renderVertices.size() * 6; // 6 floats per Vertex

    // Allocate memory for the array of floats
    float *renderVerticesArray = new float[numFloats];
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float maxY = std::numeric_limits<float>::min();
    float maxZ = std::numeric_limits<float>::min();

    for (const auto& vertex : vertices) {
        if (vertex.x < minX) minX = vertex.x;
        if (vertex.y < minY) minY = vertex.y;
        if (vertex.z < minZ) minZ = vertex.z;
        if (vertex.x > maxX) maxX = vertex.x;
        if (vertex.y > maxY) maxY = vertex.y;
        if (vertex.z > maxZ) maxZ = vertex.z;
    }

    // Calculate the scaling factors for x, y, and z coordinates
    float scaleX = 1 / maxX;
    float scaleY = 1 / maxY;
    float scaleZ = 1 / maxZ;

    float f = 0.2f;
    viewportMatrix = glm::mat4(
        scaleX, 0.0f, 0.0f, 0.0f,
        0.0f, scaleY, 0.0f, 0.0f,
        0.0f, 0.0f, scaleZ, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    // viewportMatrix = glm::mat4(
    //     scaleX, 0.0f, 0.0f, 0.0f,
    //     0.0f, scaleY, 0.0f, 0.0f,
    //     0.0f, 0.0f, scaleZ, 0.0f,
    //     0.0f, 0.0f, 0.0f,    1.0f
    // );


    // Convert Vertex objects to floats
    for (size_t i = 0; i < renderVertices.size(); ++i) {
        renderVerticesArray[i * 6]     = (renderVertices[i].x);
        renderVerticesArray[i * 6 + 1] = (renderVertices[i].y);
        renderVerticesArray[i * 6 + 2] = (renderVertices[i].z);
        renderVerticesArray[i * 6 + 3] = renderVertices[i].r;
        renderVerticesArray[i * 6 + 4] = renderVertices[i].g;
        renderVerticesArray[i * 6 + 5] = renderVertices[i].b;
        // std::cout << renderVerticesArray[i * 6] << " " << renderVerticesArray[i * 6 + 1] << " " << renderVerticesArray[i * 6 + 2] << std::endl;
    }

    obj.vertices = renderVerticesArray;
    obj.size = renderVertices.size();

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

const char * vertexShaderSource = readFile("source.vs");
const char * fragmentShaderSource = readFile("source.fs");

int main()
{
    std::cout << fragmentShaderSource;
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

    // std::cout << "size of vertices: " << vertices.size() << std::endl;
    objReturn obj = loadObjFile("data/data/cow.obj");
    float *renderVertices = obj.vertices;
    int size = obj.size * 6;
    std::cout << "size of vertices: " << size << std::endl;
    float vertices[size];

    for (int i = 0; i < size; i++)
    {
        vertices[i] = renderVertices[i];
        // std::cout << renderVertices[i] << std::endl;
    }

    unsigned int numVertices = sizeof(vertices) / sizeof(float) / 6;

    // unsigned int numVertices = nvertices / 6;
    std::cout << "fuc" << numVertices << "fuc" << std::endl;

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
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
    while(!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        // clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate the shader
        glUseProgram(shaderProgram);
    
        // // update the uniform color
        // float timeValue = glfwGetTime();
        // float greenValue = sin(timeValue) / 2.0f + 0.5f;
        // int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        for (int i = 0; i < obj.size; ++i) {
            glm::vec4 vertexVec(renderVertices[i * 6], renderVertices[i * 6 + 1], renderVertices[i * 6 + 2], 1.0f);
            // vertexVec = rotationMatrix * vertexVec;
            // vertexVec = scalingMatrix * viewportMatrix * rotationMatrix * vertexVec;
            // vertexVec = translationMatrix * scalingMatrix * rotationMatrix * vertexVec;
            // vertexVec = rotationMatrix * vertexVec;
            vertices[i * 6] = vertexVec.x;
            vertices[i * 6 + 1] = vertexVec.y;
            vertices[i * 6 + 2] = vertexVec.z;
        }

        GLuint MatrixID = glGetUniformLocation(shaderProgram, "rotationMatrix");
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &rotationMatrix[0][0]);

        GLuint MatrixID2 = glGetUniformLocation(shaderProgram, "translationMatrix");
        glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &translationMatrix[0][0]);

        GLuint MatrixID3 = glGetUniformLocation(shaderProgram, "scalingMatrix");
        glUniformMatrix4fv(MatrixID3, 1, GL_FALSE, &scalingMatrix[0][0]);

        GLuint MatrixID4 = glGetUniformLocation(shaderProgram, "viewportMatrix");
        glUniformMatrix4fv(MatrixID4, 1, GL_FALSE, &viewportMatrix[0][0]);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // now render the triangle
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);

    
        // swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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
        glm::mat4 rotationStep = glm::rotate(glm::mat4(1.0f), glm::radians(2.0f), rotationAxis);
        rotationMatrix = rotationStep * rotationMatrix;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        sX *= 1.1;
        sY *= 1.1;
        sZ *= 1.1;
        scalingMatrix = glm::mat4(
            sX, 0.0f, 0.0f, tX,
            0.0f, sY, 0.0f, tY,
            0.0f, 0.0f, sZ, tZ, 
            0.0f, 0.0f, 0.0f, 1.0f);
    }
    
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        sX *= 0.9;
        sY *= 0.9;
        sZ *= 0.9;
        scalingMatrix = glm::mat4(
            sX, 0.0f, 0.0f, tX,
            0.0f, sY, 0.0f, tY,
            0.0f, 0.0f, sZ, tZ, 
            0.0f, 0.0f, 0.0f, 1.0f);
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
    translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);

    // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    // {
    //     std::cout << "pressed A" << std::endl;
    //     // tX -= 0.5f;
    //     tY -= 0.5f;
    //     // tZ -= 0.5f;
    //     translationMatrix = glm::mat4(
    //         1.0f, 0.0f, 0.0f, tX,
    //         0.0f, 1.0f, 0.0f, tY, 
    //         0.0f, 0.0f, 1.0f, tZ, 
    //         0.0f, 0.0f, 0.0f, 1.0f);
    // }

    // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    // {
    //     // tX += 0.5f;
    //     tY += 0.5f;
    //     // tZ += 0.5f;
    //     translationMatrix = glm::mat4(
    //         1.0f, 0.0f, 0.0f, tX,
    //         0.0f, 1.0f, 0.0f, tY,
    //         0.0f, 0.0f, 1.0f, tZ, 
    //         0.0f, 0.0f, 0.0f, 1.0f);
    // }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
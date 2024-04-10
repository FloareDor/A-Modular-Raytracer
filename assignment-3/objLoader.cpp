

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include<./glm.hpp>
#include <./gtc/matrix_transform.hpp>

struct objReturn
{
    float* vertices;
    int* indices;
    int verticesSize;
    int indicesSize;
    float* normals;
    int normalsSize;
};

objReturn loadObjFile(const std::string& filename)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) {
        std::cerr << "Error: Failed to load OBJ file: " << filename << std::endl;
        std::cerr << "Error message: " << err << std::endl;
        return objReturn();
    }

    std::vector<float> vertices;
    std::vector<int> indices;
    std::vector<float> normals;
    objReturn obj;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            // // Set color based on coordinates: red for x, green for y, blue for z
            // vertices.push_back((attrib.vertices[3 * index.vertex_index + 0] + 1.0f) / 2.0f);
            // vertices.push_back((attrib.vertices[3 * index.vertex_index + 1] + 1.0f) / 2.0f);
            // vertices.push_back((attrib.vertices[3 * index.vertex_index + 2] + 1.0f) / 2.0f);

            indices.push_back(indices.size());

            if (index.normal_index >= 0) {
                vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
                vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
                vertices.push_back(attrib.normals[3 * index.normal_index + 2]);
            } else {
                // If the normal index is not available, add default normal (0, 0, 1)
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
                vertices.push_back(1.0f);
            }
        }
    }

    obj.indicesSize = indices.size();
    obj.verticesSize = vertices.size();
    obj.normalsSize = normals.size();

    float* verticesArray = new float[obj.verticesSize];
    std::copy(vertices.begin(), vertices.end(), verticesArray);

    int* indicesArray = new int[obj.indicesSize];
    std::copy(indices.begin(), indices.end(), indicesArray);

    float* normalsArray = new float[obj.normalsSize];
    std::copy(normals.begin(), normals.end(), normalsArray);

    obj.indices = indicesArray;
    obj.vertices = verticesArray;
    obj.normals = normalsArray;

    return obj;
}

objReturn loadObjFileForFlatShading(const std::string& filename)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) {
        std::cerr << "Error: Failed to load OBJ file: " << filename << std::endl;
        std::cerr << "Error message: " << err << std::endl;
        return objReturn();
    }

    std::vector<float> vertices;
    std::vector<int> indices;
    std::vector<float> normals;

    objReturn obj;

    for (const auto& shape : shapes) {
        for (size_t i = 0; i < shape.mesh.indices.size(); i += 3) {
            // Calculate the average normal for the current face
            glm::vec3 normal(0.0f);
            for (size_t j = 0; j < 3; ++j) {
                const auto& index = shape.mesh.indices[i + j];
                normal.x += attrib.normals[3 * index.normal_index + 0];
                normal.y += attrib.normals[3 * index.normal_index + 1];
                normal.z += attrib.normals[3 * index.normal_index + 2];
            }
            normal = glm::normalize(normal);

            // Assign the average normal to all vertices of the face
            for (size_t j = 0; j < 3; ++j) {
                const auto& index = shape.mesh.indices[i + j];
                vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
                vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
                vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);
                vertices.push_back(normal.x);
                vertices.push_back(normal.y);
                vertices.push_back(normal.z);
                indices.push_back(indices.size());
            }
        }
    }

    obj.indicesSize = indices.size();
    obj.verticesSize = vertices.size();
    obj.normalsSize = normals.size();

    float* verticesArray = new float[obj.verticesSize];
    std::copy(vertices.begin(), vertices.end(), verticesArray);

    int* indicesArray = new int[obj.indicesSize];
    std::copy(indices.begin(), indices.end(), indicesArray);

    float* normalsArray = new float[obj.normalsSize];
    std::copy(normals.begin(), normals.end(), normalsArray);

    obj.indices = indicesArray;
    obj.vertices = verticesArray;
    obj.normals = normalsArray;

    return obj;
}
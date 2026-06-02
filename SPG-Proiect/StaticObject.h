#ifndef STATIC_OBJECT_H
#define STATIC_OBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include "Shader.h"
#include "stb_image.h"

struct ObjectVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class StaticObject {
public:
    unsigned int VAO, VBO, EBO;
    unsigned int textureID;
    int numIndices;
    glm::vec3 position;
    glm::vec3 scale;
    float rotation;

    StaticObject() : VAO(0), VBO(0), EBO(0), textureID(0), numIndices(0),
        position(0.0f), scale(1.0f), rotation(0.0f) {
    }

    
    static StaticObject CreateCube(glm::vec3 pos, glm::vec3 size, float rotY, const char* texturePath) {
        StaticObject obj;
        obj.position = pos;
        obj.scale = size;
        obj.rotation = rotY;
        obj.CreateCubeGeometry(texturePath);
        return obj;
    }

    static StaticObject CreateCylinder(glm::vec3 pos, float radius, float height,
        int segments, float rotY, const char* texturePath) {
        StaticObject obj;
        obj.position = pos;
        obj.scale = glm::vec3(1.0f); 
        obj.rotation = rotY;
        obj.CreateCylinderGeometry(radius, height, segments, texturePath);
        return obj;
    }

    void Draw(const Shader& shader, const glm::mat4& view, const glm::mat4& projection) const {
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, scale);

        shader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Cleanup() {
        if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
        if (EBO != 0) glDeleteBuffers(1, &EBO);
        if (textureID != 0) glDeleteTextures(1, &textureID);
    }

private:
    void CreateCubeGeometry(const char* texturePath) {
        
        std::vector<ObjectVertex> vertices = {
            // Front face
            { { -0.5f,  1.0f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 3.0f } },
            { { -0.5f,  0.0f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
            { {  0.5f,  0.0f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
            { {  0.5f,  1.0f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 3.0f } },
            // Back face
            { { -0.5f,  1.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 3.0f } },
            { { -0.5f,  0.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
            { {  0.5f,  0.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
            { {  0.5f,  1.0f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 3.0f } },
            // Top face
            { { -0.5f,  1.0f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
            { { -0.5f,  1.0f,  0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
            { {  0.5f,  1.0f,  0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { {  0.5f,  1.0f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
            // Bottom face
            { { -0.5f,  0.0f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
            { { -0.5f,  0.0f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
            { {  0.5f,  0.0f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
            { {  0.5f,  0.0f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
            // Right face
            { {  0.5f,  1.0f,  0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 3.0f } },
            { {  0.5f,  0.0f,  0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
            { {  0.5f,  0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
            { {  0.5f,  1.0f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 3.0f } },
            // Left face
            { { -0.5f,  1.0f,  0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 3.0f } },
            { { -0.5f,  0.0f,  0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
            { { -0.5f,  0.0f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
            { { -0.5f,  1.0f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 3.0f } }
        };

        std::vector<unsigned int> indices = {
            0, 1, 2, 0, 2, 3,       // Front
            4, 6, 5, 4, 7, 6,       // Back
            8, 9, 10, 8, 10, 11,    // Top
            12, 13, 14, 12, 14, 15, // Bottom
            16, 17, 18, 16, 18, 19, // Right
            20, 21, 22, 20, 22, 23  // Left
        };

        SetupGeometry(vertices, indices, texturePath);
    }

    void CreateCylinderGeometry(float radius, float height, int segments, const char* texturePath) {
        std::vector<ObjectVertex> vertices;
        std::vector<unsigned int> indices;

        
        for (int i = 0; i < segments; i++) {
            float angle = glm::two_pi<float>() * i / segments;
            float x = radius * cos(angle);
            float z = radius * sin(angle);

            ObjectVertex v;
            v.Position = { x, height, z };
            v.Normal = { cos(angle), 0.0f, sin(angle) };
            v.TexCoords = { (float)i / segments, 1.0f };
            vertices.push_back(v);
        }

        
        for (int i = 0; i < segments; i++) {
            float angle = glm::two_pi<float>() * i / segments;
            float x = radius * cos(angle);
            float z = radius * sin(angle);

            ObjectVertex v;
            v.Position = { x, 0.0f, z };
            v.Normal = { cos(angle), 0.0f, sin(angle) };
            v.TexCoords = { (float)i / segments, 0.0f };
            vertices.push_back(v);
        }

        
        for (int i = 0; i < segments; i++) {
            unsigned int topLeft = i;
            unsigned int topRight = (i + 1) % segments;
            unsigned int bottomLeft = segments + i;
            unsigned int bottomRight = segments + ((i + 1) % segments);

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }

        SetupGeometry(vertices, indices, texturePath);
    }

    void SetupGeometry(const std::vector<ObjectVertex>& vertices,
        const std::vector<unsigned int>& indices,
        const char* texturePath) {
        numIndices = indices.size();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ObjectVertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertex), (void*)offsetof(ObjectVertex, Position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertex), (void*)offsetof(ObjectVertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ObjectVertex), (void*)offsetof(ObjectVertex, TexCoords));

        textureID = LoadTexture(texturePath);
    }

    unsigned int LoadTexture(const char* path) {
        unsigned int tid;
        glGenTextures(1, &tid);
        int w, h, ch;
        stbi_set_flip_vertically_on_load(true);

        unsigned char* data = stbi_load(path, &w, &h, &ch, 4);
        if (data) {
            glBindTexture(GL_TEXTURE_2D, tid);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else {
            std::cout << "Failed to load texture: " << path << std::endl;
        }
        return tid;
    }
};

#endif
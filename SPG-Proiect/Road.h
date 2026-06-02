#ifndef ROAD_H
#define ROAD_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include "Shader.h"
#include "stb_image.h"
#include "Terrain.h" 

struct RoadVertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
};

class Road {
public:
    unsigned int VAO, VBO, EBO;
    unsigned int textureID;
    int numIndices;

    
    Road(const Terrain& terrain, float innerRadius, float outerRadius, int segments, const char* texturePath) {
        std::vector<RoadVertex> vertices;
        std::vector<unsigned int> indices;

        for (int i = 0; i <= segments; i++) {
            float angle = glm::two_pi<float>() * i / segments;
            float cosA = cos(angle);
            float sinA = sin(angle);

            float posXIn = innerRadius * cosA;
            float posZIn = innerRadius * sinA;
            
            float posYIn = terrain.GetHeight(posXIn, posZIn) + 0.05f;

            RoadVertex vIn;
            vIn.Position = glm::vec3(posXIn, posYIn, posZIn);
        
            vIn.TexCoords = glm::vec2(0.0f, (float)i * (segments / 150.0f));
            vertices.push_back(vIn);

            float posXOut = outerRadius * cosA;
            float posZOut = outerRadius * sinA;
            float posYOut = terrain.GetHeight(posXOut, posZOut) + 0.05f;

            RoadVertex vOut;
            vOut.Position = glm::vec3(posXOut, posYOut, posZOut);
            vOut.TexCoords = glm::vec2(1.0f, (float)i * (segments / 150.0f));
            vertices.push_back(vOut);
        }

        for (int i = 0; i < segments; i++) {
            unsigned int i0 = i * 2;
            unsigned int i1 = i * 2 + 1;
            unsigned int i2 = (i + 1) * 2;
            unsigned int i3 = (i + 1) * 2 + 1;

            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            indices.push_back(i2);
            indices.push_back(i1);
            indices.push_back(i3);
        }

        numIndices = indices.size();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(RoadVertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RoadVertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(RoadVertex), (void*)offsetof(RoadVertex, TexCoords));

        textureID = loadTexture(texturePath);
    }

    void Draw(Shader& shader, glm::mat4 view, glm::mat4 projection) {
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setMat4("model", glm::mat4(1.0f));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    unsigned int loadTexture(const char* path) {
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
            stbi_image_free(data);
        }
        return tid;
    }
};
#endif
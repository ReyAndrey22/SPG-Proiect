#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include "Shader.h"
#include "stb_image.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
};

class Terrain {
public:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int textureID;
    int numIndices;


    Terrain(const char* heightmapPath, const char* texturePath) {
        int width;
        int height;
        int nrChannels;

        unsigned char* data = stbi_load(heightmapPath, &width, &height, &nrChannels, 1);

        if (!data) {
            std::cout << "Error: Failed to load heightmap at: " << heightmapPath << std::endl;
            return;
        }

        std::vector<Vertex> vertices;
        float yScale = 0.2f;
        float xzScale = 1.8f;


        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                Vertex v;
                unsigned char pixelValue = data[i * width + j];


                v.Position.x = (j - width / 2.0f) * xzScale;
                v.Position.y = (float)pixelValue * yScale - 10.0f;
                v.Position.z = (i - height / 2.0f) * xzScale;


                v.TexCoords.x = (float)j / (width / 16.0f);
                v.TexCoords.y = (float)i / (height / 16.0f);

                vertices.push_back(v);
            }
        }
        stbi_image_free(data);


        std::vector<unsigned int> indices;
        for (int i = 0; i < height - 1; i++) {
            for (int j = 0; j < width - 1; j++) {
                unsigned int topLeft = i * width + j;
                unsigned int topRight = topLeft + 1;
                unsigned int bottomLeft = (i + 1) * width + j;
                unsigned int bottomRight = bottomLeft + 1;


                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }
        numIndices = indices.size();


        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

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
    unsigned int loadTexture(const char* path) const {
        unsigned int tid;
        glGenTextures(1, &tid);
        int w;
        int h;
        int ch;
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
        return tid;
    }
};
#endif

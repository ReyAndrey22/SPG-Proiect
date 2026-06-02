#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <ctime>   
#include <string>

#include "Shader.h"
#include "Camera.h"
#include "Skybox.h"
#include "Terrain.h"
#include "Road.h"
#include "StaticObject.h"

const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 900;

Camera camera(glm::vec3(0.0f, 60.0f, 150.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;


glm::vec3 carPos(0.0f, 0.0f, 185.0f);
float carAngle = 0.0f;
float carSpeed = 45.0f;
glm::vec3 carSize(5.0f, 3.0f, 8.0f);


float npcPresetAngle = 0.0f;
float npcPresetSpeed = 0.5f; 
glm::vec3 npcPresetPos(0.0f);
glm::vec3 npcPresetSize(4.0f, 2.5f, 7.0f);


glm::vec3 npcRandomPos(30.0f, 0.0f, 30.0f);
glm::vec3 npcRandomDirection(1.0f, 0.0f, 0.0f);
float npcRandomSpeed = 20.0f;
float randomMoveTimer = 0.0f; 
glm::vec3 npcRandomSize(3.0f, 3.0f, 3.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window, const Terrain& terrain, const std::vector<StaticObject>& objects);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proiect SPG - Scena Urbana Animata", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader terrainShader("shaders/terrain.vs.txt", "shaders/terrain.fs.txt");
    Shader skyboxShader("shaders/skybox.vs.txt", "shaders/skybox.fs.txt");
    Shader roadShader("shaders/road.vs.txt", "shaders/road.fs.txt");
    Shader objectShader("shaders/object.vs.txt", "shaders/object.fs.txt");

    Terrain myTerrain("textures/heightmap.png", "textures/grass.png");

    float innerRadius = 160.0f;
    float outerRadius = 210.0f;
    Road myRoad(myTerrain, innerRadius, outerRadius, 600, "textures/asphalt.jpg");

    std::vector<std::string> faces = {
        "textures/rainbow_ft.png",
        "textures/rainbow_bk.png",
        "textures/rainbow_up.png",
        "textures/rainbow_dn.png",
        "textures/rainbow_rt.png",
        "textures/rainbow_lf.png"
    };
    Skybox mySkybox(faces);

    unsigned int buildingTex = StaticObject::LoadTexture("textures/medieval_red_brick.png");
    unsigned int barkTex = StaticObject::LoadTexture("textures/pine_bark.png");
    unsigned int leavesTex = StaticObject::LoadTexture("textures/grass.png");
    unsigned int poleTex = StaticObject::LoadTexture("textures/asphalt.png");
    unsigned int lightTex = StaticObject::LoadTexture("textures/white.png");
    unsigned int carTex = StaticObject::LoadTexture("textures/rainbow_bk.png");

    
    unsigned int presetNpcTex = StaticObject::LoadTexture("textures/rainbow_ft.png");
    unsigned int randomNpcTex = StaticObject::LoadTexture("textures/rainbow_dn.png");

    std::vector<StaticObject> staticObjects;
    srand(static_cast<unsigned int>(time(0)));

    int numBuildings = 15;
    int numTrees = 35;
    int numStreetLights = 6;

   
    for (int i = 0; i < numBuildings; i++) {
        float angle = (glm::two_pi<float>() / numBuildings) * i + ((rand() % 100) / 500.0f);
        float spawnRadius = outerRadius + 25.0f + (rand() % 20);

        float x = spawnRadius * cos(angle);
        float z = spawnRadius * sin(angle);
        float y = myTerrain.GetHeight(x, z);

        float w = 15.0f + (rand() % 12);
        float h = 25.0f + (rand() % 30);
        float d = 15.0f + (rand() % 12);

        float randomRotation = static_cast<float>(rand() % 360);

        staticObjects.push_back(StaticObject::CreateCube(glm::vec3(x, y, z), glm::vec3(w, h, d), randomRotation, buildingTex));
    }

    
    for (int i = 0; i < numTrees; i++) {
        float angle = (glm::two_pi<float>() / numTrees) * i + ((rand() % 100) / 200.0f);
        float spawnRadius = (i % 2 == 0) ? (innerRadius - 15.0f - (rand() % 25)) : (outerRadius + 8.0f + (rand() % 12));

        float x = spawnRadius * cos(angle);
        float z = spawnRadius * sin(angle);
        float y = myTerrain.GetHeight(x, z);

        float radius = 1.0f + (rand() % 10) * 0.1f;
        float height = 10.0f + (rand() % 8);

        staticObjects.push_back(StaticObject::CreateCylinder(glm::vec3(x, y, z), radius, height, 16, 0.0f, barkTex));

        float leafSize1 = 6.0f + (rand() % 4);
        float leafY1 = y + height;
        staticObjects.push_back(StaticObject::CreateCube(glm::vec3(x, leafY1, z), glm::vec3(leafSize1), 0.0f, leavesTex));

        float leafSize2 = leafSize1 * 0.7f;
        float leafY2 = leafY1 + leafSize1;
        staticObjects.push_back(StaticObject::CreateCube(glm::vec3(x, leafY2, z), glm::vec3(leafSize2), 45.0f, leavesTex));
    }

    
    std::vector<glm::vec3> lightPositions;
    for (int i = 0; i < numStreetLights; i++) {
        float angle = (glm::two_pi<float>() / numStreetLights) * i;
        float spawnRadius = outerRadius + 4.0f;

        float x = spawnRadius * cos(angle);
        float z = spawnRadius * sin(angle);
        float y = myTerrain.GetHeight(x, z);

        float poleRadius = 0.3f;
        float poleHeight = 16.0f;

        staticObjects.push_back(StaticObject::CreateCylinder(glm::vec3(x, y, z), poleRadius, poleHeight, 12, 0.0f, poleTex));

        float boxSize = 1.2f;
        float boxY = y + poleHeight;
        staticObjects.push_back(StaticObject::CreateCube(glm::vec3(x, boxY, z), glm::vec3(boxSize), 0.0f, lightTex));

        lightPositions.push_back(glm::vec3(x, boxY, z));
    }

    carPos.y = myTerrain.GetHeight(carPos.x, carPos.z);

    objectShader.use();
    objectShader.setInt("texture_diffuse", 0);
    terrainShader.use();
    terrainShader.setInt("texture_diffuse", 0);
    roadShader.use();
    roadShader.setInt("texture_diffuse", 0);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, myTerrain, staticObjects);

        

        
        npcPresetAngle += npcPresetSpeed * deltaTime; 
        float middleRadius = (innerRadius + outerRadius) / 2.0f; 
        npcPresetPos.x = middleRadius * cos(npcPresetAngle);
        npcPresetPos.z = middleRadius * sin(npcPresetAngle);
        npcPresetPos.y = myTerrain.GetHeight(npcPresetPos.x, npcPresetPos.z); 

       
        randomMoveTimer += deltaTime;
        if (randomMoveTimer > 1.5f) { 
            float randomAngle = static_cast<float>(rand() % 360);
            float rad = glm::radians(randomAngle);
            npcRandomDirection = glm::vec3(cos(rad), 0.0f, sin(rad));
            randomMoveTimer = 0.0f; 
        }
        
        npcRandomPos += npcRandomDirection * npcRandomSpeed * deltaTime;

        
        if (glm::length(npcRandomPos) > innerRadius - 10.0f) {
            npcRandomDirection = -npcRandomDirection; 
        }
        npcRandomPos.y = myTerrain.GetHeight(npcRandomPos.x, npcRandomPos.z);

        

        if (camera.Position.y < 5.0f)
            camera.Position.y = 5.0f;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 2000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        mySkybox.Draw(skyboxShader, view, projection);
        myTerrain.Draw(terrainShader, view, projection);

        glClear(GL_DEPTH_BUFFER_BIT);
        myRoad.Draw(roadShader, view, projection);

        
        std::vector<Shader*> lightingShaders = { &objectShader, &terrainShader, &roadShader };
        for (Shader* shader : lightingShaders) {
            shader->use();
            shader->setVec3("viewPos", camera.Position);
            shader->setVec3("dirLightDir", glm::vec3(-0.3f, -1.0f, -0.4f));
            shader->setVec3("dirLightColor", glm::vec3(0.7f, 0.7f, 0.6f));

            for (int i = 0; i < numStreetLights; i++) {
                std::string baseName = "pointLights[" + std::to_string(i) + "].";
                shader->setVec3(baseName + "position", lightPositions[i]);
                shader->setVec3(baseName + "color", glm::vec3(1.0f, 0.85f, 0.5f));
                shader->setFloat(baseName + "constant", 1.0f);
                shader->setFloat(baseName + "linear", 0.007f);
                shader->setFloat(baseName + "quadratic", 0.0002f);
            }
        }

       
        for (const auto& obj : staticObjects) {
            obj.Draw(objectShader, view, projection);
        }

        
        StaticObject visualCar = StaticObject::CreateCube(carPos, carSize, carAngle, carTex);
        visualCar.Draw(objectShader, view, projection);
        visualCar.Cleanup();

       
        float npcPresetRotation = -glm::degrees(npcPresetAngle) + 90.0f;
        StaticObject visualPresetNpc = StaticObject::CreateCube(npcPresetPos, npcPresetSize, npcPresetRotation, presetNpcTex);
        visualPresetNpc.Draw(objectShader, view, projection);
        visualPresetNpc.Cleanup();

        
        StaticObject visualRandomNpc = StaticObject::CreateCube(npcRandomPos, npcRandomSize, 0.0f, randomNpcTex);
        visualRandomNpc.Draw(objectShader, view, projection);
        visualRandomNpc.Cleanup();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (auto& obj : staticObjects) {
        obj.Cleanup();
    }

    glDeleteTextures(1, &buildingTex);
    glDeleteTextures(1, &barkTex);
    glDeleteTextures(1, &leavesTex);
    glDeleteTextures(1, &poleTex);
    glDeleteTextures(1, &lightTex);
    glDeleteTextures(1, &carTex);
    glDeleteTextures(1, &presetNpcTex);
    glDeleteTextures(1, &randomNpcTex);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window, const Terrain& terrain, const std::vector<StaticObject>& objects)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    
    glm::vec3 oldPos = carPos;

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        carAngle += 100.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        carAngle -= 100.0f * deltaTime;

    float rad = glm::radians(carAngle);
    glm::vec3 forwardDirection(sin(rad), 0.0f, cos(rad));

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        carPos += forwardDirection * carSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        carPos -= forwardDirection * carSpeed * deltaTime;

    carPos.y = terrain.GetHeight(carPos.x, carPos.z);

    
    for (const auto& obj : objects) {
        if (obj.CheckCollision(carPos, carSize)) {
            carPos = oldPos;
            break;
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
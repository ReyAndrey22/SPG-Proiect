#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <ctime>   

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proiect SPG - Circuit Stradal", nullptr, nullptr);
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

    
    std::vector<StaticObject> staticObjects;
    srand(static_cast<unsigned int>(time(0)));

    int numBuildings = 15;
    int numTrees = 35;

    
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

        staticObjects.push_back(StaticObject::CreateCube(glm::vec3(x, y, z), glm::vec3(w, h, d), randomRotation, "textures/medieval_red_brick.png"));
    }

    
    for (int i = 0; i < numTrees; i++) {
        float angle = (glm::two_pi<float>() / numTrees) * i + ((rand() % 100) / 200.0f);

        float spawnRadius = 0.0f;
        if (i % 2 == 0) {
            spawnRadius = innerRadius - 15.0f - (rand() % 25); 
        }
        else {
            spawnRadius = outerRadius + 8.0f + (rand() % 12);   
        }

        float x = spawnRadius * cos(angle);
        float z = spawnRadius * sin(angle);
        float y = myTerrain.GetHeight(x, z);

        float radius = 1.8f + (rand() % 15) * 0.1f; 
        float height = 12.0f + (rand() % 12);         

        staticObjects.push_back(StaticObject::CreateCylinder(glm::vec3(x, y, z), radius, height, 16, 0.0f, "textures/pine_bark.png"));
    }

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

        processInput(window);

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

       
        for (const auto& obj : staticObjects) {
            obj.Draw(objectShader, view, projection);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (auto& obj : staticObjects) {
        obj.Cleanup();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
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
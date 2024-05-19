#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cstdlib> 
#include <ctime>

#include <glew.h>

#define GLM_FORCE_CTOR_INIT 
#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>

#define STB_IMAGE_IMPLEMENTATION

#include "Camera.h"
#include "Shader.h"
#include "Model.h"
#include "StaticObject.h"

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
bool isDayTime = false;



Camera* pCamera = nullptr;

StaticObject* currentObject;

std::vector<std::pair<float, float>> grassPositions;

bool cameraOnGround = false;


unsigned int CreateTexture(const std::string& strTexturePath)
{
    unsigned int textureId = -1;

    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(strTexturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Failed to load texture: " << strTexturePath << std::endl;
    }
    stbi_image_free(data);

    return textureId;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


void GeneratePositions();

void renderScene(const Shader& shader);
void renderFloor();
void renderGrass(const Shader& shaderBlending);
void renderModel(Shader& ourShader, Model& ourModel, const glm::vec3& position, float rotationAngle, const glm::vec3& scale);
void renderModelRotationX(Shader& ourShader, Model& ourModel, const glm::vec3& position, float rotationAngle, const glm::vec3& scale);
void renderModelRotationXYZ(Shader& ourShader, Model& ourModel, const glm::vec3& position, float rotationAngleX, float rotationAngleY, float rotationAngleZ, const glm::vec3& scale);
void renderPlusRoad(Shader& ourShader, Model& ourModel, const glm::vec3& position, float rotationAngle, const glm::vec3& scale);

double deltaTime = 0.0f; 
double lastFrame = 0.0f;

float skyboxVertices[] = {
    -1.0f,  -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
     1.0f, -1.0f, -1.0f,
     -1.0f, -1.0f, -1.0f,
     -1.0f, 1.0f, 1.0f,
     1.0f,  1.0f, 1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
    1,2,6,
    6,5,1,
    0,4,7,
    7,3,0,
    4,5,6,
    6,7,4,
    0,3,2,
    2,1,0,
    0,1,5,
    5,4,0,
    3,7,6,
    6,2,3
};




// Grass VAO si VBO



Model dilophosaurusModel, parasourModel,crowModel,batalionModel,cormorantModel ,corythosaurModel,dodoModel,parrotModel,styracosaurusModel,trexModel,parcelModel, treeModel, fountainModel;
StaticObject dilophosaurusObject, parasourObject,crowObject,batalionObject,cormorantObject,corythosaurObject,dodoObject,parrotObject,styracosaurusObject,trexObject,parcelObject, treeObject, fountainObject;

std::vector<std::string> facesDay
{
    "..\\skybox_images\\skybox_right.jpg",
    "..\\skybox_images\\skybox_left.jpg",
    "..\\skybox_images\\skybox_top.jpg",
    "..\\skybox_images\\skybox_bottom.jpg",
    "..\\skybox_images\\skybox_back.jpg",
    "..\\skybox_images\\skybox_front.jpg"
};

std::vector<std::string>facesNight
{

    "..\\skybox_images_night\\skybox_night_front.jpg",
    "..\\skybox_images_night\\skybox_night_back.jpg",
    "..\\skybox_images_night\\skybox_night_left.jpg",
    "..\\skybox_images_night\\skybox_night_right.jpg",
    "..\\skybox_images_night\\skybox_night_top.jpg",
    "..\\skybox_images_night\\skybox_night_bottom.jpg"
};


float blendFactor = 0;
float ambientFactor = 0.9;


std::vector<glm::vec3> treePositions = {
    glm::vec3(-34.0f, -0.55f, -27.0f),
    glm::vec3(37.0f, -0.55f, 29.0f),
    glm::vec3(12.0f, -0.55f, 35.0f),
    glm::vec3(40.0f, -0.55f, 8.0f),
    glm::vec3(15.0f, -0.55f, -31.0f),
    glm::vec3(-10.0f, -0.55f, 19.0f),
    glm::vec3(-30.0f, -0.55f, 15.0f),
    glm::vec3(-13.0f, -0.55f, -30.0f),
    glm::vec3(30.0f, -0.55f, -15.0f),
};

std::vector<glm::vec3> batalionPositions = {
    glm::vec3(36.0f, -0.5f, 30.0f),
    glm::vec3(38.0f, -0.5f, 28.0f),
    glm::vec3(13.0f, -0.5f, 34.0f),
    glm::vec3(11.0f, -0.5f, 36.0f),
    glm::vec3(39.0f, -0.5f, 9.0f),
    glm::vec3(41.0f, -0.5f, 7.0f),
};

std::vector<glm::vec3> crowPositions = {
    glm::vec3(10.0f, 2.0f, 8.0f),
    glm::vec3(21.0f, 2.5f, 15.0f),
    glm::vec3(35.0f, 3.0f, 20.0f),
    glm::vec3(18.0f, 2.5f, 35.0f),
};

glm::vec3 lightPos(-50.0f, 10.0f, 0.0f);

int main(int argc, char** argv)
{

    GeneratePositions();
    
    std::string strFullExeFileName = argv[0];
    std::string strExePath;
    const size_t last_slash_idx = strFullExeFileName.rfind('\\');
    if (std::string::npos != last_slash_idx)
    {
        strExePath = strFullExeFileName.substr(0, last_slash_idx);
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Muzeu", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glewInit();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 7.0f, 25.0f));

    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader shaderBlending("Blending.vs", "Blending.fs");
    shaderBlending.SetInt("texture1", 0);
    Shader shadowMappingShader("ShadowMapping.vs", "ShadowMapping.fs");
    Shader shadowMappingDepthShader("ShadowMappingDepth.vs", "ShadowMappingDepth.fs");
    Shader ModelShader("ModelShader.vs", "ModelShader.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");
    

    // load textures
    // -------------

unsigned int grassTexture = CreateTexture("..\\Textures\\grass3.png");
    unsigned int floorTexture = CreateTexture("..\\Textures\\Grass.jpg");
    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // shader configuration
    // --------------------
    shadowMappingShader.Use();
    shadowMappingShader.SetInt("diffuseTexture", 0);
    shadowMappingShader.SetInt("shadowMap", 1);

    // lighting info
    // -------------
   

    glEnable(GL_CULL_FACE);

    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    unsigned int cubemapTexture;

    unsigned int cubemapTextureNight;

    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < facesDay.size(); i++)
    {
        unsigned char* data = stbi_load(facesDay[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D
            (
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );
            glGetError();
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << facesDay[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glGenTextures(1, &cubemapTextureNight);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureNight);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    //night skybox
    for (unsigned int i = 0; i < facesNight.size(); i++) {
        unsigned char* data = stbi_load(facesNight[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            glGetError();
            stbi_image_free(data);
        }
        else {
            std::cout << "Cubemap texture failed to load at path: " << facesNight[i] << std::endl;
        }
    }

    treeModel = Model("..\\Models\\Tree\\Quercus glauca_FREEMODEL_std_obj.obj");
    treeObject = StaticObject(treeModel, SCR_WIDTH, SCR_HEIGHT, glm::vec3(-10.0f, -0.55f, -10.0f));

    parcelModel = Model("..\\Models\\Parcel\\cobblestone.obj");
    parcelObject = StaticObject(parcelModel, SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, -0.4f, 0.0f));
    parcelObject.SetRotation(0.0f);

    currentObject = &parcelObject;

    fountainModel = Model("..\\Models\\Fountain\\objFountain.obj");
    fountainObject = StaticObject(fountainModel, SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.2f, -0.0f, 0.5f));

    dilophosaurusModel = Model("..\\Models\\dilophosaurus\\dilophosaurus.obj");
    dilophosaurusObject = StaticObject(dilophosaurusModel, SCR_WIDTH, SCR_HEIGHT, glm::vec3(-10.0f, 1.7f, -15.0f));

    cormorantModel = Model("..\\Models\\Cormorant\\NHMW-Zoo1-Vogel_Galapagos Cormorant_low res.obj");
    cormorantObject = StaticObject(cormorantModel, SCR_WIDTH, SCR_HEIGHT, glm::vec3(18.0f, 4.0f, 12.0f));
    cormorantObject.SetRotation(90.0f);

    crowModel = Model("..\\Models\\AmericanCrow\\AmericanCrow.obj");
    crowObject = StaticObject(crowModel, SCR_WIDTH, SCR_HEIGHT, glm::vec3(10.0f, 1.0f, 5.0f));
    crowObject.SetRotation(90.0f);

    batalionModel = Model("..\\Models\\Batalion_2\\Batalion_2.obj");
    batalionObject = StaticObject(batalionModel, SCR_WIDTH, SCR_HEIGHT, glm::vec3(36.0f, -0.5f, 30.0f));


   dodoModel = Model("..\\Models\\dodo\\dodo.obj");
    dodoObject = StaticObject(dodoModel, SCR_WIDTH, SCR_HEIGHT, glm::vec3(15.0f, 0.20f, -12.0f));
    dodoObject.SetRotation(20.0f);

    corythosaurModel = Model("..\\Models\\Corythosaurus-OBJ\\corythosaurus.obj");
    corythosaurObject = StaticObject(corythosaurModel, SCR_WIDTH, SCR_HEIGHT, glm::vec3(-32.0f, -4.0f, -10.0f));
    corythosaurObject.SetRotation(90.0f);

    parasourModel = Model("..\\Models\\parasaurolophus-toy-OBJ\\parasaurolophus-toy.obj");
    parasourObject = StaticObject(parasourModel, SCR_WIDTH, SCR_HEIGHT, glm::vec3(-15.0f, -0.8f, 10.0f));
    parasourObject.SetRotation(90.0f);

    styracosaurusModel = Model("..\\Models\\styracosaurus\\styracosaurus.obj");
    styracosaurusObject = StaticObject(styracosaurusModel, SCR_WIDTH, SCR_HEIGHT, glm::vec3(-20.0f, -5.0f, 35.0f));
    styracosaurusObject.SetRotation(90.0f);

    trexModel = Model("..\\Models\\trex\\t-rex.obj");
    trexObject = StaticObject(trexModel, SCR_WIDTH, SCR_HEIGHT, glm::vec3(30.0f, -0.6f, -31.0f));


    while (!glfwWindowShouldClose(window))
    {

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureNight);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
        glDepthMask(GL_TRUE);
        double currentFrame = glfwGetTime();

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;

        glm::mat4 lightRotationMatrix = glm::mat4(
            glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
            glm::vec4(sin(lastFrame), 0.0f, cos(lastFrame), 1.0f)
        );

        glm::vec4 rotatedLightPos = lightRotationMatrix * glm::vec4(lightPos, 1.0f);

        float near_plane = 1.0f, far_plane = 100.0f;
        lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;


        // render scene from light's point of view
        shadowMappingDepthShader.Use();
        shadowMappingDepthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        renderScene(shadowMappingDepthShader);

        renderModel(shadowMappingDepthShader, dilophosaurusObject.GetModel(), dilophosaurusObject.GetPosition(), dilophosaurusObject.GetRotation(), glm::vec3(2.0f));
        renderModelRotationX(shadowMappingDepthShader, parasourObject.GetModel(), parasourObject.GetPosition(), parasourObject.GetRotation(), glm::vec3(0.1f));
        renderModelRotationX(shadowMappingDepthShader, corythosaurObject.GetModel(), corythosaurObject.GetPosition(), corythosaurObject.GetRotation(), glm::vec3(0.75f));
        renderModelRotationXYZ(shadowMappingDepthShader, styracosaurusObject.GetModel(), styracosaurusObject.GetPosition(), styracosaurusObject.GetRotation(), 5.0f, 0.0f, glm::vec3(0.75f));
        renderModelRotationXYZ(shadowMappingDepthShader, dodoObject.GetModel(), dodoObject.GetPosition(), dodoObject.GetRotation(), 0.0f, 8.0f, glm::vec3(0.002f));
        renderModel(shadowMappingDepthShader, trexObject.GetModel(), trexObject.GetPosition(), trexObject.GetRotation(), glm::vec3(1.75f));
       
        for (auto position : crowPositions)
        renderModelRotationX(shadowMappingDepthShader, crowObject.GetModel(), position, crowObject.GetRotation(), glm::vec3(1.0f));
        
        for (auto position : batalionPositions)
        renderModel(shadowMappingDepthShader, batalionObject.GetModel(), position, batalionObject.GetRotation(), glm::vec3(1.0f));
        
        renderModelRotationX(shadowMappingDepthShader, cormorantObject.GetModel(), cormorantObject.GetPosition(), cormorantObject.GetRotation(), glm::vec3(0.008f));
       
        for(auto position : treePositions)
            renderModel(shadowMappingDepthShader, treeObject.GetModel(), position, treeObject.GetRotation(), glm::vec3(0.001f));
        
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shadowMappingShader.Use();
        glm::mat4 projection = pCamera->GetProjectionMatrix();
        glm::mat4 view = pCamera->GetViewMatrix(currentObject);
        shadowMappingShader.SetMat4("projection", projection);
        shadowMappingShader.SetMat4("view", view);
        shadowMappingShader.SetFloat("ambientFactor", ambientFactor);
        // set light uniforms
        shadowMappingShader.SetVec3("viewPos", pCamera->GetPosition());
        shadowMappingShader.SetVec3("lightPos", lightPos);
        shadowMappingShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glDisable(GL_CULL_FACE);
        renderScene(shadowMappingShader);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        renderGrass(shaderBlending);
           
       
        


        renderModel(ModelShader, dilophosaurusObject.GetModel(), dilophosaurusObject.GetPosition(), dilophosaurusObject.GetRotation(), glm::vec3(2.0f));
        renderModelRotationX(ModelShader, parasourObject.GetModel(), parasourObject.GetPosition(), parasourObject.GetRotation(), glm::vec3(0.1f));
        renderModelRotationX(ModelShader, corythosaurObject.GetModel(), corythosaurObject.GetPosition(), corythosaurObject.GetRotation(), glm::vec3(0.75f));
        renderModelRotationXYZ(ModelShader, styracosaurusObject.GetModel(), styracosaurusObject.GetPosition(), styracosaurusObject.GetRotation(), 5.0f, 0.0f, glm::vec3(0.75f));
        renderModelRotationXYZ(ModelShader, dodoObject.GetModel(), dodoObject.GetPosition(), dodoObject.GetRotation(), 0.0f, 8.0f, glm::vec3(0.002f));
        renderModel(ModelShader, trexObject.GetModel(), trexObject.GetPosition(), trexObject.GetRotation(), glm::vec3(1.75f));
        

        for (auto position : crowPositions)
        renderModelRotationX(ModelShader, crowObject.GetModel(), position, crowObject.GetRotation(), glm::vec3(1.0f));
        
        for (auto position : batalionPositions)
        renderModel(ModelShader, batalionObject.GetModel(), position, batalionObject.GetRotation(), glm::vec3(1.0f));
        
        renderModelRotationX(ModelShader, cormorantObject.GetModel(), cormorantObject.GetPosition(), cormorantObject.GetRotation(), glm::vec3(0.008f));
        

        for (auto position : treePositions)
            renderModel(ModelShader, treeObject.GetModel(), position, treeObject.GetRotation(), glm::vec3(0.001f));
        
        renderPlusRoad(ModelShader, parcelObject.GetModel(), parcelObject.GetPosition(), parcelObject.GetRotation(), glm::vec3(1.0f));

        renderModel(ModelShader, fountainObject.GetModel(), fountainObject.GetPosition(), fountainObject.GetRotation(), glm::vec3(1.0f));

        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // White light
        glm::vec3 lightDir = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)); // Example direction
        glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f); // Example color (rust)

        ModelShader.Use();
        ModelShader.SetVec3("lightColor", lightColor);
        ModelShader.SetVec3("lightDir", lightDir);
        ModelShader.SetVec3("objectColor", objectColor);

        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        skyboxShader.Use();

        skyboxShader.SetInt("skybox1", 0);
        skyboxShader.SetInt("skybox2", 1);
        skyboxShader.SetFloat("blendFactor", blendFactor);

        glm::mat4 viewSB = glm::mat4(glm::mat3(pCamera->GetViewMatrix(currentObject)));

        glm::mat4 projectionSB = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        skyboxShader.SetMat4("view", viewSB);
        skyboxShader.SetMat4("projection", projectionSB);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureNight);

        glBindVertexArray(skyboxVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete pCamera;
    glfwTerminate();
    return 0;
}

// renders the 3D scene
// --------------------
void renderScene(const Shader& shader)
{

    // floor
    glm::mat4 model;
   
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    shader.SetMat4("model", model);
    renderFloor();

}


void renderPlusRoad(Shader& ourShader, Model& ourModel, const glm::vec3& position, float rotationAngle, const glm::vec3& scale) {
    ourShader.Use();
    glm::mat4 viewMatrix = pCamera->GetViewMatrix(currentObject);
    glm::mat4 projectionMatrix = pCamera->GetProjectionMatrix();

    // Render the central tile
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    model = glm::scale(model, scale);
    ourShader.SetMat4("model", model);
    ourShader.SetMat4("view", viewMatrix);
    ourShader.SetMat4("projection", projectionMatrix);
    ourModel.Draw(ourShader);

    // Render tiles in the four directions
    int numTiles = 8; // Number of tiles in each direction from the center
    float tileSpacing = 5.65f; // Distance between each tile

    for (int i = 1; i <= numTiles; ++i) {
        // Right direction
        model = glm::mat4(1.0f);
        model = glm::translate(model, position + glm::vec3(i * tileSpacing, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        model = glm::scale(model, scale);
        ourShader.SetMat4("model", model);
        ourModel.Draw(ourShader);

        // Left direction
        model = glm::mat4(50.0f);
        model = glm::translate(model, position + glm::vec3(-i * tileSpacing, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        model = glm::scale(model, scale);
        ourShader.SetMat4("model", model);
        ourModel.Draw(ourShader);

        // Up direction
        model = glm::mat4(1.0f);
        model = glm::translate(model, position + glm::vec3(0.0f, 0.0f, i * tileSpacing));
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        model = glm::scale(model, scale);
        ourShader.SetMat4("model", model);
        ourModel.Draw(ourShader);

        // Down direction
        model = glm::mat4(1.0f);
        model = glm::translate(model, position + glm::vec3(0.0f, 0.0f, -i * tileSpacing));
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        model = glm::scale(model, scale);
        ourShader.SetMat4("model", model);
        ourModel.Draw(ourShader);
    }
}



unsigned int planeVAO = 0;
void renderFloor()
{
    unsigned int planeVBO;

    if (planeVAO == 0)
    {
        float planeVertices[] = {
            50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  50.0f,  0.0f,
            -50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
            -50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,   0.0f, 50.0f,

            50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  50.0f,  0.0f,
            -50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,   0.0f, 50.0f,
            50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,  50.0f, 50.0f
        };
        // plane VAO
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);
    }

    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void renderGrass(const Shader& shaderBlending)
{

    float grassVertices[] = {
       0.0f, 0.5f, 0.0f,  0.0f, 0.0f,
       0.0f, -0.5f, 0.0f,  0.0f, 1.0f,
       1.0f, -0.5f, 0.0f,  1.0f, 1.0f,

       0.0f, 0.5f, 0.0f,  0.0f, 0.0f,
       1.0f, -0.5f, 0.0f,  1.0f, 1.0f,
       1.0f, 0.5f, 0.0f,  1.0f, 0.0f

    };

    unsigned int grassVAO, grassVBO;
    glGenVertexArrays(1, &grassVAO);
    glGenBuffers(1, &grassVBO);
    glBindVertexArray(grassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), &grassVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


    glm::mat4 projection1 = pCamera->GetProjectionMatrix();
    glm::mat4 view1 = pCamera->GetViewMatrix(currentObject);

    shaderBlending.Use();
    shaderBlending.SetMat4("projection", projection1);
    shaderBlending.SetMat4("view", view1);

   
   

    for (int i = 0; i < grassPositions.size(); ++i) {
            
        float xPos = grassPositions[i].first;
        float zPos = grassPositions[i].second;
            glBindVertexArray(grassVAO);
           
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(xPos, 0.0f, zPos));
            model = glm::scale(model, glm::vec3(1.0f));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            shaderBlending.SetMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(grassVAO);
           
            model = glm::mat4();
            model = glm::translate(model, glm::vec3(xPos, 0.0f, zPos));
            model = glm::scale(model, glm::vec3(1.0f));
            model = glm::rotate(model, 1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, glm::vec3(-0.5f, 0.0f, 0.5f));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            shaderBlending.SetMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(grassVAO);
            
            model = glm::mat4();
            model = glm::translate(model, glm::vec3(xPos, 0.0f, zPos));
            model = glm::scale(model, glm::vec3(1.0f));
            model = glm::rotate(model, 0.8f, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, glm::vec3(-0.2f, 0.0f, 0.35f));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            shaderBlending.SetMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(grassVAO);
           
            model = glm::mat4();
            model = glm::translate(model, glm::vec3(xPos, 0.0f, zPos));
            model = glm::scale(model, glm::vec3(1.0f));
            model = glm::rotate(model, 0.8f, glm::vec3(0.0f, -1.0f, 0.0f));
            model = glm::translate(model, glm::vec3(-0.2f, 0.0f, -0.35f));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            shaderBlending.SetMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        
    }
    
}

unsigned int modelVAO = 0;
unsigned int modelVBO = 0;
unsigned int modelEBO;
void renderModel(Shader& ourShader, Model& ourModel, const glm::vec3& position, float rotationAngle, const glm::vec3& scale)
{
    ourShader.Use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, scale);

    glm::mat4 viewMatrix = pCamera->GetViewMatrix(currentObject);
    glm::mat4 projectionMatrix = pCamera->GetProjectionMatrix();

    ourShader.SetMat4("model", model);
    ourShader.SetMat4("view", viewMatrix);
    ourShader.SetMat4("projection", projectionMatrix);

    ourModel.Draw(ourShader);
}




void GeneratePositions()
{

    srand(time(nullptr));

    while (grassPositions.size() < 200)
    {
        float xPos = rand() % 97 - 48;
        float zPos = rand() % 97 - 48;
        if ((xPos < -5 || xPos>5) && (zPos < -5 || zPos >5))
            grassPositions.push_back(std::make_pair(xPos, zPos));
    }
}





void renderModelRotationX(Shader& ourShader, Model& ourModel, const glm::vec3& position, float rotationAngle, const glm::vec3& scale)
{
    ourShader.Use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::scale(model, scale);

    glm::mat4 viewMatrix = pCamera->GetViewMatrix(currentObject);
    glm::mat4 projectionMatrix = pCamera->GetProjectionMatrix();

    ourShader.SetMat4("model", model);
    ourShader.SetMat4("view", viewMatrix);
    ourShader.SetMat4("projection", projectionMatrix);

    ourModel.Draw(ourShader);
}




void renderModelRotationXYZ(Shader& ourShader, Model& ourModel, const glm::vec3& position, float rotationAngleX, float rotationAngleY, float rotationAngleZ, const glm::vec3& scale)
{
    ourShader.Use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotationAngleX), glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotationAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotationAngleZ), glm::vec3(0.0f, 0.0f, -1.0f));
    
    model = glm::scale(model, scale);

    glm::mat4 viewMatrix = pCamera->GetViewMatrix(currentObject);
    glm::mat4 projectionMatrix = pCamera->GetProjectionMatrix();

    ourShader.SetMat4("model", model);
    ourShader.SetMat4("view", viewMatrix);
    ourShader.SetMat4("projection", projectionMatrix);

    ourModel.Draw(ourShader);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        blendFactor = std::min(blendFactor + 0.01, 1.0);
        ambientFactor = std::max(ambientFactor - 0.01, 0.34);
        lightPos[0] += 1.1f;
        if (lightPos[0] > 40.0f)
            lightPos[1] = -2.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        blendFactor = std::max(blendFactor - 0.01, 0.0);
        ambientFactor = std::min(ambientFactor + 0.01, 0.9);
        lightPos[0] -= 1.1f;
        if (lightPos[0] < 40.0f)
            lightPos[1] = 10.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        cameraOnGround = true;
        pCamera->SetCameraOnGround(cameraOnGround);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraOnGround = false;
        pCamera->SetCameraOnGround(cameraOnGround);
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        pCamera->ProcessKeyboard(LEFT, (float)deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        pCamera->ProcessKeyboard(RIGHT, (float)deltaTime);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        pCamera->ProcessKeyboard(FORWARD, (float)deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pCamera->ProcessKeyboard(BACKWARD, (float)deltaTime);
    if (!cameraOnGround)
    {
        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
            pCamera->ProcessKeyboard(UP, (float)deltaTime);
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
            pCamera->ProcessKeyboard(DOWN, (float)deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        pCamera->Reset(width, height);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    pCamera->Reshape(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    pCamera->MouseControl((float)xpos, (float)ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yOffset)
{
    pCamera->ProcessMouseScroll((float)yOffset);
}
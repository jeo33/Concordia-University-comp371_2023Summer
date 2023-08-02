//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//
// Inspired by the following tutorials:
// - https://learnopengl.com/Getting-started/Hello-Window
// - https://learnopengl.com/Getting-started/Hello-Triangle

#include <iostream>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs
#include <random>
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include "shaderloader.h"
#include"stb_image.h"
#include"Cube.h"
#include"Sphere.h"
using namespace glm;
using namespace std;
glm::vec3 cameraPos = glm::vec3(0.0f, 20.0f, 65.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaFrame, LastFrame;

float toggle_texture = 1.0;

float toggle_shadow = 1.0;
GLuint loadTexture(const char* filename);

mat4 base;

mat4 Groupbase;
double dx, dy;
float dt;
float sensitivity = 0.01;
bool flag1 = false;
bool flag2 = false;
bool flag3 = false;

int renderingMode = GL_FILL; // Default: Filled triangles

std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister 19937 generator

// Define the range for the x and y coordinates (inclusive)
int minX = -50;
int maxX = 50;
int minY = -50;
int maxY = 50;
std::uniform_int_distribution<> distX(minX, maxX);
std::uniform_int_distribution<> distY(minY, maxY);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS)
    {

        if (key == GLFW_KEY_Y) {

            toggle_texture = (toggle_texture == 1.0) ? 0.0 : 1.0;

        }

        if (key == GLFW_KEY_O) {

            toggle_shadow = (toggle_shadow == 1.0) ? 0.0 : 1.0;

        }

        
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        flag1 = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        flag1 = !true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        flag2 = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        flag2 = !true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        flag3 = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        flag3 = !true;
    }
}

int main(int argc, char* argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //opengl profile to be called , here is the profile.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Comp371 - Assignment2 JUNPENG GAI", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::string shaderPathPrefix = "../Assets/Shaders/";
    std::string TexturePathPrefix = "../Assets/Textures/";

    GLuint Textureshader = loadSHADER(shaderPathPrefix + "TexturedVertexShader.glsl",
        shaderPathPrefix + "TexturedFragmentShader.glsl");
    GLuint Lightshader = loadSHADER(shaderPathPrefix + "lightshadervertex.glsl",
        shaderPathPrefix + "lightshaderFragment.glsl");
    GLuint simpleshader = loadSHADER(shaderPathPrefix + "simpleshadervertex.glsl",
        shaderPathPrefix + "simpleshaderFragment.glsl");


    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


    GLuint texture = loadTexture("../Assets/Textures/brick.jpg");

    GLuint Claytexture = loadTexture("../Assets/Textures/clay.jpg");

    GLuint Blacktexture = loadTexture("../Assets/Textures/black.jpg");

    GLuint Whitetexture = loadTexture("../Assets/Textures/White.jpg");

    GLuint Greentexture = loadTexture("../Assets/Textures/green.jpg");
    GLuint Xtexture = loadTexture("../Assets/Textures/green.jpg");

    GLuint Ytexture = loadTexture("../Assets/Textures/y.jpg");
    GLuint Ztexture = loadTexture("../Assets/Textures/red.jpg");
    GLuint armtexture = loadTexture("../Assets/Textures/skin.jpg");



    Sphere LightSource(5, 10.0, 10.0);
    Sphere TennisBall(1, 10.0, 10.0);

    Cube cube(1, 1.0, 1.0);
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Other camera parameters
    float cameraSpeed = 1.0f;
    float cameraFastSpeed = 2 * cameraSpeed;
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;
    bool  cameraFirstPerson = true; // press 1 or 2 to toggle this variable

    //those are the racket parameters
    float angle1 = 45.0;
    float angle2 = 50.0;
    float angle3 = 40;
    float angle4 = 30;
    float AxisRotationSpeed = 3.0;
    float armlength = 10;
    float WhiteEdge1Length = 7;
    float RedEdge1Length = 12;
    float WhiteEdge2Length = 2;
    float RedEdge2Length = 2;
    float TopBardLength = 5;
    float gridUnit = 1.0;
    float fov = 70;
    float netLengthY = RedEdge1Length + WhiteEdge2Length * cos(radians(angle3)) + RedEdge2Length * cos(radians(angle4)) + 1;
    float netLengthX = 2 * WhiteEdge1Length * sin(radians(angle2)) - 1;
    //racket parts
    glm::mat4 Tlowerarm, Tupperarm, Tpole, TleftWhiteEdge1, TleftRedEdge1, TleftWhiteEdge2, TleftRedEdge2, TtopRedEdge3, net;
    // Create uniform distributions for x and y coordinates

    int controlmode = 0;
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);



    mat4 base = mat4(1.0);
    mat4 racket1Base = mat4(1.0);
    mat4 racket2Base = mat4(1.0);
    mat4 racket3Base = mat4(1.0);
    mat4 racket4Base = mat4(1.0);
    mat4 Groupbase = mat4(1.0) * scale(mat4(1.0), vec3(0.5));

    glm::mat4 lowerJoint = base * Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));

    mat4 upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
    mat4 lowerArmOrigion = mat4(1.0);
    mat4 upperArmOrigion = mat4(1.0);
    vec3 LightPos = vec3(0.0, 30, 10);
    mat4 view = mat4(1.0);
    mat4 projection = mat4(1.0f);
    mat4 model = mat4(1.0);
    vec3 lightC = vec3(1.0f, 1.0f, 1.0f);
    float strength = 0.1;
    float v = -1.0f;

    while (!glfwWindowShouldClose(window))
    {
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        dx = mousePosX - lastMousePosX;
        dy = mousePosY - lastMousePosY;


        glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);

        projection = perspective(radians(fov), 1024.0f / 768.0f,  // aspect ratio
            0.01f, 200.0f);
        float currentFrame = glfwGetTime();
        deltaFrame = currentFrame - LastFrame;
        LastFrame = currentFrame;

        glPolygonMode(GL_FRONT_AND_BACK, renderingMode);

        //ConfigureShaderAndMatrices();
        float near_plane = 1.0f, far_plane = 200.0f;
        glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(LightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        glUseProgram(simpleshader);


        GLuint lightSpaceMatrixLocation = glGetUniformLocation(simpleshader, "lightSpaceMatrix");
        glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);





        glBindVertexArray(cube.VAO);


        glClear(GL_DEPTH_BUFFER_BIT);
        //RenderScene(simpleDepthShader);


        LightPos = vec3(75 * sin((float)glfwGetTime()), 75, 75 * cos((float)glfwGetTime()));

      

        glBindVertexArray(cube.VAO);
        model = mat4(1.0f);
        model = base * translate(model, vec3(0.0, -0.4, 0.0)) * scale(mat4(1.0f), vec3(36.0, 1.0, 78.0));
        GLint TextureshaderModelLocation = glGetUniformLocation(simpleshader, "model");
        glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);



        //100*100*1 mesh

      
        model = mat4(1.0f);
        model = base * translate(model, vec3(0.0, -0.5, 0.0)) * scale(mat4(1.0f), vec3(100.0, 1.0, 100.0));
         TextureshaderModelLocation = glGetUniformLocation(simpleshader, "model");
        glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);


        //2 poles mesh

        for (int i = 0; i < 2; i++)
        {
            
            model = mat4(1.0f);
            model = base * translate(model, vec3(18 - i * 36.0, 5.0, 0.0)) * scale(mat4(1.0f), vec3(1.0, 10.0, 1.0));
            GLint ModelLocation = glGetUniformLocation(simpleshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        }

        //x red
       
        model = mat4(1.0f);
        model = base * translate(mat4(1.0f), vec3(5 * gridUnit / 2, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5 * gridUnit, 1.0f, 1.0f));
        GLint  ModelLocation = glGetUniformLocation(simpleshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);


        //y gree
       
        model = mat4(1.0f);
        model = base * translate(mat4(1.0f), vec3(0.0f, 5 * gridUnit / 2, 0.0f)) * scale(mat4(1.0f), vec3(1.0f, 5 * gridUnit, 1.0f));
        ModelLocation = glGetUniformLocation(simpleshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        //z blue
       
        model = mat4(1.0f);
        model = base * translate(mat4(1.0f), vec3(0.0f, 0.0f, (5 * gridUnit) / 2)) * scale(mat4(1.0f), vec3(1.0f, 1.0f, 5 * gridUnit));
        ModelLocation = glGetUniformLocation(simpleshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);


        //net mesh
        //vertical
        for (int j = 0; j <= 10; j++)
        {
         
            model = mat4(1.0f);
            model = base * translate(model, vec3(0.0, 0.0 + j, 0.0)) * scale(mat4(1.0f), vec3(36.0, 0.2, 0.2));
            ModelLocation = glGetUniformLocation(simpleshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        }
        //Horizontal
        for (int j = 0; j <= 36; j++)
        {
            model = mat4(1.0f);
            model = base * translate(model, vec3(-18 + j, 5.0, 0.0)) * scale(mat4(1.0f), vec3(0.2, 10, 0.2));
            ModelLocation = glGetUniformLocation(simpleshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        }


        //lower arm   45 degree
        //coordinate base
        //groupMatrix = base * Groupbase * translate(mat4(1.0f), vec3(armlength * cos(radians(angle1)) * 0.5, armlength * cos(radians(angle1)) * 0.5, 0.0f));

        //glm::mat4 lowerJoint, upperjoint, arm_pole_Joint;
       
        //RTS
        mat4 scalingMatrix = translate(glm::mat4(1.0f), glm::vec3(0.0f, armlength * 0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, armlength, 1.0f));
        mat4 partMatrix = lowerJoint * scalingMatrix;
        model = mat4(1.0f);
        model = partMatrix;
        ModelLocation = glGetUniformLocation(simpleshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        glDrawArrays(GL_TRIANGLES, 0, 36);





        //upper arm

        //upperArmOrigion= groupMatrix*translate(mat4(1.0f), vec3(armlength * cos(radians(angle1)) * 0.5, armlength * cos(radians(angle1)) * 0.5, 0.0f));
        //groupMatrix= upperArmOrigion* translate(mat4(1.0f), vec3(armlength * cos(radians(angle1)) * 0.5, armlength * cos(radians(angle1)) * 0.5, 0.0f));

        
        //RTS
        scalingMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, armlength * 0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, armlength, 1.0f));
        partMatrix = upperjoint * scalingMatrix;
        model = mat4(1.0f);
        model = partMatrix;
        ModelLocation = glGetUniformLocation(simpleshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        //racket pole
      

        //RTS
        scalingMatrix = translate(mat4(1.0f), vec3(0.0f, 0.5 * armlength, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.6f, armlength, 0.6f));
        partMatrix = arm_pole_Joint * scalingMatrix;
        model = mat4(1.0f);
        model = partMatrix;
        ModelLocation = glGetUniformLocation(simpleshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        //WhiteEdge1 30 degree

        for (int i = 0; i < 2; i++)
        {
            //WhiteEdge1 30 degree
          

            scalingMatrix = translate(glm::mat4(1.0f), glm::vec3(0, WhiteEdge1Length * 0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, WhiteEdge1Length, 0.3f));
            TleftWhiteEdge1 = arm_pole_Joint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle2), vec3(0.0f, 0.0f, 1.0f - 2 * i));
            partMatrix = TleftWhiteEdge1 * scalingMatrix;
            model = mat4(1.0f);
            model = partMatrix;
            ModelLocation = glGetUniformLocation(simpleshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);



            //RedEdge1 30 degree
        
            scalingMatrix = translate(glm::mat4(1.0f), glm::vec3(0, RedEdge1Length * 0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, RedEdge1Length, 0.3f));
            TleftRedEdge1 = TleftWhiteEdge1 * translate(glm::mat4(1.0f), glm::vec3(0, WhiteEdge1Length, 0.0f)) * rotate(mat4(1.0f), radians(angle2), vec3(0.0f, 0.0f, -1.0f + 2 * i));
            partMatrix = TleftRedEdge1 * scalingMatrix;
            model = mat4(1.0f);
            model = partMatrix;
            ModelLocation = glGetUniformLocation(simpleshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);



            //WhiteEdge2 15 degree
          
            scalingMatrix = translate(mat4(1.0f), vec3(0.0f, WhiteEdge2Length * 0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, WhiteEdge2Length, 0.3f));
            TleftWhiteEdge2 =
                TleftRedEdge1
                * translate(glm::mat4(1.0f), glm::vec3(0, RedEdge1Length, 0.0f))
                * rotate(mat4(1.0f), radians(-angle3), vec3(0.0f, 0.0f, 1.0f - 2 * i));
            partMatrix = TleftWhiteEdge2 * scalingMatrix;
            model = mat4(1.0f);
            model = partMatrix;
            ModelLocation = glGetUniformLocation(simpleshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);


           
            scalingMatrix = translate(mat4(1.0f), vec3(0.0f, RedEdge2Length * 0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, RedEdge2Length, 0.3f));
            TleftRedEdge2 = TleftWhiteEdge2
                * translate(glm::mat4(1.0f), glm::vec3(0, WhiteEdge2Length, 0.0f))
                * rotate(mat4(1.0f), radians(-angle4), vec3(0.0f, 0.0f, 1.0f - 2 * i));
            partMatrix = TleftRedEdge2 * scalingMatrix;
            model = mat4(1.0f);
            model = partMatrix;
            ModelLocation = glGetUniformLocation(simpleshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);




        }


        // top bar
       
         scalingMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, TopBardLength * 0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, TopBardLength, 0.3f));
        TtopRedEdge3 = TleftRedEdge2
            * translate(glm::mat4(1.0f), glm::vec3(0, RedEdge2Length, 0.0f))
            * rotate(mat4(1.0f), radians(angle3 + angle4-90), vec3(0.0f, 0.0f, -1.0));
        partMatrix = TtopRedEdge3 * scalingMatrix;
        model = mat4(1.0f);
        model = partMatrix;
        ModelLocation = glGetUniformLocation(simpleshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
        
        //bot bar
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(WhiteEdge1Length * sin(radians(angle1)), 1.0, 0.3f));
        TtopRedEdge3 = arm_pole_Joint
            * translate(glm::mat4(1.0f), glm::vec3(0, WhiteEdge1Length * sin(radians(angle1)) * 0.5 + armlength, 0.0f));
        partMatrix = TtopRedEdge3 * scalingMatrix;
        model = mat4(1.0f);
        model = partMatrix;
        ModelLocation = glGetUniformLocation(simpleshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);


        for (int i = 1; i < 16; i++)
        {

            scalingMatrix = translate(mat4(1.0f), vec3(0.0, 0.0, 0.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(netLengthX, 0.5f, 0.2f));
            net = TtopRedEdge3 * translate(mat4(1.0f), vec3(0.0, i, 0.0));
            partMatrix = net * scalingMatrix;
            model = mat4(1.0f);
            model = partMatrix;
            ModelLocation = glGetUniformLocation(simpleshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
        }

        for (int i = -4; i < 5; i++)
        {

            scalingMatrix = translate(mat4(1.0f), vec3(0.0, (netLengthY - 1) * 0.5, 0.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, netLengthY - 1, 0.2f));
            net = TtopRedEdge3 * translate(mat4(1.0f), vec3(i * 1.0, 0.5, 0.0));
            partMatrix = net * scalingMatrix;
            model = mat4(1.0f);
            model = partMatrix;
            ModelLocation = glGetUniformLocation(simpleshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
        }





        //ball shadow
        glBindVertexArray(TennisBall.VAO);
        model = mat4(1.0f);
        model = lowerJoint * translate(model, vec3(0.0, 15.0, 0.0)) * scale(mat4(1.0f), vec3(3.0));
        ModelLocation = glGetUniformLocation(simpleshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);






        //rendering the scene
        glViewport(0, 0, 1024.0f, 768.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glUseProgram(Textureshader);
        //toggle_texture
        glUniform1f(glGetUniformLocation(Textureshader, "is_tex"), toggle_texture);

        glUniform1f(glGetUniformLocation(Textureshader, "is_shadow"), toggle_shadow);
        GLint colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 1.0, 174 / 255.0, 201 / 255.0);
        //Ground mesh 78*36


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Claytexture);
        GLint ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        GLint shadowMapLocation = glGetUniformLocation(Textureshader, "shadowMap");
        glUniform1i(shadowMapLocation, 1);

        glBindVertexArray(cube.VAO);
        model = mat4(1.0f);
        model = base*translate(model, vec3(0.0, -0.4, 0.0)) * scale(mat4(1.0f), vec3(36.0, 1.0, 78.0));
        GLuint lightSpaceMatrixPosition = glGetUniformLocation(Textureshader, "lightSpaceMatrix");
        glUniformMatrix4fv(lightSpaceMatrixPosition, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
        GLuint CameraPosition = glGetUniformLocation(Textureshader, "viewPoint");
        glUniform3fv(CameraPosition, 1, glm::value_ptr(cameraPos));
        GLuint AmbientStrength = glGetUniformLocation(Textureshader, "ambientStrength");
        glUniform1f(AmbientStrength, strength);
        GLuint LightColor = glGetUniformLocation(Textureshader, "lightcolor");
        glUniform3f(LightColor, 1.0f, 1.0f, 1.0f);
        GLuint LightPosition = glGetUniformLocation(Textureshader, "lightpos");
        glUniform3fv(LightPosition, 1, glm::value_ptr(LightPos));
        GLuint ViewMatrixLocation = glGetUniformLocation(Textureshader, "view");
        glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, &view[0][0]);
        GLuint projectionMatrixLocation = glGetUniformLocation(Textureshader, "projection");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection[0][0]);
         TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);



        //100*100*1 mesh

        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 1.0, 0.0, 1.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);
        model = mat4(1.0f);
        model = base*translate(model, vec3(0.0, -0.5, 0.0)) * scale(mat4(1.0f), vec3(100.0, 1.0, 100.0)) ;
        TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        
        //2 poles mesh

        for (int i = 0; i < 2; i++)
        {
            colorLocation = glGetUniformLocation(Textureshader, "object_color");
            glUniform3f(colorLocation, 0.0, 0.0, 0.0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Blacktexture);
            ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
            glUniform1i(ourTextureLocation, 0);
            model = mat4(1.0f);
            model = base*translate(model, vec3(18 - i * 36.0, 5.0, 0.0)) * scale(mat4(1.0f), vec3(1.0, 10.0, 1.0));
            ModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        }

        //x red
        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 1.0, 0.0, 0.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Xtexture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);
        model = mat4(1.0f);
        model = base * translate(mat4(1.0f), vec3(5 * gridUnit / 2, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5 * gridUnit, 1.0f, 1.0f)) ;
        ModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);


        //y gree
        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 0.0, 0.0, 1.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Ytexture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);
        model = mat4(1.0f);
        model = base * translate(mat4(1.0f), vec3(0.0f, 5 * gridUnit / 2, 0.0f)) * scale(mat4(1.0f), vec3(1.0f, 5 * gridUnit, 1.0f)) ;
        ModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        //z blue
        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 0.0, 1.0, 0.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Ztexture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);
        model = mat4(1.0f);
        model = base * translate(mat4(1.0f), vec3(0.0f, 0.0f, (5 * gridUnit) / 2)) * scale(mat4(1.0f), vec3(1.0f, 1.0f, 5 * gridUnit)) ;
        ModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);


        //net mesh
        //vertical
        for (int j = 0; j <= 10; j++)
        {
            colorLocation = glGetUniformLocation(Textureshader, "object_color");
            glUniform3f(colorLocation, 1.0, 1.0, 1.0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Whitetexture);
            ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
            glUniform1i(ourTextureLocation, 0);
            model = mat4(1.0f);
            model = base * translate(model, vec3(0.0, 0.0 + j, 0.0)) * scale(mat4(1.0f), vec3(36.0, 0.2, 0.2));
            ModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        }
        //Horizontal
        for (int j = 0; j <= 36; j++)
        {
            model = mat4(1.0f);
            model = base * translate(model, vec3(-18 + j, 5.0, 0.0)) * scale(mat4(1.0f), vec3(0.2, 10, 0.2));
            ModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        }


        //lower arm   45 degree
        //coordinate base
        //groupMatrix = base * Groupbase * translate(mat4(1.0f), vec3(armlength * cos(radians(angle1)) * 0.5, armlength * cos(radians(angle1)) * 0.5, 0.0f));

        //glm::mat4 lowerJoint, upperjoint, arm_pole_Joint;
        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 153 / 255.0f, 132 / 255.0f, 103 / 255.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, armtexture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);
        //RTS
        scalingMatrix= translate(glm::mat4(1.0f), glm::vec3(0.0f, armlength*0.5, 0.0f))*glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, armlength, 1.0f));
        partMatrix = lowerJoint * scalingMatrix;
        model = mat4(1.0f);
        model = partMatrix;
        ModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        


        
        //upper arm

        //upperArmOrigion= groupMatrix*translate(mat4(1.0f), vec3(armlength * cos(radians(angle1)) * 0.5, armlength * cos(radians(angle1)) * 0.5, 0.0f));
        //groupMatrix= upperArmOrigion* translate(mat4(1.0f), vec3(armlength * cos(radians(angle1)) * 0.5, armlength * cos(radians(angle1)) * 0.5, 0.0f));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, armtexture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);
        //RTS
        scalingMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, armlength* 0.5, 0.0f))*glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, armlength, 1.0f));
        partMatrix = upperjoint * scalingMatrix;
        model = mat4(1.0f);
        model = partMatrix;
        ModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
        
        //racket pole
        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 133 / 255.0f, 0.0f, 0.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);

        //RTS
        scalingMatrix = translate(mat4(1.0f), vec3(0.0f, 0.5*armlength, 0.0f)) *glm::scale(glm::mat4(1.0f), glm::vec3(0.6f, armlength, 0.6f));
        partMatrix = arm_pole_Joint * scalingMatrix;
        model = mat4(1.0f);
        model =partMatrix;
        ModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
        
        //WhiteEdge1 30 degree
       
        for (int i = 0; i < 2; i++)
        {
            //WhiteEdge1 30 degree
            colorLocation = glGetUniformLocation(Textureshader, "object_color");
            glUniform3f(colorLocation, 1.0, 1.0, 1.0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Whitetexture);
            ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
            glUniform1i(ourTextureLocation, 0);

            scalingMatrix = translate(glm::mat4(1.0f), glm::vec3(0, WhiteEdge1Length*0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, WhiteEdge1Length, 0.3f));
            TleftWhiteEdge1= arm_pole_Joint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle2), vec3(0.0f, 0.0f, 1.0f-2*i));
            partMatrix = TleftWhiteEdge1 * scalingMatrix;
            model = mat4(1.0f);
            model = partMatrix;
            ModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);



            //RedEdge1 30 degree
            colorLocation = glGetUniformLocation(Textureshader, "object_color");
            glUniform3f(colorLocation, 1.0, 0.0, 0.0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Ztexture);
            ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
            glUniform1i(ourTextureLocation, 0);


            scalingMatrix = translate(glm::mat4(1.0f), glm::vec3(0, RedEdge1Length * 0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, RedEdge1Length, 0.3f));
            TleftRedEdge1 = TleftWhiteEdge1 *translate(glm::mat4(1.0f), glm::vec3(0, WhiteEdge1Length, 0.0f)) * rotate(mat4(1.0f), radians(angle2), vec3(0.0f, 0.0f, -1.0f + 2 * i));
            partMatrix = TleftRedEdge1 * scalingMatrix;
            model = mat4(1.0f);
            model = partMatrix;
            ModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);



            //WhiteEdge2 15 degree
            colorLocation = glGetUniformLocation(Textureshader, "object_color");
            glUniform3f(colorLocation, 1.0, 1.0, 1.0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Whitetexture);
            ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
            glUniform1i(ourTextureLocation, 0);

            scalingMatrix = translate(mat4(1.0f), vec3(0.0f, WhiteEdge2Length*0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, WhiteEdge2Length, 0.3f));
            TleftWhiteEdge2 = 
                TleftRedEdge1  
                *translate(glm::mat4(1.0f), glm::vec3(0, RedEdge1Length, 0.0f))
                * rotate(mat4(1.0f), radians(-angle3), vec3(0.0f, 0.0f, 1.0f - 2 * i));
            partMatrix = TleftWhiteEdge2 * scalingMatrix;
            model = mat4(1.0f);
            model = partMatrix;
            ModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);


            colorLocation = glGetUniformLocation(Textureshader, "object_color");
            glUniform3f(colorLocation, 1.0, 0.0, 0.0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Ztexture);
            ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
            glUniform1i(ourTextureLocation, 0);



            scalingMatrix = translate(mat4(1.0f),vec3(0.0f, RedEdge2Length*0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, RedEdge2Length, 0.3f));
            TleftRedEdge2 = TleftWhiteEdge2
                *translate(glm::mat4(1.0f), glm::vec3(0, WhiteEdge2Length, 0.0f))
                * rotate(mat4(1.0f), radians(-angle4), vec3(0.0f, 0.0f, 1.0f - 2 * i));
            partMatrix = TleftRedEdge2 * scalingMatrix;
            model = mat4(1.0f);
            model = partMatrix;
            ModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);



            
        }


        // top bar
        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 1.0, 1.0, 1.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Whitetexture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);
        scalingMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, TopBardLength * 0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, TopBardLength, 0.3f));
        TtopRedEdge3 = TleftRedEdge2
            * translate(glm::mat4(1.0f), glm::vec3(0, RedEdge2Length, 0.0f))
            * rotate(mat4(1.0f), radians(angle3 + angle4-90), vec3(0.0f, 0.0f, -1.0));
        partMatrix = TtopRedEdge3 * scalingMatrix;
        model = mat4(1.0f);
        model = partMatrix;
        ModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        //bot bar
        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 1.0, 1.0, 1.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Ztexture);
        ourTextureLocation = glGetUniformLocation(Claytexture, "ourTexture");
        glUniform1i(ourTextureLocation, 0);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(WhiteEdge1Length*sin(radians(angle1)), 1.0, 0.3f));
        TtopRedEdge3 = arm_pole_Joint
        *translate(glm::mat4(1.0f), glm::vec3(0, WhiteEdge1Length* sin(radians(angle1) )*0.5+armlength, 0.0f));
        partMatrix = TtopRedEdge3 * scalingMatrix;
        model = mat4(1.0f);
        model = partMatrix;
        ModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);


        //net
        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 1.0, 0.0, 1.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Claytexture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);
        for (int i = 1; i < 16; i++)
        {
            
            scalingMatrix = translate(mat4(1.0f), vec3(0.0, 0.0, 0.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(netLengthX , 0.5f, 0.2f));
            net = TtopRedEdge3 * translate(mat4(1.0f), vec3(0.0, i, 0.0));
            partMatrix = net * scalingMatrix;
            model = mat4(1.0f);
            model = partMatrix;
            ModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
        }

        for (int i = -4; i < 5; i++)
        {

            scalingMatrix = translate(mat4(1.0f), vec3(0.0, (netLengthY-1)*0.5, 0.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, netLengthY-1, 0.2f));
            net = TtopRedEdge3 * translate(mat4(1.0f), vec3(i*1.0,0.5, 0.0));
            partMatrix = net * scalingMatrix;
            model = mat4(1.0f);
            model = partMatrix;
            ModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
        }
 

        //Tennis ball

        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Greentexture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);
        glBindVertexArray(TennisBall.VAO);

        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 0.0, 0.0, 1.0);
        model = mat4(1.0f);
        model = lowerJoint *translate(model, vec3(0.0, 15.0, 0.0)) * scale(mat4(1.0f), vec3(3.0)) ;
        ModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);


        //light source rendering
        glUseProgram(Lightshader);
        glBindVertexArray(LightSource.VAO);
        model = mat4(1.0f);
        model = translate(model, LightPos);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        GLuint ModelLocationLight = glGetUniformLocation(Lightshader, "model");
        glUniformMatrix4fv(ModelLocationLight, 1, GL_FALSE, &model[0][0]);
        GLuint ViewMatrixLocationLight = glGetUniformLocation(Lightshader, "view");
        glUniformMatrix4fv(ViewMatrixLocationLight, 1, GL_FALSE, &view[0][0]);
        GLuint projectionMatrixLocationLight = glGetUniformLocation(Lightshader, "projection");
        glUniformMatrix4fv(projectionMatrixLocationLight, 1, GL_FALSE, &projection[0][0]);
        glDrawElements(GL_TRIANGLES, LightSource.indices.size(), GL_UNSIGNED_INT, 0);


        //ley bind
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // move camera down
        {
            fov += 0.1;
            projection = perspective(radians(fov), 1024.0f / 768.0f,  // aspect ratio
                0.01f, 200.0f);
            GLuint projectionMatrixLocation = glGetUniformLocation(Textureshader, "projection");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection[0][0]);
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // move camera down
        {
            fov -= 0.1;
            projection = perspective(radians(fov), 1024.0f / 768.0f,  // aspect ratio
                0.01f, 200.0f);
            cameraPos = glm::vec3(0.0f, 20.0f, 65.0f);
            cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

            ViewMatrixLocation = glGetUniformLocation(Textureshader, "view");
            glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, &view[0][0]);
            GLuint projectionMatrixLocation = glGetUniformLocation(Textureshader, "projection");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection[0][0]);
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) // move camera down
        {
            controlmode = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) // move camera down
        {

            controlmode = 2;
        }

        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) // move camera down
        {

            controlmode = 3;
        }
        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) // move camera down
        {

            controlmode = 0;
        }

        float cameraspeed = 2.5f;



        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) // move camera down
        {
            base =  rotate(mat4(1.0f), radians(3.0f), vec3(1.0, 0.0, 0.0))*base;
            lowerJoint = base * Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) // move camera down
        {
            base = rotate(mat4(1.0f), radians(-3.0f), vec3(1.0, 0.0, 0.0))*base;
            lowerJoint = base * Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // move camera down
        {
            base =  rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0))*base;
            lowerJoint = base * Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // move camera down
        {
            base =  rotate(mat4(1.0f), radians(-3.0f), vec3(0.0, 1.0, 0.0))*base;
            lowerJoint = base * Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) // move camera down
        {
            base = mat4(1.0);
            Groupbase = mat4(1.0) * scale(mat4(1.0), vec3(0.5));
            lowerJoint = base * Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
            cameraPos = glm::vec3(0.0f, 20.0f, 65.0f);
            cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

            ViewMatrixLocation = glGetUniformLocation(Textureshader, "view");
            glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, &view[0][0]);

            fov = 70;
            projection = perspective(radians(fov), 1024.0f / 768.0f,  // aspect ratio
                0.01f, 200.0f);

            GLuint projectionMatrixLocation = glGetUniformLocation(Textureshader, "projection");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection[0][0]);
        }
        


        if (flag1 == true)//this is the flag when mouse right button mousecallback is triggered
        {
            cameraPos = cameraPos - vec3(dx * 0.01 * sensitivity, 0.0, 0.0);

        }

        if (flag2 == true)//this is the flag when mouse middle button mousecallback is triggered
        {
            cameraPos = cameraPos + vec3(0.0, dy * 0.01 * sensitivity, 0.0);
        }


        if (flag3 == true)//this is the flag when mouse left button mousecallback is triggered
        {
            fov += dx * sensitivity * 0.01;
            projection = perspective(radians(fov), 1024.0f / 768.0f,  // aspect ratio
                0.01f, 200.0f);

            GLuint projectionMatrixLocation = glGetUniformLocation(Textureshader, "projection");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection[0][0]);
        }


        // This was solution for Lab02 - Moving camera exercise
        // We'll change this to be a first or third person camera


        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move model to left
        {
            Groupbase = translate(mat4(1.0f), vec3(-0.2, 0.0, 0.0)) * Groupbase;
            lowerJoint = base * Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)  // move model to the right
        {
            Groupbase = translate(mat4(1.0f), vec3(0.2, 0.0, 0.0)) * Groupbase;
            lowerJoint = base * Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)  // move model down
        {
            Groupbase = translate(mat4(1.0f), vec3(0.0, -0.2, 0.0)) * Groupbase;
            lowerJoint = base * Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)  // move model up
        {
            Groupbase = translate(mat4(1.0f), vec3(0.0, 0.2, 0.0)) * Groupbase;
            lowerJoint = base * Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) // move model along +Z axis
        {
            switch (controlmode)
            {
            case 1:
                lowerJoint = lowerJoint * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
                upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            case 2:
                upperjoint = upperjoint * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            case 3:
                arm_pole_Joint = arm_pole_Joint * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
                break;
            default:
                Groupbase = Groupbase * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
                lowerJoint = lowerJoint * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
                upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            }

            /*
            
            */
        }

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) // move model along -Z axis
        {
            Groupbase = translate(mat4(1.0f), vec3(0.0, 0.0, -0.2)) * Groupbase;
            lowerJoint = base * Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // Counter Clockwise rotation around world coordianate Y-axis;
        {
            //glm::mat4 lowerJoint, upperjoint, arm_pole_Joint;
            switch(controlmode)
            {
            case 1:
                lowerJoint = lowerJoint* rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
                upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            case 2:
                upperjoint = upperjoint*rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0)) ;
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            case 3:
                arm_pole_Joint = arm_pole_Joint*rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0)) ;
                break;
            default:
                Groupbase = Groupbase*rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
                lowerJoint = lowerJoint * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
                upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            }       

        }


       

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // Clockwise rotation around world coordianate Y-axis;
        {
            //glm::mat4 lowerJoint, upperjoint, arm_pole_Joint;
            switch (controlmode)
            {
            case 1:
                lowerJoint = lowerJoint * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, -1.0, 0.0));
                upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            case 2:
                upperjoint = upperjoint * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, -1.0, 0.0));
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            case 3:
                arm_pole_Joint = arm_pole_Joint * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, -1.0, 0.0));
                break;
            default:
                Groupbase = Groupbase * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, -1.0, 0.0));
                lowerJoint = lowerJoint * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
                upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) // Counter Clockwise rotation around Model Y-axis;
        {
            //glm::mat4 lowerJoint, upperjoint, arm_pole_Joint;
            switch (controlmode)
            {
            case 1:
                lowerJoint = lowerJoint * rotate(mat4(1.0f), radians(3.0f), vec3(1.0, 0.0, 0.0));
                upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            case 2:
                upperjoint = upperjoint * rotate(mat4(1.0f), radians(3.0f), vec3(1.0, 0.0, 0.0));
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            case 3:
                arm_pole_Joint = arm_pole_Joint * rotate(mat4(1.0f), radians(3.0f), vec3(1.0, 0.0, 0.0));
                break;
            default:
                Groupbase = Groupbase * rotate(mat4(1.0f), radians(3.0f), vec3(1.0, 0.0, 0.0));
                lowerJoint = lowerJoint * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
                upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            }
        }


        

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) // Clockwise rotation around Model Y-axis;
        {
            //glm::mat4 lowerJoint, upperjoint, arm_pole_Joint;
            switch (controlmode)
            {
            case 1:
                lowerJoint = lowerJoint * rotate(mat4(1.0f), radians(3.0f), vec3(-1.0, 0.0, 0.0));
                upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            case 2:
                upperjoint = upperjoint * rotate(mat4(1.0f), radians(3.0f), vec3(-1.0, 0.0, 0.0));
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            case 3:
                arm_pole_Joint = arm_pole_Joint * rotate(mat4(1.0f), radians(3.0f), vec3(-1.0, 0.0, 0.0));
                break;
            default:
                Groupbase = Groupbase * rotate(mat4(1.0f), radians(3.0f), vec3(-1.0, 0.0, 0.0));
                lowerJoint = lowerJoint * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
                upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
                arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
                break;
            }
        }


        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) //size up
        {
            Groupbase = Groupbase * scale(mat4(1.0f), vec3(1.01, 1.01, 1.01));
            lowerJoint = base * Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) //size down
        {
            Groupbase = Groupbase * scale(mat4(1.0f), vec3(0.95, 0.95, 0.95));
            lowerJoint = base * Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }


        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) // Pan to the right
        {
            cameraPos = cameraPos + vec3(1, 0.0, 0.0);
        }

        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) // Pan to the left
        {
            cameraPos = cameraPos - vec3(1, 0.0, 0.0);
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) // Tilt up
        {
            cameraPos = cameraPos + vec3(0.0, 1.0, 0.0);
        }

        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) // Tilt down
        {
            cameraPos = cameraPos - vec3(0.0, 1.0, 0.0);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            int xCoordinate = distX(gen);
            int yCoordinate = distY(gen);
            Groupbase = mat4(1.0) * scale(mat4(1.0), vec3(0.5)) * translate(mat4(1.0), vec3(xCoordinate, 0.0, yCoordinate));
            lowerJoint = base * Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }

      
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            renderingMode = GL_POINT; // Switch to points rendering
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            renderingMode = GL_LINE; // Switch to lines rendering
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            renderingMode = GL_FILL; // Switch to filled triangles rendering
        }

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);


        ViewMatrixLocation = glGetUniformLocation(Textureshader, "view");
        glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, &view[0][0]);


        

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    // Shutdown GLFW

    glfwTerminate();

    return 0;
}
// The vertex data for the cube (position, color, normal, uv)
// The vertex data for the cube (position, color, normal, uv)
GLuint loadTexture(const char* filename)
{
    // Step1 Create and bind textures
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);


    glBindTexture(GL_TEXTURE_2D, textureId);

    // Step2 Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Step3 Load Textures with dimension data
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // Step4 Upload the texture to the PU
    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
        0, format, GL_UNSIGNED_BYTE, data);

    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}
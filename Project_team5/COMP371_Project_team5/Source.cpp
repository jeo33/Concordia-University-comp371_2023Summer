//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//
// Inspired by the following tutorials:
// - https://learnopengl.com/Getting-started/Hello-Window
// - https://learnopengl.com/Getting-started/Hello-Triangle

#include <iostream>
#include <stdio.h>
#include <irrKLang/irrKlang.h>
#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
#include <random>
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include "shaderloader.h"
#include"stb_image.h"
#include"Cube.h"
#include"Sphere.h"
#include<list>
#include "OBJloader.h"    //For loading .obj files
#include "OBJloaderV2.h"    //For loading .obj files
using namespace glm;
using namespace std;
using namespace irrklang;

GLuint setupModelEBO(string path, int& vertexCount);
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll
glm::vec3 cameraPos = glm::vec3(0.0f, 20.0f, 65.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
mat4 OrigionIndex[2] = {
    translate(mat4(1.0f),vec3(0.0,0.0,18.0)) ,
    translate(mat4(1.0f),vec3(0.0,0.0,-18.0))
};
float deltaFrame, LastFrame;
float spotlightSwtich = 1.0;
float toggle_texture = 1.0;
float transparentColor = 0.0;
float toggle_shadow = 1.0;
float pureTexture = 0.0;
int CameraMode = 0;
float LightOff = 0.0;
GLuint loadTexture(const char* filename);
mat4 dummy = mat4(1.0);
mat4 view = mat4(1.0);
mat4 base;
int score1 = 0;
int score2 = 0;
int score11 = 0;
int score22 = 0;

mat4 Groupbase;
double dx, dy;
float dt;
float sensitivity = 0.01;
bool flag1 = false;
bool flag2 = false;
bool flag3 = false;
bool hitflag = false;
int renderingMode = GL_FILL; // Default: Filled triangles

std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister 19937 generator
int modeOfCamera = 0;
// Define the range for the x and y coordinates (inclusive)
int minX = -50;
int maxX = 50;
int minY = -50;
int maxY = 50;
// For spinning model
float spinningAngle = 0.0f;
std::uniform_int_distribution<> distX(minX, maxX);
std::uniform_int_distribution<> distY(minY, maxY);
int curveMode = 0;
class Projectile
{
public:
    Projectile(vec3 position, vec3 intialpos, float time)
    {
        Pointposition = position;
        Pointintialpos = intialpos;
        Pointtime = time;
        valide = true;
    }

    vec3 Pointposition;
    vec3 Pointintialpos;
    float Pointtime;
    bool valide;
};

std::list<Projectile> projectileList;
std::list<Projectile> projectileList_second;


std::list<Projectile> Curve2_projectileList;
std::list<Projectile> Curve2_projectileList_second;


std::list<Projectile> Curve3_projectileList;
std::list<Projectile> Curve3_projectileList_second;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS)
    {

        if (key == GLFW_KEY_Y) {

            toggle_texture = (toggle_texture == 1.0) ? 0.0 : 1.0;

        }

        if (key == GLFW_KEY_O) {

            toggle_shadow = (toggle_shadow == 1.0) ? 0.0 : 1.0;

        }


        if (key == GLFW_KEY_P) {

            spotlightSwtich = (spotlightSwtich == 1.0) ? 0.0 : 1.0;

        }

        if (key == GLFW_KEY_L) {

            LightOff = (LightOff == 1.0) ? 0.0 : 1.0;

        }
        if (key == GLFW_KEY_3) {

            curveMode = 0;

        }
        if (key == GLFW_KEY_4) {

            curveMode = 1;
        }
        if (key == GLFW_KEY_5) {

            curveMode = 2;
        }

        if (key == GLFW_KEY_Q) // move model to left
        {
            

            if (curveMode == 0)
            {
                switch (CameraMode)
                {
                case 0:
                    OrigionIndex[0] = rotate(OrigionIndex[0], radians(45.0f), vec3(-1.0, 0.0, 0.0));
                    dummy = OrigionIndex[0];
                    projectileList.push_back(Projectile(vec3(dummy[3][0] + 5, dummy[3][1] + 23, dummy[3][2]),
                        vec3(dummy[3][0] + 5, dummy[3][1] + 23, dummy[3][2]),
                        glfwGetTime()));
                    break;
                case 1:
                    OrigionIndex[1] = rotate(OrigionIndex[1], radians(45.0f), vec3(1.0, 0.0, 0.0));
                    dummy = OrigionIndex[1];
                    projectileList_second.push_back(Projectile(vec3(dummy[3][0] + 5, dummy[3][1] + 23, dummy[3][2]),
                        vec3(dummy[3][0] + 5, dummy[3][1] + 23, dummy[3][2]),
                        glfwGetTime()));
                    break;
                }
            }
            else if (curveMode == 1)
            {
                switch (CameraMode)
                {
                case 0:
                    OrigionIndex[0] = rotate(OrigionIndex[0], radians(45.0f), vec3(-1.0, 0.0, 0.0));
                    dummy = OrigionIndex[0];
                    Curve2_projectileList.push_back(Projectile(vec3(dummy[3][0] + 5, dummy[3][1] + 23, dummy[3][2]),
                        vec3(dummy[3][0] + 5, dummy[3][1] + 23, dummy[3][2]),
                        glfwGetTime()));
                    break;
                case 1:
                    OrigionIndex[1] = rotate(OrigionIndex[1], radians(45.0f), vec3(1.0, 0.0, 0.0));
                    dummy = OrigionIndex[1];
                    Curve2_projectileList_second.push_back(Projectile(vec3(dummy[3][0] + 5, dummy[3][1] + 23, dummy[3][2]),
                        vec3(dummy[3][0] + 5, dummy[3][1] + 23, dummy[3][2]),
                        glfwGetTime()));
                    break;
                }
            }
            else if (curveMode == 2)
            {
                switch (CameraMode)
                {
                case 0:
                    OrigionIndex[0] = rotate(OrigionIndex[0], radians(45.0f), vec3(-1.0, 0.0, 0.0));
                    dummy = OrigionIndex[0];
                    Curve3_projectileList.push_back(Projectile(vec3(dummy[3][0] + 5, dummy[3][1] + 23, dummy[3][2]),
                        vec3(dummy[3][0] + 5, dummy[3][1] + 23, dummy[3][2]),
                        glfwGetTime()));
                    break;
                case 1:
                    OrigionIndex[1] = rotate(OrigionIndex[1], radians(45.0f), vec3(1.0, 0.0, 0.0));
                    dummy = OrigionIndex[1];
                    Curve3_projectileList_second.push_back(Projectile(vec3(dummy[3][0] + 5, dummy[3][1] + 23, dummy[3][2]),
                        vec3(dummy[3][0] + 5, dummy[3][1] + 23, dummy[3][2]),
                        glfwGetTime()));
                    break;
                }
            }
            


        }
        if (key == GLFW_KEY_E) // move model to left
        {

            switch (CameraMode)
            {
            case 0:
                OrigionIndex[0] = rotate(OrigionIndex[0], radians(45.0f), vec3(+1.0, 0.0, 0.0));
                break;
            case 1:
                OrigionIndex[1] = rotate(OrigionIndex[1], radians(45.0f), vec3(-1.0, 0.0, 0.0));
                break;
            }



        }
        if (key == GLFW_KEY_N)
        {
            score1 += 1;
            if (score1 == 10)
            {
                score1 = 0;
                score11 += 1;
            }
        }
        if (key == GLFW_KEY_M)
        {
            score1 -= 1;
            if (score1 == -1)
            {
                score1 = 9;
                score11 -= 1;
            }
        }
        if (key == GLFW_KEY_9)
        {
            score2 += 1;
            if (score2 == 10)
            {
                score2 = 0;
                score22 += 1;
            }
        }
        if (key == GLFW_KEY_0)
        {
            score2 -= 1;
            if (score2 == -1)
            {
                score2 = 9;
                score22 -= 1;
            }
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

    GLuint skytexture = loadTexture("../Assets/Textures/sky.jpg");
    GLuint courttexture = loadTexture("../Assets/Textures/court.jpg");

    std::string shaderPathPrefix = "../Assets/Shaders/";
    std::string TexturePathPrefix = "../Assets/Textures/";

    GLuint Textureshader = loadSHADER(shaderPathPrefix + "TexturedVertexShader.glsl",
        shaderPathPrefix + "TexturedFragmentShader.glsl");
    GLuint Lightshader = loadSHADER(shaderPathPrefix + "lightshadervertex.glsl",
        shaderPathPrefix + "lightshaderFragment.glsl");
    GLuint simpleshader = loadSHADER(shaderPathPrefix + "simpleshadervertex.glsl",
        shaderPathPrefix + "simpleshaderFragment.glsl");

#if defined(PLATFORM_OSX)
    string heraclesPath = "Models/heracles.obj";
    string cubePath = "Models/cube.obj";
#else
    string heraclesPath = "../Assets/Models/heracles.obj";
    string cubePath = "../Assets/Models/cube.obj";
#endif

    int heraclesVertices;
    GLuint heraclesVAO = setupModelEBO(heraclesPath, heraclesVertices);

    int activeVertices = heraclesVertices;
    GLuint activeVAO = heraclesVAO;


    Sphere LightSource(1, 10.0, 10.0);
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
    float cameraSpeed = 0.4f;
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
    mat4 AlphabetJoint = mat4(1.0);
    mat4 boardBase = mat4(1.0f);// *translate(mat4(1.0f), vec3(0.0f, 0.0f, -25.0f))* rotate(mat4(1.0f), radians(90.0f), vec3(0.0, 1.0, 0.0));

    mat4 Groupbase = translate(base, vec3(0.0, 0.0, 0.0)) * scale(mat4(1.0), vec3(0.2));
    glm::mat4 lowerJoint = Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f))
        * scale(mat4(1.0), vec3(0.5));

    mat4 upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
    mat4 lowerArmOrigion = mat4(1.0);
    mat4 upperArmOrigion = mat4(1.0);
    vec3 LightPos;
    mat4 projection = mat4(1.0f);
    mat4 model = mat4(1.0);
    vec3 lightC = vec3(1.0f, 1.0f, 1.0f);
    float strength = 0.1;
    float v = -1.0f;


    mat4 controlMatrix = mat4(1.0f);


    LightPos = vec3(0.6, 30, 5.0);


    ISoundEngine* engine = createIrrKlangDevice();

    if (!engine)
    {
        printf("Could not startup engine\n");
        return 0; // error starting up the engine
    }

    // To play a sound, we only to call play2D(). The second parameter
    // tells the engine to play it looped.

    // play some sound stream, looped
    engine->play2D("../Assets/music/bgm.mp3", true);

    while (!glfwWindowShouldClose(window))
    {
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        LightPos = vec3(30 * sin((glfwGetTime())), 30, 30 * cos((glfwGetTime())));

        dt = glfwGetTime() - lastFrameTime;
        dx = mousePosX - lastMousePosX;
        dy = mousePosY - lastMousePosY;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);

        projection = perspective(radians(fov), 1024.0f / 768.0f,  // aspect ratio
            0.01f, 300.0f);
        float currentFrame = glfwGetTime();
        deltaFrame = currentFrame - LastFrame;
        LastFrame = currentFrame;

        glPolygonMode(GL_FRONT_AND_BACK, renderingMode);
        //LightPos=
        //ConfigureShaderAndMatrices();
        float near_plane = 1.0f, far_plane = 300.0f;
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
        //Render  shadow (simpleDepthShader);*****************************************************************





        glBindVertexArray(cube.VAO);
        model = mat4(1.0f);
        model = base * translate(model, vec3(0.0, -0.4, 0.0)) * scale(mat4(1.0f), vec3(36.0, 1.0, 78.0));
        GLint TextureshaderModelLocation = glGetUniformLocation(simpleshader, "model");
        glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);



        //100*100*1 mesh


        model = mat4(1.0f);
        model = base * translate(model, vec3(0.0, -0.5, 0.0)) * scale(mat4(1.0f), vec3(120.0, 1.0, 120.0));
        TextureshaderModelLocation = glGetUniformLocation(simpleshader, "model");
        glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);


        //2 poles mesh

        for (int i = 0; i < 3; i++)
        {

            model = mat4(1.0f);
            model = base * translate(model, vec3(18 - i * 18.0, 5.0, 0.0)) * scale(mat4(1.0f), vec3(1.0, 10.0, 1.0));
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
        for (int i = 0; i < 2; i++)
        {
            model = mat4(1.0f);
            model = base * translate(model, vec3(-40 + 80 * i, 5, 0.0)) * scale(mat4(1.0f), vec3(30.0, 5, 80.0));
            ModelLocation = glGetUniformLocation(simpleshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
            model = mat4(1.0f);
            model = base * translate(model, vec3(-45 + 90 * i, 10, 0.0)) * scale(mat4(1.0f), vec3(20.0, 5, 80.0));
            ModelLocation = glGetUniformLocation(simpleshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
            model = mat4(1.0f);
            model = base * translate(model, vec3(-50 + 100 * i, 15, 0.0)) * scale(mat4(1.0f), vec3(10.0, 5, 80.0));
            ModelLocation = glGetUniformLocation(simpleshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
        }
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                for (int side = 0; side < 2; side++)
                {
                    for (int layer = 0; layer <= 2; layer++)
                    {
                        if (layer == 0)
                        {

                        }
                        if (side == 0)AlphabetJoint = base * translate(mat4(1.0), vec3(35 + 10 * i, 10 + 5.0 * i, (-4 + j) * 10));
                        else AlphabetJoint = base * translate(mat4(1.0), vec3(-35 - 10 * i, 10 + 5.0 * i, (-4 + j) * 10)) * rotate(mat4(1.0), radians(180.0f), vec3(0.0, 1.0, 0.0));


                        mat4 CharacterG = AlphabetJoint * translate(mat4(1.0f), vec3(-8.0, 0.0, 2 - layer));
                        mat4 CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 5.0f, gridUnit * 1.0f, gridUnit * 1.0f));
                        mat4 CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                        GLuint CharacterALocation = glGetUniformLocation(simpleshader, "model");
                        glUniformMatrix4fv(CharacterALocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

                        CharacterGMatrixTogether = CharacterGMatrixTogether * translate(mat4(1.0f), vec3(0.0, 7.0f, 0.0f));
                        CharacterALocation = glGetUniformLocation(simpleshader, "model");
                        glUniformMatrix4fv(CharacterALocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

                        CharacterG = CharacterG * translate(mat4(1.0f), vec3(-3.0, 3.5, 0.0));
                        CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 1.0f, gridUnit * 8.0f, gridUnit * 1.0f));
                        CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                        CharacterALocation = glGetUniformLocation(simpleshader, "model");
                        glUniformMatrix4fv(CharacterALocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

                        CharacterG = CharacterG * translate(mat4(1.0f), vec3(6.0, 2.5, 0.0));
                        CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 1.0f, gridUnit * 3.0f, gridUnit * 1.0f));
                        CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                        CharacterALocation = glGetUniformLocation(simpleshader, "model");
                        glUniformMatrix4fv(CharacterALocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

                        CharacterG = CharacterG * translate(mat4(1.0f), vec3(0.0, -5, 0.0));
                        CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 1.0f, gridUnit * 3.0f, gridUnit * 1.0f));
                        CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                        CharacterALocation = glGetUniformLocation(simpleshader, "model");
                        glUniformMatrix4fv(CharacterALocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

                        CharacterG = CharacterG * translate(mat4(1.0f), vec3(-1.5, 1.5, 0.0));
                        CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 4.0f, gridUnit * 1.0f, gridUnit * 1.0f));
                        CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                        CharacterALocation = glGetUniformLocation(simpleshader, "model");
                        glUniformMatrix4fv(CharacterALocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                    }
                }
            }
        }


        for (int i = 0; i < 2; i++)
        {
            //WhiteEdge1 30 degree

            lowerJoint = Groupbase * OrigionIndex[i] * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
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
                * rotate(mat4(1.0f), radians(angle3 + angle4 - 90), vec3(0.0f, 0.0f, -1.0));
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


            //net

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

        }




        for (list<Projectile>::iterator it = projectileList.begin(); it != projectileList.end(); it++) {
            float currenttime = glfwGetTime();
            if (curveMode == 0)
            {
                it->Pointposition.z = it->Pointintialpos.z - 50 * (currenttime - it->Pointtime);
                it->Pointposition.y = it->Pointintialpos.y + 30 * (currenttime - it->Pointtime) - 0.5 * 9.8 * (currenttime - it->Pointtime) * (currenttime - it->Pointtime);
                it->Pointposition.x = it->Pointintialpos.x;
                vec3 temp = vec3(it->Pointposition.x * 0.2, it->Pointposition.y * 0.2, it->Pointposition.z * 0.2);


                if (temp.y <= 0 && it->valide == true)
                {
                    if (((temp.z > -39.0) && (temp.z < 0.0)) && (temp.x >= -18 && temp.x <= 18))
                    {
                        score1 += 1;
                        if (score1 == 10)
                        {
                            score1 = 0;
                            score11 += 1;
                        }

                    }
                    else
                    {
                        score2 += 1;
                        if (score2 == 10)
                        {
                            score2 = 0;
                            score22 += 1;
                        }
                    }
                    engine->play2D("../Assets/music/cheer.mp3");
                    it->valide = false;
                }
                else
                {
                    model = base * translate(mat4(1.0f), vec3(temp)) * scale(glm::mat4(1.0f), glm::vec3(1.0));
                    ModelLocation = glGetUniformLocation(simpleshader, "model");
                    glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
                    glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

                }
            }

        }


        for (list<Projectile>::iterator it = projectileList_second.begin(); it != projectileList_second.end(); it++) {
            float currenttime = glfwGetTime();
            it->Pointposition.z = it->Pointintialpos.z + 50 * (currenttime - it->Pointtime);
            it->Pointposition.y = it->Pointintialpos.y + 30 * (currenttime - it->Pointtime) - 0.5 * 9.8 * (currenttime - it->Pointtime) * (currenttime - it->Pointtime);
            it->Pointposition.x = it->Pointintialpos.x;
            vec3 temp = vec3(it->Pointposition.x * 0.2, it->Pointposition.y * 0.2, it->Pointposition.z * 0.2);


            if (temp.y <= 0 && it->valide == true)
            {
                if (((temp.z < 39.0) && (temp.z > 0.0)) && (temp.x >= -18 && temp.x <= 18))
                {
                    score2 += 1;
                    if (score2 == 10)
                    {
                        score2 = 0;
                        score22 += 1;
                    }
                }
                else
                {

                    score1 += 1;
                    if (score1 == 10)
                    {
                        score1 = 0;
                        score11 += 1;
                    }
                }
                engine->play2D("../Assets/music/cheer.mp3");
                it->valide = false;
            }
            else
            {
                model = base * translate(mat4(1.0f), vec3(temp)) * scale(glm::mat4(1.0f), glm::vec3(1.0));
                ModelLocation = glGetUniformLocation(simpleshader, "model");
                glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

            }

        }



        //*****curve2
        for (list<Projectile>::iterator it = Curve2_projectileList.begin(); it != Curve2_projectileList.end(); it++) {
            float currenttime = glfwGetTime();

            it->Pointposition.z = it->Pointintialpos.z - 50 * (currenttime - it->Pointtime);
            it->Pointposition.y = it->Pointintialpos.y + 30 * (currenttime - it->Pointtime) - 0.5 * 9.8 * (currenttime - it->Pointtime) * (currenttime - it->Pointtime);
            it->Pointposition.x = it->Pointintialpos.x - 12 * (currenttime - it->Pointtime);
            vec3 temp = vec3(it->Pointposition.x * 0.2, it->Pointposition.y * 0.2, it->Pointposition.z * 0.2);


            if (temp.y <= 0 && it->valide == true)
            {
                if (((temp.z > -39.0) && (temp.z < 0.0)) && (temp.x >= -18 && temp.x <= 18))
                {
                    score1 += 1;
                    if (score1 == 10)
                    {
                        score1 = 0;
                        score11 += 1;
                    }

                }
                else
                {
                    score2 += 1;
                    if (score2 == 10)
                    {
                        score2 = 0;
                        score22 += 1;
                    }
                }
                engine->play2D("../Assets/music/cheer.mp3");
                it->valide = false;
            }
            else
            {
                model = base * translate(mat4(1.0f), vec3(temp)) * scale(glm::mat4(1.0f), glm::vec3(1.0));
                ModelLocation = glGetUniformLocation(simpleshader, "model");
                glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

            }

        }


        for (list<Projectile>::iterator it = Curve2_projectileList_second.begin(); it != Curve2_projectileList_second.end(); it++) {
            float currenttime = glfwGetTime();
            it->Pointposition.z = it->Pointintialpos.z + 50 * (currenttime - it->Pointtime);
            it->Pointposition.y = it->Pointintialpos.y + 30 * (currenttime - it->Pointtime) - 0.5 * 9.8 * (currenttime - it->Pointtime) * (currenttime - it->Pointtime);
            it->Pointposition.x = it->Pointintialpos.x - 12 * (currenttime - it->Pointtime);
            vec3 temp = vec3(it->Pointposition.x * 0.2, it->Pointposition.y * 0.2, it->Pointposition.z * 0.2);


            if (temp.y <= 0 && it->valide == true)
            {
                if (((temp.z < 39.0) && (temp.z > 0.0)) && (temp.x >= -18 && temp.x <= 18))
                {
                    score2 += 1;
                    if (score2 == 10)
                    {
                        score2 = 0;
                        score22 += 1;
                    }
                }
                else
                {

                    score1 += 1;
                    if (score1 == 10)
                    {
                        score1 = 0;
                        score11 += 1;
                    }
                }
                engine->play2D("../Assets/music/cheer.mp3");
                it->valide = false;
            }
            else
            {
                model = base * translate(mat4(1.0f), vec3(temp)) * scale(glm::mat4(1.0f), glm::vec3(1.0));
                ModelLocation = glGetUniformLocation(simpleshader, "model");
                glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

            }

        }
        //end of curve 2
        //curve 3

        for (list<Projectile>::iterator it = Curve3_projectileList.begin(); it != Curve3_projectileList.end(); it++) {
            float currenttime = glfwGetTime();

            it->Pointposition.z = it->Pointintialpos.z - 50 * (currenttime - it->Pointtime);
            it->Pointposition.y = it->Pointintialpos.y + 30 * (currenttime - it->Pointtime) - 0.5 * 9.8 * (currenttime - it->Pointtime) * (currenttime - it->Pointtime);
            it->Pointposition.x = it->Pointintialpos.x + 12 * (currenttime - it->Pointtime);
            vec3 temp = vec3(it->Pointposition.x * 0.2, it->Pointposition.y * 0.2, it->Pointposition.z * 0.2);


            if (temp.y <= 0 && it->valide == true)
            {
                if (((temp.z > -39.0) && (temp.z < 0.0)) && (temp.x >= -18 && temp.x <= 18))
                {
                    score1 += 1;
                    if (score1 == 10)
                    {
                        score1 = 0;
                        score11 += 1;
                    }

                }
                else
                {
                    score2 += 1;
                    if (score2 == 10)
                    {
                        score2 = 0;
                        score22 += 1;
                    }
                }
                engine->play2D("../Assets/music/cheer.mp3");
                it->valide = false;
            }
            else
            {
                model = base * translate(mat4(1.0f), vec3(temp)) * scale(glm::mat4(1.0f), glm::vec3(1.0));
                ModelLocation = glGetUniformLocation(simpleshader, "model");
                glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

            }

        }


        for (list<Projectile>::iterator it = Curve3_projectileList_second.begin(); it != Curve3_projectileList_second.end(); it++) {
            float currenttime = glfwGetTime();
            it->Pointposition.z = it->Pointintialpos.z + 50 * (currenttime - it->Pointtime);
            it->Pointposition.y = it->Pointintialpos.y + 30 * (currenttime - it->Pointtime) - 0.5 * 9.8 * (currenttime - it->Pointtime) * (currenttime - it->Pointtime);
            it->Pointposition.x = it->Pointintialpos.x + 12 * (currenttime - it->Pointtime);
            vec3 temp = vec3(it->Pointposition.x * 0.2, it->Pointposition.y * 0.2, it->Pointposition.z * 0.2);


            if (temp.y <= 0 && it->valide == true)
            {
                if (((temp.z < 39.0) && (temp.z > 0.0)) && (temp.x >= -18 && temp.x <= 18))
                {
                    score2 += 1;
                    if (score2 == 10)
                    {
                        score2 = 0;
                        score22 += 1;
                    }
                }
                else
                {

                    score1 += 1;
                    if (score1 == 10)
                    {
                        score1 = 0;
                        score11 += 1;
                    }
                }
                engine->play2D("../Assets/music/cheer.mp3");
                it->valide = false;
            }
            else
            {
                model = base * translate(mat4(1.0f), vec3(temp)) * scale(glm::mat4(1.0f), glm::vec3(1.0));
                ModelLocation = glGetUniformLocation(simpleshader, "model");
                glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

            }

        }




        glBindVertexArray(0);



        //ball shadow
        glBindVertexArray(TennisBall.VAO);
        model = mat4(1.0f);
        model = lowerJoint * translate(model, vec3(0.0, 15.0, 0.0)) * scale(mat4(1.0f), vec3(3.0));
        ModelLocation = glGetUniformLocation(simpleshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);






        //rendering the scene*********************************************************************
        glViewport(0, 0, 1024.0f, 768.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glUseProgram(Textureshader);
        //toggle_texture
        glUniform1f(glGetUniformLocation(Textureshader, "is_tex"), toggle_texture);
        glUniform1f(glGetUniformLocation(Textureshader, "LightOff"), LightOff);
        glUniform1f(glGetUniformLocation(Textureshader, "spotlight"), spotlightSwtich);
        glUniform1f(glGetUniformLocation(Textureshader, "pureTexture"), pureTexture);
        glUniform1f(glGetUniformLocation(Textureshader, "transparent"), transparentColor);
        glUniform1f(glGetUniformLocation(Textureshader, "is_shadow"), toggle_shadow);
        GLint colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 1.0, 174 / 255.0, 201 / 255.0);


        //Ground mesh 78*36


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Greentexture);
        GLint ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        GLint shadowMapLocation = glGetUniformLocation(Textureshader, "shadowMap");
        glUniform1i(shadowMapLocation, 1);


        //***************************************************************************************
        //scoreboard

        glBindVertexArray(0);



        glBindVertexArray(cube.VAO);


        for (int i = 0; i < 2; i++)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            model = mat4(1.0f);
            model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(0.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(70.0, 30.0, 1.0));
            TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

            //:
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Ztexture);
            model = mat4(1.0f);
            model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(0.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 3.0, 3.0));
            TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

            model = mat4(1.0f);
            model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(0.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 3.0, 3.0));
            TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

            //player1
            switch (score1)
            {
            case 0:
                //0
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-15.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-5.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 1:
                //1
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-5.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 2:
                //2
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-15.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-5.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 3:
                //3
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-5.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-5.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 4:
                //4
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-15.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-5.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-5.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 5:
                //5
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-15.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-5.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 6:
                //6
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-15.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-5.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-15.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 7:
                //7
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-5.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 8:
                //8
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-15.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-5.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 9:
                //9
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-10.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-15.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-5.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            default:
                break;
            }

            switch (score11)
            {
            case 0:
                //0
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-25.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-25.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-30.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-20.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 1:
                //1
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-20.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 2:
                //2
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-25.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-25.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-25.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-20.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(-20.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            default:
                break;
            }

            //player2
            switch (score2)
            {
            case 0:
                //0
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(30.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(20.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 1:
                //1
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(30.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 2:
                //2
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(20.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(30.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 3:
                //3
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(30.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(30.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 4:
                //4
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(20.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(30.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(30.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 5:
                //5
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(20.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(30.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 6:
                //6
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(20.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(30.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(20.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 7:
                //7
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(30.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 8:
                //8
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(20.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(30.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 9:
                //9
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(25.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(20.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(30.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            default:
                break;
            }

            switch (score22)
            {
            case 0:
                //0
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(10.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(10.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(15.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(5.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 1:
                //1
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(15.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 20.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 2:
                //2
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(10.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(10.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(10.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(5.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(15.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            case 3:
                //3
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(10.0, 60.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(10.0, 40.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(10.0, 50.0, -100.0)) * scale(mat4(1.0f), vec3(10.0, 3.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(15.0, 45.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                model = mat4(1.0f);
                model = base * boardBase * rotate(mat4(1.0f), radians(180.0f * i), vec3(0.0f, 1.0f, 0.0f)) * translate(model, vec3(15.0, 55.0, -100.0)) * scale(mat4(1.0f), vec3(3.0, 10.0, 3.0));
                TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                break;
            default:
                break;
            }
        }














        //*************************************************************************************
        glBindTexture(GL_TEXTURE_2D, Greentexture);
        model = mat4(1.0f);
        model = base * translate(model, vec3(0.0, -0.4, 0.0)) * scale(mat4(1.0f), vec3(36.0, 1.0, 78.0));
        TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);


        near_plane = 1.0f, far_plane = 300.0f;
        lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
        lightView = glm::lookAt(LightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix = lightProjection * lightView;

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

        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);


        glCullFace(GL_FRONT); // Cull the front faces
        glFrontFace(GL_CCW);  // Set the front face winding order (assuming counter-clockwise vertex order for front faces)

        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 135 / 255.0, 206 / 255.0, 235 / 255.0);

        //sky cube


        pureTexture = 1.0;
        glUniform1f(glGetUniformLocation(Textureshader, "pureTexture"), pureTexture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skytexture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);

        model = mat4(1.0f);
        model = base * translate(model, vec3(0.0, -0.5, 0.0)) * scale(mat4(1.0f), vec3(200.0, 200.0, 200.0));
        TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        pureTexture = 0.0;
        glUniform1f(glGetUniformLocation(Textureshader, "pureTexture"), pureTexture);

        glDisable(GL_CULL_FACE);//turn of the front cull


        //100*100*1 mesh

        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 1.0, 0.0, 1.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Claytexture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);
        model = mat4(1.0f);
        model = base * translate(model, vec3(0.0, -0.5, 0.0)) * scale(mat4(1.0f), vec3(120.0, 1.0, 120.0));
        TextureshaderModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(TextureshaderModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);


        //2 poles mesh

        for (int i = 0; i < 3; i++)
        {
            colorLocation = glGetUniformLocation(Textureshader, "object_color");
            glUniform3f(colorLocation, 0.0, 0.0, 0.0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Blacktexture);
            ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
            glUniform1i(ourTextureLocation, 0);
            model = mat4(1.0f);
            model = base * translate(model, vec3(18 - i * 18.0, 5.0, 0.0)) * scale(mat4(1.0f), vec3(1.0, 10.0, 1.0));
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
        model = base * translate(mat4(1.0f), vec3(5 * gridUnit / 2, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5 * gridUnit, 1.0f, 1.0f));
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
        model = base * translate(mat4(1.0f), vec3(0.0f, 5 * gridUnit / 2, 0.0f)) * scale(mat4(1.0f), vec3(1.0f, 5 * gridUnit, 1.0f));
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
        model = base * translate(mat4(1.0f), vec3(0.0f, 0.0f, (5 * gridUnit) / 2)) * scale(mat4(1.0f), vec3(1.0f, 1.0f, 5 * gridUnit));
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
            if (j == 36)
            {
                colorLocation = glGetUniformLocation(Textureshader, "object_color");
                glUniform3f(colorLocation, 1.0, 1.0, 1.0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Blacktexture);
                ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
                glUniform1i(ourTextureLocation, 0);
            }
            model = mat4(1.0f);
            model = base * translate(model, vec3(-18 + j, 5.0, 0.0)) * scale(mat4(1.0f), vec3(0.2, 10, 0.2));
            ModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

        }
        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 1.0, 1.0, 1.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);
        for (int i = 0; i < 2; i++)
        {
            model = mat4(1.0f);
            model = base * translate(model, vec3(-40 + 80 * i, 5, 0.0)) * scale(mat4(1.0f), vec3(30.0, 5, 80.0));
            ModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
            model = mat4(1.0f);
            model = base * translate(model, vec3(-45 + 90 * i, 10, 0.0)) * scale(mat4(1.0f), vec3(20.0, 5, 80.0));
            ModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
            model = mat4(1.0f);
            model = base * translate(model, vec3(-50 + 100 * i, 15, 0.0)) * scale(mat4(1.0f), vec3(10.0, 5, 80.0));
            ModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
        }

        toggle_texture = (toggle_texture == 1.0) ? 0.0 : 1.0;

        glUniform1f(glGetUniformLocation(Textureshader, "is_tex"), toggle_texture);
        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 1.0, 1.0, 1.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Greentexture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);




        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                for (int side = 0; side < 2; side++)
                {
                    for (int layer = 0; layer <= 2; layer++)
                    {
                        if (layer == 0)
                        {
                            transparentColor = (transparentColor == 1.0) ? 0.0 : 1.0;

                            glUniform1f(glGetUniformLocation(Textureshader, "transparent"), transparentColor);
                        }
                        if (side == 0)AlphabetJoint = base * translate(mat4(1.0), vec3(35 + 10 * i, 10 + 5.0 * i, (-4 + j) * 10));
                        else AlphabetJoint = base * translate(mat4(1.0), vec3(-35 - 10 * i, 10 + 5.0 * i, (-4 + j) * 10)) * rotate(mat4(1.0), radians(180.0f), vec3(0.0, 1.0, 0.0));
                        colorLocation = glGetUniformLocation(Textureshader, "object_color");
                        glUniform3f(colorLocation, 0.8, 0.95 - 0.30 * layer, 0.0);
                        mat4 CharacterG = AlphabetJoint * translate(mat4(1.0f), vec3(-8.0, 0.0, 2 - layer));
                        mat4 CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 5.0f, gridUnit * 1.0f, gridUnit * 1.0f));
                        mat4 CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                        GLuint CharacterALocation = glGetUniformLocation(Textureshader, "model");
                        glUniformMatrix4fv(CharacterALocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

                        CharacterGMatrixTogether = CharacterGMatrixTogether * translate(mat4(1.0f), vec3(0.0, 7.0f, 0.0f));
                        CharacterALocation = glGetUniformLocation(Textureshader, "model");
                        glUniformMatrix4fv(CharacterALocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

                        CharacterG = CharacterG * translate(mat4(1.0f), vec3(-3.0, 3.5, 0.0));
                        CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 1.0f, gridUnit * 8.0f, gridUnit * 1.0f));
                        CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                        CharacterALocation = glGetUniformLocation(Textureshader, "model");
                        glUniformMatrix4fv(CharacterALocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

                        CharacterG = CharacterG * translate(mat4(1.0f), vec3(6.0, 2.5, 0.0));
                        CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 1.0f, gridUnit * 3.0f, gridUnit * 1.0f));
                        CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                        CharacterALocation = glGetUniformLocation(Textureshader, "model");
                        glUniformMatrix4fv(CharacterALocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

                        CharacterG = CharacterG * translate(mat4(1.0f), vec3(0.0, -5, 0.0));
                        CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 1.0f, gridUnit * 3.0f, gridUnit * 1.0f));
                        CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                        CharacterALocation = glGetUniformLocation(Textureshader, "model");
                        glUniformMatrix4fv(CharacterALocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

                        CharacterG = CharacterG * translate(mat4(1.0f), vec3(-1.5, 1.5, 0.0));
                        CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 4.0f, gridUnit * 1.0f, gridUnit * 1.0f));
                        CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                        CharacterALocation = glGetUniformLocation(Textureshader, "model");
                        glUniformMatrix4fv(CharacterALocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                        glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
                    }
                }
            }
        }

        toggle_texture = (toggle_texture == 1.0) ? 0.0 : 1.0;

        glUniform1f(glGetUniformLocation(Textureshader, "is_tex"), toggle_texture);






        glBindVertexArray(0);

        spinningAngle += 6.0f * dt;  // This is equivalent to 45 degrees per second
        glBindVertexArray(activeVAO);
        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 1.0, 1.0, 1.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                for (int side = 0; side < 2; side++)
                {
                   
                       
                        if (side == 0)AlphabetJoint = base * translate(mat4(1.0), vec3(35 + 10 * i, 10 + 5.0 * i, (-4 + j) * 10));
                        else AlphabetJoint = base * translate(mat4(1.0), vec3(-35 - 10 * i, 10 + 5.0 * i, (-4 + j) * 10)) * rotate(mat4(1.0), radians(180.0f), vec3(0.0, 1.0, 0.0));
                        mat4 CharacterG = AlphabetJoint * translate(mat4(1.0f), vec3(-8.0, 0.0, 2 ));
                        mat4 CharacterGMatrixscale = glm::rotate(mat4(1.0f), radians((spinningAngle) ), vec3(0.0f, 1.0f, 0.0f)) *
                            glm::rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) *
                            glm::scale(mat4(1.0f), vec3(0.4f));
                        mat4 CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                        GLuint CharacterALocation = glGetUniformLocation(Textureshader, "model");
                        glUniformMatrix4fv(CharacterALocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                        glDrawElements(GL_TRIANGLES, heraclesVertices, GL_UNSIGNED_INT, 0);
                }
            }
        }


        glBindVertexArray(0);


        glBindVertexArray(cube.VAO);

        for (int i = 0; i < 2; i++)
        {
            //WhiteEdge1 30 degree

            lowerJoint = Groupbase * OrigionIndex[i] * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
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
            mat4 scalingMatrix = translate(glm::mat4(1.0f), glm::vec3(0.0f, armlength * 0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, armlength, 1.0f));
            mat4 partMatrix = lowerJoint * scalingMatrix;
            model = mat4(1.0f);
            model = partMatrix;
            ModelLocation = glGetUniformLocation(Textureshader, "model");
            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);

            glDrawArrays(GL_TRIANGLES, 0, 36);

            //OJB files



            glDrawElements(GL_TRIANGLES, activeVertices, GL_UNSIGNED_INT, 0);
            //OBJ files



            //upper arm

            //upperArmOrigion= groupMatrix*translate(mat4(1.0f), vec3(armlength * cos(radians(angle1)) * 0.5, armlength * cos(radians(angle1)) * 0.5, 0.0f));
            //groupMatrix= upperArmOrigion* translate(mat4(1.0f), vec3(armlength * cos(radians(angle1)) * 0.5, armlength * cos(radians(angle1)) * 0.5, 0.0f));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, armtexture);
            ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
            glUniform1i(ourTextureLocation, 0);
            //RTS
            scalingMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, armlength * 0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, armlength, 1.0f));
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
            scalingMatrix = translate(mat4(1.0f), vec3(0.0f, 0.5 * armlength, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.6f, armlength, 0.6f));
            partMatrix = arm_pole_Joint * scalingMatrix;
            model = mat4(1.0f);
            model = partMatrix;
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

                scalingMatrix = translate(glm::mat4(1.0f), glm::vec3(0, WhiteEdge1Length * 0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, WhiteEdge1Length, 0.3f));
                TleftWhiteEdge1 = arm_pole_Joint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle2), vec3(0.0f, 0.0f, 1.0f - 2 * i));
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
                TleftRedEdge1 = TleftWhiteEdge1 * translate(glm::mat4(1.0f), glm::vec3(0, WhiteEdge1Length, 0.0f)) * rotate(mat4(1.0f), radians(angle2), vec3(0.0f, 0.0f, -1.0f + 2 * i));
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

                scalingMatrix = translate(mat4(1.0f), vec3(0.0f, WhiteEdge2Length * 0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, WhiteEdge2Length, 0.3f));
                TleftWhiteEdge2 =
                    TleftRedEdge1
                    * translate(glm::mat4(1.0f), glm::vec3(0, RedEdge1Length, 0.0f))
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



                scalingMatrix = translate(mat4(1.0f), vec3(0.0f, RedEdge2Length * 0.5, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, RedEdge2Length, 0.3f));
                TleftRedEdge2 = TleftWhiteEdge2
                    * translate(glm::mat4(1.0f), glm::vec3(0, WhiteEdge2Length, 0.0f))
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
                * rotate(mat4(1.0f), radians(angle3 + angle4 - 90), vec3(0.0f, 0.0f, -1.0));
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
            scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(WhiteEdge1Length * sin(radians(angle1)), 1.0, 0.3f));
            TtopRedEdge3 = arm_pole_Joint
                * translate(glm::mat4(1.0f), glm::vec3(0, WhiteEdge1Length * sin(radians(angle1)) * 0.5 + armlength, 0.0f));
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

                scalingMatrix = translate(mat4(1.0f), vec3(0.0, 0.0, 0.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(netLengthX, 0.5f, 0.2f));
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

                scalingMatrix = translate(mat4(1.0f), vec3(0.0, (netLengthY - 1) * 0.5, 0.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, netLengthY - 1, 0.2f));
                net = TtopRedEdge3 * translate(mat4(1.0f), vec3(i * 1.0, 0.5, 0.0));
                partMatrix = net * scalingMatrix;
                model = mat4(1.0f);
                model = partMatrix;
                ModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, cube.indices.size(), GL_UNSIGNED_INT, 0);
            }

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
        model = lowerJoint * translate(model, vec3(0.0, 15.0, 0.0)) * scale(mat4(1.0f), vec3(3.0));
        ModelLocation = glGetUniformLocation(Textureshader, "model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);




        colorLocation = glGetUniformLocation(Textureshader, "object_color");
        glUniform3f(colorLocation, 1.0, 0.0, 1.0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Ytexture);
        ourTextureLocation = glGetUniformLocation(Textureshader, "ourTexture");
        glUniform1i(ourTextureLocation, 0);

        for (list<Projectile>::iterator it = projectileList.begin(); it != projectileList.end(); it++) {
            float currenttime = glfwGetTime();
            if (curveMode == 0)
            {
                it->Pointposition.z = it->Pointintialpos.z - 50 * (currenttime - it->Pointtime);
            it->Pointposition.y = it->Pointintialpos.y + 30 * (currenttime - it->Pointtime) - 0.5 * 9.8 * (currenttime - it->Pointtime) * (currenttime - it->Pointtime);
            it->Pointposition.x = it->Pointintialpos.x;
            vec3 temp = vec3(it->Pointposition.x * 0.2, it->Pointposition.y * 0.2, it->Pointposition.z * 0.2);


            if (temp.y <= 0 && it->valide == true)
            {
                if (((temp.z > -39.0) && (temp.z < 0.0)) && (temp.x >= -18 && temp.x <= 18))
                {
                    score1 += 1;
                    if (score1 == 10)
                    {
                        score1 = 0;
                        score11 += 1;
                    }

                }
                else
                {
                    score2 += 1;
                    if (score2 == 10)
                    {
                        score2 = 0;
                        score22 += 1;
                    }
                }
                engine->play2D("../Assets/music/cheer.mp3");
                it->valide = false;
            }
            else
            {
                model = base * translate(mat4(1.0f), vec3(temp)) * scale(glm::mat4(1.0f), glm::vec3(1.0));
                ModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

            }
            }

        }


        for (list<Projectile>::iterator it = projectileList_second.begin(); it != projectileList_second.end(); it++) {
            float currenttime = glfwGetTime();
            it->Pointposition.z = it->Pointintialpos.z + 50 * (currenttime - it->Pointtime);
            it->Pointposition.y = it->Pointintialpos.y + 30 * (currenttime - it->Pointtime) - 0.5 * 9.8 * (currenttime - it->Pointtime) * (currenttime - it->Pointtime);
            it->Pointposition.x = it->Pointintialpos.x;
            vec3 temp = vec3(it->Pointposition.x * 0.2, it->Pointposition.y * 0.2, it->Pointposition.z * 0.2);


            if (temp.y <= 0 && it->valide == true)
            {
                if (((temp.z <39.0) && (temp.z > 0.0)) && (temp.x >= -18 && temp.x <= 18))
                {
                    score2 += 1;
                    if (score2 == 10)
                    {
                        score2 = 0;
                        score22 += 1;
                    }
                }
                else
                {

                    score1 += 1;
                    if (score1 == 10)
                    {
                        score1 = 0;
                        score11 += 1;
                    }
                }
                engine->play2D("../Assets/music/cheer.mp3");
                it->valide = false;
            }
            else
            {
                model = base * translate(mat4(1.0f), vec3(temp)) * scale(glm::mat4(1.0f), glm::vec3(1.0));
                ModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

            }

        }



        //*****curve2
        for (list<Projectile>::iterator it = Curve2_projectileList.begin(); it != Curve2_projectileList.end(); it++) {
            float currenttime = glfwGetTime();

                it->Pointposition.z = it->Pointintialpos.z - 50 * (currenttime - it->Pointtime);
                it->Pointposition.y = it->Pointintialpos.y + 30 * (currenttime - it->Pointtime) - 0.5 * 9.8 * (currenttime - it->Pointtime) * (currenttime - it->Pointtime);
                it->Pointposition.x = it->Pointintialpos.x-12* (currenttime - it->Pointtime);
                vec3 temp = vec3(it->Pointposition.x * 0.2, it->Pointposition.y * 0.2, it->Pointposition.z * 0.2);


                if (temp.y <= 0 && it->valide == true)
                {
                    if (((temp.z > -39.0) && (temp.z < 0.0)) && (temp.x >= -18 && temp.x <= 18))
                    {
                        score1 += 1;
                        if (score1 == 10)
                        {
                            score1 = 0;
                            score11 += 1;
                        }

                    }
                    else
                    {
                        score2 += 1;
                        if (score2 == 10)
                        {
                            score2 = 0;
                            score22 += 1;
                        }
                    }
                    engine->play2D("../Assets/music/cheer.mp3");
                    it->valide = false;
                }
                else
                {
                    model = base * translate(mat4(1.0f), vec3(temp)) * scale(glm::mat4(1.0f), glm::vec3(1.0));
                    ModelLocation = glGetUniformLocation(Textureshader, "model");
                    glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
                    glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

                }

        }


        for (list<Projectile>::iterator it = Curve2_projectileList_second.begin(); it != Curve2_projectileList_second.end(); it++) {
            float currenttime = glfwGetTime();
            it->Pointposition.z = it->Pointintialpos.z + 50 * (currenttime - it->Pointtime);
            it->Pointposition.y = it->Pointintialpos.y + 30 * (currenttime - it->Pointtime) - 0.5 * 9.8 * (currenttime - it->Pointtime) * (currenttime - it->Pointtime);
            it->Pointposition.x = it->Pointintialpos.x - 12 * (currenttime - it->Pointtime);
            vec3 temp = vec3(it->Pointposition.x * 0.2, it->Pointposition.y * 0.2, it->Pointposition.z * 0.2);


            if (temp.y <= 0 && it->valide == true)
            {
                if (((temp.z < 39.0) && (temp.z > 0.0)) && (temp.x >= -18 && temp.x <= 18))
                {
                    score2 += 1;
                    if (score2 == 10)
                    {
                        score2 = 0;
                        score22 += 1;
                    }
                }
                else
                {

                    score1 += 1;
                    if (score1 == 10)
                    {
                        score1 = 0;
                        score11 += 1;
                    }
                }
                engine->play2D("../Assets/music/cheer.mp3");
                it->valide = false;
            }
            else
            {
                model = base * translate(mat4(1.0f), vec3(temp)) * scale(glm::mat4(1.0f), glm::vec3(1.0));
                ModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

            }

        }
        //end of curve 2
        //curve 3

        for (list<Projectile>::iterator it = Curve3_projectileList.begin(); it != Curve3_projectileList.end(); it++) {
            float currenttime = glfwGetTime();

            it->Pointposition.z = it->Pointintialpos.z - 50 * (currenttime - it->Pointtime);
            it->Pointposition.y = it->Pointintialpos.y + 30 * (currenttime - it->Pointtime) - 0.5 * 9.8 * (currenttime - it->Pointtime) * (currenttime - it->Pointtime);
            it->Pointposition.x = it->Pointintialpos.x + 12 * (currenttime - it->Pointtime);
            vec3 temp = vec3(it->Pointposition.x * 0.2, it->Pointposition.y * 0.2, it->Pointposition.z * 0.2);


            if (temp.y <= 0 && it->valide == true)
            {
                if (((temp.z > -39.0) && (temp.z < 0.0)) && (temp.x >= -18 && temp.x <= 18))
                {
                    score1 += 1;
                    if (score1 == 10)
                    {
                        score1 = 0;
                        score11 += 1;
                    }

                }
                else
                {
                    score2 += 1;
                    if (score2 == 10)
                    {
                        score2 = 0;
                        score22 += 1;
                    }
                }
                engine->play2D("../Assets/music/cheer.mp3");
                it->valide = false;
            }
            else
            {
                model = base * translate(mat4(1.0f), vec3(temp)) * scale(glm::mat4(1.0f), glm::vec3(1.0));
                ModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

            }

        }


        for (list<Projectile>::iterator it = Curve3_projectileList_second.begin(); it != Curve3_projectileList_second.end(); it++) {
            float currenttime = glfwGetTime();
            it->Pointposition.z = it->Pointintialpos.z + 50 * (currenttime - it->Pointtime);
            it->Pointposition.y = it->Pointintialpos.y + 30 * (currenttime - it->Pointtime) - 0.5 * 9.8 * (currenttime - it->Pointtime) * (currenttime - it->Pointtime);
            it->Pointposition.x = it->Pointintialpos.x + 12 * (currenttime - it->Pointtime);
            vec3 temp = vec3(it->Pointposition.x * 0.2, it->Pointposition.y * 0.2, it->Pointposition.z * 0.2);


            if (temp.y <= 0 && it->valide == true)
            {
                if (((temp.z < 39.0) && (temp.z > 0.0)) && (temp.x >= -18 && temp.x <= 18))
                {
                    score2 += 1;
                    if (score2 == 10)
                    {
                        score2 = 0;
                        score22 += 1;
                    }
                }
                else
                {

                    score1 += 1;
                    if (score1 == 10)
                    {
                        score1 = 0;
                        score11 += 1;
                    }
                }
                engine->play2D("../Assets/music/cheer.mp3");
                it->valide = false;
            }
            else
            {
                model = base * translate(mat4(1.0f), vec3(temp)) * scale(glm::mat4(1.0f), glm::vec3(1.0));
                ModelLocation = glGetUniformLocation(Textureshader, "model");
                glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &model[0][0]);
                glDrawElements(GL_TRIANGLES, TennisBall.indices.size(), GL_UNSIGNED_INT, 0);

            }

        }
        //end of curve 3


        glBindVertexArray(0);



        //racket!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!




        //light source rendering
        glUseProgram(Lightshader);
        glBindVertexArray(LightSource.VAO);
        model = mat4(1.0f);
        model = translate(mat4(1.0f), LightPos);
        mat4 cameraview = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        GLuint ModelLocationLight = glGetUniformLocation(Lightshader, "model");
        glUniformMatrix4fv(ModelLocationLight, 1, GL_FALSE, &model[0][0]);
        GLuint ViewMatrixLocationLight = glGetUniformLocation(Lightshader, "view");
        glUniformMatrix4fv(ViewMatrixLocationLight, 1, GL_FALSE, &cameraview[0][0]);
        GLuint projectionMatrixLocationLight = glGetUniformLocation(Lightshader, "projection");
        glUniformMatrix4fv(projectionMatrixLocationLight, 1, GL_FALSE, &projection[0][0]);
        glDrawElements(GL_TRIANGLES, LightSource.indices.size(), GL_UNSIGNED_INT, 0);







        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // move camera down
        {
            CameraMode = 0;
            cameraPos = glm::vec3(0.0f, 20.0f, 65.0f);
            cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            ViewMatrixLocation = glGetUniformLocation(Textureshader, "view");
            glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, &view[0][0]);

        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // move camera down
        {
            CameraMode = 1;
            cameraPos = glm::vec3(0.0f, 20.0f, -65.0f);
            cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

            ViewMatrixLocation = glGetUniformLocation(Textureshader, "view");
            glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, &view[0][0]);
        }
        
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) // move camera down
        {
            CameraMode = 2;
            base = mat4(1.0);
            Groupbase = translate(base, vec3(0.0, 0.0, 0.0)) * scale(mat4(1.0), vec3(0.5));
            lowerJoint = Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
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
                0.01f, 300.0f);

            GLuint projectionMatrixLocation = glGetUniformLocation(Textureshader, "projection");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection[0][0]);
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) // move camera down
        {
            base = rotate(mat4(1.0f), radians(3.0f), vec3(1.0, 0.0, 0.0)) * base;
            Groupbase = translate(base, vec3(0.0, 0.0, 0.0)) * scale(mat4(1.0), vec3(0.2));
            lowerJoint = Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) // move camera down
        {
            base = rotate(mat4(1.0f), radians(-3.0f), vec3(1.0, 0.0, 0.0)) * base;
            Groupbase = translate(base, vec3(0.0, 0.0, 0.0)) * scale(mat4(1.0), vec3(0.2));
            lowerJoint = Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // move camera down
        {
            //lightView = rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0)) * lightView;
            base = rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0)) * base;
            Groupbase = translate(base, vec3(0.0, 0.0, 0.0)) * scale(mat4(1.0), vec3(0.2));
            lowerJoint = Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // move camera down
        {
            base = rotate(mat4(1.0f), radians(-3.0f), vec3(0.0, 1.0, 0.0)) * base;
            Groupbase = translate(base, vec3(0.0, 0.0, 0.0)) * scale(mat4(1.0), vec3(0.2));
            lowerJoint = Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
            upperjoint = lowerJoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f)) * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, 1.0f));
            arm_pole_Joint = upperjoint * translate(glm::mat4(1.0f), glm::vec3(0, armlength, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) // move camera down
        {
            base = mat4(1.0);
            OrigionIndex[0] = translate(mat4(1.0f), vec3(0.0, 0.0, 18.0));
            OrigionIndex[1] = translate(mat4(1.0f), vec3(0.0, 0.0, -18.0));
            Groupbase = translate(base, vec3(0.0, 0.0, 0.0)) * scale(mat4(1.0), vec3(0.2));
            lowerJoint = Groupbase * rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f));
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
                0.01f, 300.0f);
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
                0.01f, 300.0f);

            GLuint projectionMatrixLocation = glGetUniformLocation(Textureshader, "projection");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection[0][0]);
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move model to left
        {

            switch (CameraMode)
            {
            case 0:

                OrigionIndex[CameraMode] = translate(OrigionIndex[CameraMode], vec3(cameraSpeed * dt, 0.0, 0.0));
                break;
            case 1:

                OrigionIndex[CameraMode] = translate(OrigionIndex[CameraMode], vec3(-cameraSpeed * dt, 0.0, 0.0));
                break;
            }
            //cameraPos = vec3(translate(mat4(1.0), (cameraSpeed * dt / 2) * normalize(cross(cameraFront, cameraUp))) * vec4(cameraPos, 1.0));

        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)  // move model to the right
        {
            switch (CameraMode)
            {
            case 0:

                OrigionIndex[CameraMode] = translate(OrigionIndex[CameraMode], vec3(-cameraSpeed * dt, 0.0, 0.0));
                break;
            case 1:

                OrigionIndex[CameraMode] = translate(OrigionIndex[CameraMode], vec3(cameraSpeed * dt, 0.0, 0.0));
                break;
            }
            //cameraPos = vec3(translate(mat4(1.0), (-cameraSpeed * dt / 2) * normalize(cross(cameraFront, cameraUp))) * vec4(cameraPos, 1.0));
            //*vec3(-cameraSpeed * dt/2, 0.0, 0.0))


        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)  // move model down
        {
            switch (CameraMode)
            {
            case 0:
                OrigionIndex[CameraMode] = translate(OrigionIndex[CameraMode], vec3(0.0, 0.0, 1.0) * cameraSpeed * dt);
                break;
            case 1:
                OrigionIndex[CameraMode] = translate(OrigionIndex[CameraMode], vec3(0.0, 0.0, -1.0) * cameraSpeed * dt);
                break;
            }
            //cameraPos = cameraPos + vec3(0.0, -cameraSpeed * dt / 2, 0.0);


        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)  // move model up
        {
            switch (CameraMode)
            {
            case 0:
                OrigionIndex[CameraMode] = translate(OrigionIndex[CameraMode], vec3(0.0, 0.0, -1.0) * cameraSpeed * dt);
                break;
            case 1:
                OrigionIndex[CameraMode] = translate(OrigionIndex[CameraMode], vec3(0.0, 0.0, 1.0) * cameraSpeed * dt);
                break;
            }
            //cameraPos = cameraPos + vec3(0.0, cameraSpeed * dt / 2, 0.0);

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



        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) // move model to left
        {

            OrigionIndex[CameraMode] = rotate(mat4(1.0), radians(cameraSpeed * dt), vec3(1.0, 0.0, 0.0)) * OrigionIndex[CameraMode];
            if (CameraMode != 2)
            {
                cameraPos = vec3(rotate(mat4(1.0), radians(cameraSpeed * dt), vec3(1.0, 0.0, 0.0)) * vec4(cameraPos, 1.0));
                cameraFront = vec3(rotate(mat4(1.0), radians(cameraSpeed * dt), vec3(1.0, 0.0, 0.0)) * vec4(cameraFront, 1.0));
            }
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)  // move model to the right
        {
            OrigionIndex[CameraMode] = rotate(mat4(1.0), radians(cameraSpeed * dt), vec3(-1.0, 0.0, 0.0)) * OrigionIndex[CameraMode];
            if (CameraMode != 2)
            {
                cameraPos = vec3(rotate(mat4(1.0), radians(cameraSpeed * dt), vec3(-1.0, 0.0, 0.0)) * vec4(cameraPos, 1.0));

                cameraFront = vec3(rotate(mat4(1.0), radians(cameraSpeed * dt), vec3(-1.0, 0.0, 0.0)) * vec4(cameraFront, 1.0));
            }

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
GLuint setupModelEBO(string path, int& vertexCount) {
    vector<int> vertexIndices;
    // The contiguous sets of three indices of vertices, normals and UVs, used to
    // make a triangle
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec2> UVs;

    // read the vertices from the cube.obj file
    // We won't be needing the normals or UVs for this program
    loadOBJ2(path.c_str(), vertexIndices, vertices, normals, UVs);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);  // Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and
    // attribute pointer(s).

    // Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
        &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
        (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
        &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
        (GLvoid*)0);
    glEnableVertexAttribArray(1);

    // UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(),
        GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
        (GLvoid*)0);
    glEnableVertexAttribArray(2);

    // EBO setup
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(int),
        &vertexIndices.front(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent
    // strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
    vertexCount = vertexIndices.size();
    return VAO;
}
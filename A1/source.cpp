/*
The main body of this assignment is based on:

COMP 371 Labs Framework
Created by Nicolas Bergeron on 20/06/2019.

Based on the framework , I finished the quiz 1
Junpeng gai
SID:40009896
Concordia University
*/

#include <iostream>
#include <list>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>


using namespace glm;
using namespace std;
vec3 cameraPosition;
vec3 cameraLookAt;
vec3 cameraUp;
mat4 base;

mat4 Groupbase;
double dx, dy;
float dt;
float sensitivity = 0.01;
bool flag1 = false;
bool flag2 = false;
bool flag3 = false;

int renderingMode = GL_FILL; // Default: Filled triangles

// GLFW callback function for keyboard input
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_P) {
            renderingMode = GL_POINT; // Switch to points rendering
        }
        else if (key == GLFW_KEY_L) {
            renderingMode = GL_LINE; // Switch to lines rendering
        }
        else if (key == GLFW_KEY_T) {
            renderingMode = GL_FILL; // Switch to filled triangles rendering
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


const char* getVertexShaderSource()
{
    // For now, you use a string for your shader code, in the assignment, shaders will be stored in .glsl files
    return
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;"
        "layout (location = 1) in vec3 aColor;"
        ""
        "uniform mat4 worldMatrix;"
        "uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
        "uniform mat4 projectionMatrix = mat4(1.0);"
        ""
        "out vec3 vertexColor;"
        "void main()"
        "{"
        "   vertexColor = aColor;"
        "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
        "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
        "}";
}


const char* getFragmentShaderSource()
{
    return
        "#version 330 core\n"
        "in vec3 vertexColor;"
        "uniform vec3 mycolor;" // Uniform variable for color
        "out vec4 FragColor;"
        "void main()"
        "{"
        "   FragColor = vec4(mycolor, 1.0f);"
        "}";
}


int compileAndLinkShaders()
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = getVertexShaderSource();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = getFragmentShaderSource();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


int createVertexBufferObject()
{
    // Cube model
    vec3 vertexArray[] = {  // position,                            color
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), //left - red
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),

        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),

        vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), // far - blue
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),

        vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),

        vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), // bottom - turquoise
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),

        vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),

        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), // near - green
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), // right - purple
        vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),

        vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), // top - yellow
        vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f)
    };


    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        2 * sizeof(vec3), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
        3,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(vec3),
        (void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);


    return vertexBufferObject;
}


int main(int argc, char* argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Assignment1_Junpeng Gai)40009896", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // @TODO 3 - Disable mouse cursor
    // ...

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Compile and link shaders here ...
    int shaderProgram = compileAndLinkShaders();

    // We can set the shader once, since we have only one
    glUseProgram(shaderProgram);


    // Camera parameters for view transform
    cameraPosition = vec3(0.0f, 40.0f, 90.0f);
    cameraLookAt = vec3(0.0f, 20.0f, -1.0f);
    cameraUp = vec3(0.0f, 1.0f, 0.0f);
    base = mat4(1.0);

    mat4 Groupbase = mat4(1.0)*translate(mat4(1.0),vec3(10.0,0.0,0.0));
    // Other camera parameters
    float cameraSpeed = 1.0f;
    float cameraFastSpeed = 2 * cameraSpeed;
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;
    bool  cameraFirstPerson = true; // press 1 or 2 to toggle this variable


    //bracket parameter if there are any modifications I dont need to change it line by line
    float angle1 = 45.0;
    float angle2 = 50.0;
    float angle3 = 40;
    float angle4 = 60;
    float AxisRotationSpeed = 3.0;
    float armlength = 10;
    float WhiteEdge1Length = 7;
    float RedEdge1Length = 12;
    float WhiteEdge2Length = 2;
    float RedEdge2Length = 2;
    float TopBardLength = 6;
    float netLengthY = RedEdge1Length + WhiteEdge2Length * cos(radians(angle3)) + RedEdge2Length * cos(radians(angle4)) + 1;
    float netLengthX = 2 * WhiteEdge1Length * sin(radians(angle2)) - 1;

    //unit length is 1
    float gridUnit = 1.0f;
    glm::mat4 Tlowerarm, Tupperarm, Tpole, TleftWhiteEdge1, TleftRedEdge1, TleftWhiteEdge2, TleftRedEdge2, TtopRedEdge3, net;

    // Spinning cube at camera position
    float spinningCubeAngle = 0.0f;
    float fov = 70.0;
    // Set projection matrix for shader, this won't change
    mat4 projectionMatrix = glm::perspective(fov,            // field of view in degrees
        1024.0f / 768.0f,  // aspect ratio
        0.01f, 10000.0f);   // near and far (near > 0)

    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
        cameraLookAt,  // center
        cameraUp); // up

    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);



    // Define and upload geometry to the GPU here ...
    int vao = createVertexBufferObject();

    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, keyCallback);
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);

    // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        // Frame time calculation
        dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;
        GLint colorLocation = glGetUniformLocation(shaderProgram, "mycolor");
        glPolygonMode(GL_FRONT_AND_BACK, renderingMode);
        // ...
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw geometry
        glBindVertexArray(vao);


        //base is the identity matrix for all parts 
        
        //x red bar
        glUniform3f(colorLocation, 1.0, 0.0, 0.0);
        mat4 groundWorldMatrix = base * translate(mat4(1.0f), vec3(5 * gridUnit / 2, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5 * gridUnit, 1.0f, 1.0f));
        GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        //y gree bar
        glUniform3f(colorLocation, 0.0, 1.0, 0.0);
        groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f, 5 * gridUnit / 2, 0.0f)) * scale(mat4(1.0f), vec3(1.0f, 5 * gridUnit, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //z blue bar
        glUniform3f(colorLocation, 0.0, 0.0, 1.0);
        groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f, 0.0f, (5 * gridUnit) / 2)) * scale(mat4(1.0f), vec3(1.0f, 1.0f, 5 * gridUnit));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);



        for (int i = 0; i <= 50; ++i)
        {

            glUniform3f(colorLocation, 1.0, 1.0, 0.0);
            groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f + gridUnit * i)) * scale(mat4(1.0f), vec3(gridUnit * 100.0f, 0.1f, 0.1f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f - gridUnit * i)) * scale(mat4(1.0f), vec3(gridUnit * 100.0f, 0.1f, 0.1f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }


        for (int i = 0; i <= 50; ++i)
        {

            glUniform3f(colorLocation, 1.0, 1.0, 0.0);
            groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f - gridUnit * i, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.1f, 0.1f, gridUnit * 100.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f + gridUnit * i, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.1f, 0.1f, gridUnit * 100.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }



        //Groupbase is the identity matrix for all model parts 
        //glm::mat4 Tlowerarm, Tupperarm, Tpole, TleftWhiteEdge1, TleftRedEdge1, TleftWhiteEdge2, TleftRedEdge2, TtopRedEdge3;
        //lower arm   slope 45 degree
        glUniform3f(colorLocation, 153 / 255.0f, 132 / 255.0f, 103 / 255.0f);
        glm::mat4 partMatrix = rotate(mat4(1.0f), radians(angle1), vec3(0.0f, 0.0f, -1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, armlength, 1.0f));
        glm::mat4 groupMatrix = base * Groupbase * translate(mat4(1.0f), vec3(armlength * cos(radians(angle1)) * 0.5, armlength * cos(radians(angle1)) * 0.5, 0.0f));
        glm::mat4 lowerarmMatrix = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &lowerarmMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        //upper arm
        glUniform3f(colorLocation, 153 / 255.0f, 132 / 255.0f, 103 / 255.0f);
        glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, armlength, 1.0f));
        Tupperarm = glm::translate(glm::mat4(1.0f), glm::vec3(armlength * cos(radians(angle1)) * 0.5, armlength * cos(radians(angle1)) * 0.5 + armlength * 0.5f, 0.0f));
        partMatrix = Tupperarm * scalingMatrix;
        glm::mat4 upperarmMatrix = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &upperarmMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        //racket pole
        glUniform3f(colorLocation, 133 / 255.0f, 0.0f, 0.0f);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.6f, armlength, 0.6f));
        Tpole = Tupperarm * translate(mat4(1.0f), vec3(0.0f, armlength, 0.0f));
        partMatrix = Tpole * scalingMatrix;
        glm::mat4 pole = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pole[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //WhiteEdge1 30 degree
        glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);
        scalingMatrix = rotate(mat4(1.0f), radians(angle2), vec3(0.0f, 0.0f, -1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, WhiteEdge1Length, 0.3f));
        TleftWhiteEdge1 = Tpole * translate(mat4(1.0f), vec3(WhiteEdge1Length * sin(radians(angle2)) * 0.5, WhiteEdge1Length, 0.0f));
        partMatrix = TleftWhiteEdge1 * scalingMatrix;
        glm::mat4 whiteEdge = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &whiteEdge[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //right white edge
        glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);
        scalingMatrix = rotate(mat4(1.0f), radians(-angle2), vec3(0.0f, 0.0f, -1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, WhiteEdge1Length, 0.3f));
        TleftWhiteEdge1 = Tpole * translate(mat4(1.0f), vec3(-WhiteEdge1Length * sin(radians(angle2)) * 0.5, WhiteEdge1Length, 0.0f));
        partMatrix = TleftWhiteEdge1 * scalingMatrix;
        whiteEdge = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &whiteEdge[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //RedEdge1 30 degree
        glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, RedEdge1Length, 0.3f));
        TleftRedEdge1 = TleftWhiteEdge1 * translate(mat4(1.0f), vec3(-WhiteEdge1Length * sin(radians(angle2)) * 0.5, RedEdge1Length * 0.5 + WhiteEdge1Length * cos(radians(angle2)) * 0.5, 0.0f));
        partMatrix = TleftRedEdge1 * scalingMatrix;
        glm::mat4 RedEdge = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &RedEdge[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //right RedEdge1

        glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f);
        scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, RedEdge1Length, 0.3f));
        TleftRedEdge1 = TleftWhiteEdge1 * translate(mat4(1.0f), vec3(-WhiteEdge1Length * sin(radians(angle2)) * 0.5 + 2 * WhiteEdge1Length
            * sin(radians(angle2)), RedEdge1Length * 0.5 + WhiteEdge1Length * cos(radians(angle2)) * 0.5, 0.0f));
        partMatrix = TleftRedEdge1 * scalingMatrix;
        RedEdge = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &RedEdge[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);



        //WhiteEdge2 15 degree
        glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);
        scalingMatrix = rotate(mat4(1.0f), radians(angle3), vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, WhiteEdge2Length, 0.3f));
        TleftWhiteEdge2 = TleftRedEdge1 * translate(mat4(1.0f), vec3(-WhiteEdge2Length * sin(radians(angle3)) * 0.5, RedEdge1Length * 0.5 + WhiteEdge2Length * cos(radians(angle3)) * 0.5, 0.0f));
        partMatrix = TleftWhiteEdge2 * scalingMatrix;
        glm::mat4 WhiteEdge2 = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &WhiteEdge2[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //right WhiteEdge2


        glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);
        scalingMatrix = rotate(mat4(1.0f), radians(angle3), vec3(0.0f, 0.0f, -1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, WhiteEdge2Length, 0.3f));
        TleftWhiteEdge2 = TleftRedEdge1 * translate(mat4(1.0f), vec3(WhiteEdge2Length * sin(radians(angle3)) * 0.5 - 2 * WhiteEdge1Length
            * sin(radians(angle2)), RedEdge1Length * 0.5 + WhiteEdge2Length * cos(radians(angle3)) * 0.5, 0.0f));
        partMatrix = TleftWhiteEdge2 * scalingMatrix;
        WhiteEdge2 = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &WhiteEdge2[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);




        //RedEdge2 5 degree


        glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f);
        scalingMatrix = rotate(mat4(1.0f), radians(angle4), vec3(0.0f, 0.0f, -1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, RedEdge2Length, 0.3f));
        TleftRedEdge2 = translate(mat4(1.0f), vec3(0.5 * WhiteEdge2Length * sin(radians(angle3)), 0.5 * WhiteEdge2Length * cos(radians(angle3)), 0.0)) * TleftWhiteEdge2 * translate(mat4(1.0f), vec3(0.5 * RedEdge2Length * sin(radians(angle4)), 0.5 * RedEdge2Length * cos(radians(angle4)), 0.0));
        partMatrix = TleftRedEdge2 * scalingMatrix;
        glm::mat4 RedEdge2 = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &RedEdge2[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        //right RedEdge2


        glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f);
        scalingMatrix = rotate(mat4(1.0f), radians(angle4), vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, RedEdge2Length, 0.3f));
        TleftRedEdge2 = translate(mat4(1.0f), vec3(0.5 * WhiteEdge2Length * sin(radians(angle3)), 0.5 * WhiteEdge2Length * cos(radians(angle3)), 0.0)) * TleftWhiteEdge2 * translate(mat4(1.0f), vec3(-0.5 * RedEdge2Length * sin(radians(angle4)) + 2 * (WhiteEdge1Length * sin(radians(angle2)) - WhiteEdge2Length * sin(radians(angle3))), 0.5 * RedEdge2Length * cos(radians(angle4)), 0.0));
        partMatrix = TleftRedEdge2 * scalingMatrix;
        RedEdge2 = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &RedEdge2[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // top bar

        glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);
        scalingMatrix = rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 0.0f, -1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, TopBardLength, 0.3f));
        TtopRedEdge3 = TleftRedEdge2 * translate(mat4(1.0f), vec3(-(WhiteEdge1Length * sin(radians(angle1)) - WhiteEdge2Length * sin(radians(angle2)) - 0.5 * RedEdge2Length
            * sin(radians(angle3))), 0.5 * RedEdge2Length * cos(radians(angle3)) - 0.5, 0.0));
        partMatrix = TtopRedEdge3 * scalingMatrix;
        glm::mat4 topEdge = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &topEdge[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //net
        //vertical line

        glUniform3f(colorLocation, 80 / 255.0f, 161 / 255.0f, 0.0f);
        scalingMatrix = translate(mat4(1.0f), vec3(0.5 * netLengthX, 0.0, 0.0)) * rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 0.0f, -1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, netLengthX - 1, 0.2f));
        net = TleftWhiteEdge1 * translate(mat4(1.0f), vec3(-0.5 * WhiteEdge1Length * sin(radians(angle2)), 0.5 * WhiteEdge1Length * cos(radians(angle2)) - 1, 0.0));
        partMatrix = net * scalingMatrix;
        glm::mat4 netEdges = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &netEdges[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        for (int i = 0; i <= 12; ++i)
        {

            glUniform3f(colorLocation, 80 / 255.0f, 161 / 255.0f, 0.0f);
            scalingMatrix = translate(mat4(1.0f), vec3(0.5 * netLengthX, 0.0, 0.0)) * rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 0.0f, -1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, netLengthX, 0.2f));
            net = TleftWhiteEdge1 * translate(mat4(1.0f), vec3(-0.5 * WhiteEdge1Length * sin(radians(angle2)), 0.5 * WhiteEdge1Length * cos(radians(angle2)) + i, 0.0));
            partMatrix = net * scalingMatrix;
            netEdges = groupMatrix * partMatrix;
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &netEdges[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }

        glUniform3f(colorLocation, 80 / 255.0f, 161 / 255.0f, 0.0f);
        scalingMatrix = translate(mat4(1.0f), vec3(0.5 * netLengthX, 0.0, 0.0)) * rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 0.0f, -1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, netLengthX, 0.2f));
        net = TleftWhiteEdge1 * translate(mat4(1.0f), vec3(-0.5 * WhiteEdge1Length * sin(radians(angle2)) + 0.5, 0.5 * WhiteEdge1Length * cos(radians(angle2)) + 13, 0.0));
        partMatrix = net * scalingMatrix;
        netEdges = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &netEdges[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f);
        scalingMatrix = translate(mat4(1.0f), vec3(0.5 * netLengthX, 0.0, 0.0)) * rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 0.0f, -1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, netLengthX - 3, 0.2f));
        net = TleftWhiteEdge1 * translate(mat4(1.0f), vec3(-0.5 * WhiteEdge1Length * sin(radians(angle2)) + 0.5, 0.5 * WhiteEdge1Length * cos(radians(angle2)) - 1.5, 0.0));
        partMatrix = net * scalingMatrix;
        netEdges = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &netEdges[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Horizontal lines
        glUniform3f(colorLocation, 80 / 255.0f, 161 / 255.0f, 0.0f);
        scalingMatrix = translate(mat4(1.0f), vec3(0.0, 0.5 * netLengthY, 0.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, netLengthY, 0.2f));
        net = TleftWhiteEdge1 * translate(mat4(1.0f), vec3(-0.5 * (WhiteEdge1Length)*sin(radians(angle2)) + 1, 0.5 * (WhiteEdge1Length)*cos(radians(angle2)) - 1.5, 0.0));
        partMatrix = net * scalingMatrix;
        netEdges = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &netEdges[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);



        for (int i = 2; i <= 9; ++i)
        {

            glUniform3f(colorLocation, 80 / 255.0f, 161 / 255.0f, 0.0f);
            scalingMatrix = translate(mat4(1.0f), vec3(0.0, 0.5 * netLengthY, 0.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, netLengthY, 0.2f));
            net = TleftWhiteEdge1 * translate(mat4(1.0f), vec3(-0.5 * (WhiteEdge1Length)*sin(radians(angle2)) + i, 0.5 * (WhiteEdge1Length)*cos(radians(angle2)) - 1, 0.0));
            partMatrix = net * scalingMatrix;
            netEdges = groupMatrix * partMatrix;
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &netEdges[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }



        glUniform3f(colorLocation, 80 / 255.0f, 161 / 255.0f, 0.0f);
        scalingMatrix = translate(mat4(1.0f), vec3(0.0, 0.5 * netLengthY, 0.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, netLengthY, 0.2f));
        net = TleftWhiteEdge1 * translate(mat4(1.0f), vec3(-0.5 * (WhiteEdge1Length)*sin(radians(angle2)) + 10, 0.5 * (WhiteEdge1Length)*cos(radians(angle2)) - 1.5, 0.0));
        partMatrix = net * scalingMatrix;
        netEdges = groupMatrix * partMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &netEdges[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        dx = mousePosX - lastMousePosX;
        dy = mousePosY - lastMousePosY;


        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) // zoom out
        {
            fov += 0.01;
            projectionMatrix = glm::perspective(fov,            // field of view in degrees
                1024.0f / 768.0f,  // aspect ratio
                0.01f, 1000.0f);   // near and far (near > 0)

            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
        }

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) // zoom in
        {
            fov -= 0.01;
            mat4 projectionMatrix = glm::perspective(fov,            // field of view in degrees
                1024.0f / 768.0f,  // aspect ratio
                0.01f, 1000.0f);   // near and far (near > 0)

            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
        }



        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) // Counter Clockwise rotation around world coordianate X-axis;
        {
            base = base * rotate(mat4(1.0f), radians(3.0f), vec3(1.0, 0.0, 0.0));
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) // Clockwise rotation around world coordianate X-axis;
        {
            base = base * rotate(mat4(1.0f), radians(-3.0f), vec3(1.0, 0.0, 0.0));
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // Counter Clockwise rotation around world coordianate Y-axis;
        {
            base = base * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // Clockwise rotation around world coordianate Y-axis;
        {
            base = base * rotate(mat4(1.0f), radians(-3.0f), vec3(0.0, 1.0, 0.0));
        }
        if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) // back to initial position no matter it's shifted,scaled, or rotated.
        {
            base = mat4(1.0);
            cameraPosition = vec3(0.0f, 40.0f, 90.0f);
            cameraLookAt = vec3(0.0f, 20.0f, -1.0f);
            cameraUp = vec3(0.0f, 1.0f, 0.0f);
            Groupbase = mat4(1.0);
            fov = 70.0;
            projectionMatrix = glm::perspective(fov,            // field of view in degrees
                1024.0f / 768.0f,  // aspect ratio
                0.01f, 1000.0f);   // near and far (near > 0)

            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // reset the model position
        {
            Groupbase = mat4(1.0);
        }

        if (flag1 == true)//this is the flag when mouse right button mousecallback is triggered
        {
            cameraLookAt = cameraLookAt - vec3(dx * 0.01 * sensitivity, 0.0, 0.0);

        }

        if (flag2 == true)//this is the flag when mouse middle button mousecallback is triggered
        {
            cameraLookAt = cameraLookAt + vec3(0.0, dy * 0.01 * sensitivity, 0.0);
        }


        if (flag3 == true)//this is the flag when mouse left button mousecallback is triggered
        {
            fov += dx * sensitivity * 0.001;
            projectionMatrix = glm::perspective(fov,            // field of view in degrees
                1024.0f / 768.0f,  // aspect ratio
                0.01f, 1000.0f);   // near and far (near > 0)

            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
        }


        // This was solution for Lab02 - Moving camera exercise
        // We'll change this to be a first or third person camera
        bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;


        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move model to left
        {
            Groupbase = translate(mat4(1.0f), vec3(-0.2, 0.0, 0.0)) * Groupbase;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)  // move model to the right
        {
            Groupbase = translate(mat4(1.0f), vec3(0.2, 0.0, 0.0)) * Groupbase;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)  // move model down
        {
            Groupbase = translate(mat4(1.0f), vec3(0.0, -0.2, 0.0)) * Groupbase;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)  // move model up
        {
            Groupbase = translate(mat4(1.0f), vec3(0.0, 0.2, 0.0)) * Groupbase;
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) // move model along +Z axis
        {
            Groupbase = translate(mat4(1.0f), vec3(0.0, 0.0, 0.2)) * Groupbase;
        }

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) // move model along -Z axis
        {
            Groupbase = translate(mat4(1.0f), vec3(0.0, 0.0, -0.2)) * Groupbase;
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // Counter Clockwise rotation around world coordianate Y-axis;
        {
            Groupbase = rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0)) * Groupbase;
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // Clockwise rotation around world coordianate Y-axis;
        {
            Groupbase = rotate(mat4(1.0f), radians(-3.0f), vec3(0.0, 1.0, 0.0)) * Groupbase;
        }

        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) // Counter Clockwise rotation around Model Y-axis;
        {
            Groupbase = Groupbase * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) // Clockwise rotation around Model Y-axis;
        {
            Groupbase = Groupbase * rotate(mat4(1.0f), radians(-3.0f), vec3(0.0, 1.0, 0.0));
        }


        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) //size up
        {
            Groupbase = Groupbase * scale(mat4(1.0f), vec3(1.01, 1.01, 1.01));
        }

        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) //size down
        {
            Groupbase = Groupbase * scale(mat4(1.0f), vec3(0.95, 0.95, 0.95));
        }


        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) // Pan to the right
        {
            cameraLookAt = cameraLookAt + vec3(1, 0.0, 0.0);
        }

        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) // Pan to the left
        {
            cameraLookAt = cameraLookAt - vec3(1, 0.0, 0.0);
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) // Tilt up
        {
            cameraLookAt = cameraLookAt + vec3(0.0, 1.0, 0.0);
        }

        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) // Tilt down
        {
            cameraLookAt = cameraLookAt - vec3(0.0, 1.0, 0.0);
        }



        mat4 viewMatrix = lookAt(cameraPosition, cameraLookAt, cameraUp);

        GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);




    }


    glfwTerminate();

    return 0;
}

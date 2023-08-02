
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

#define GLEW_STATIC 1   
#include <GL/glew.h>   
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>


using namespace glm;
using namespace std;
vec3 cameraPosition;
vec3 cameraLookAt;
vec3 cameraUp;
mat4 base;
mat4 racket1Base, racket2Base, racket3Base, racket4Base;

mat4 Groupbase;
double dx, dy;
float dt;
float sensitivity = 0.01;
int SelectedIndex = 999;
int renderingMode = GL_FILL; // Default: Filled triangles


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
        "in vec3 vertexNormal;"
        "uniform vec3 mycolor;" // Uniform variable for color
        "out vec4 FragColor;"
        "void main()"
        "{"
        "   vec3 lightColor = vec3(255.0, 255.0, 255.0);"
        "   vec3 lightDirection = normalize(vec3(0.0, -0.5, 0.0));" // Light direction representing sunlight
        "   float ambientStrength = 0.2;" // Adjust the ambient strength as needed
        "   float diffuseStrength = 0.8;" // Adjust the diffuse strength as needed
        "   float specularStrength = 0.5;" // Adjust the specular strength as needed
        "   vec3 ambient = ambientStrength * mycolor;"
        "   float diffuseFactor = max(dot(vertexNormal, lightDirection), 0.0);"
        "   vec3 diffuse = diffuseStrength * diffuseFactor * mycolor;"
        "   vec3 viewDirection = normalize(vec3(0.0, 0.0, -1.0));" // Example view direction (you can change this)
        "   vec3 reflectDirection = reflect(-lightDirection, vertexNormal);"
        "   float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);"
        "   vec3 specular = specularStrength * specularFactor * lightColor;"
        "   vec3 finalColor = ambient + diffuse + specular;"
        "   FragColor = vec4(finalColor, 1.0f);"
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
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Quiz1_Junpeng Gai)40009896", NULL, NULL);
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
    cameraPosition = vec3(0.0f, 40.0f, 60.0f);
    cameraLookAt = vec3(0.0f, 20.0f, -1.0f);
    cameraUp = vec3(0.0f, 1.0f, 0.0f);
    base = mat4(1.0);
    racket1Base = mat4(1.0);
    racket2Base = mat4(1.0);
    racket3Base = mat4(1.0);
    racket4Base = mat4(1.0);
    mat4 Groupbase = mat4(1.0);
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
    float angle4 = 60;
    float AxisRotationSpeed = 3.0;
    float armlength = 10;
    float WhiteEdge1Length = 7;
    float RedEdge1Length = 12;
    float WhiteEdge2Length = 2;
    float RedEdge2Length = 2;
    float TopBardLength = 6;


    float gridUnit = 1.0f;//unit length of grid

    //dynamic net length calculation 
    float netLengthY = RedEdge1Length + WhiteEdge2Length * cos(radians(angle3)) + RedEdge2Length * cos(radians(angle4)) + 1;
    float netLengthX = 2 * WhiteEdge1Length * sin(radians(angle2)) - 1;
    //racket parts
    glm::mat4 Tlowerarm, Tupperarm, Tpole, TleftWhiteEdge1, TleftRedEdge1, TleftWhiteEdge2, TleftRedEdge2, TtopRedEdge3, net;



    mat4 OrigionIndex[4] = { mat4(1.0)*translate(mat4(1.0f),vec3(-20*gridUnit,0.0,0.0)), mat4(1.0), mat4(1.0), mat4(1.0)};

    
    float fov = 70.0;//set fov
   // Set projection matrix for shader, this won't change
    mat4 projectionMatrix = glm::perspective(fov,            // field of view in degrees
        1024.0f / 768.0f,  // aspect ratio
        0.01f, 10000.0f);   // near and far (near > 0)
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
        cameraPosition + cameraLookAt,  // center
        cameraUp); // up
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

    // Define and upload geometry to the GPU here ...
    int vao = createVertexBufferObject();
    
    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;

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

        // Drawing geometry starts here
        glBindVertexArray(vao);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT); // Cull the front faces
        glFrontFace(GL_CCW);  // Set the front face winding order (assuming counter-clockwise vertex order for front faces)



        // cube1 which is the huge cube with color of sky blue
        glUniform3f(colorLocation, 135/255.0, 206 / 255.0, 235 / 255.0);
        mat4 groundWorldMatrix = base;
        mat4 groundWorldMatrixscale = scale(mat4(1.0f), vec3(gridUnit* 80.0f, gridUnit * 80.0f, gridUnit * 80.0f));
        mat4 groundWorldMatrixTogether = groundWorldMatrix * groundWorldMatrixscale;
        GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrixTogether[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        glDisable(GL_CULL_FACE);//turn of the front cull

        //X Y Z coordinate drawing

        //x red
        glUniform3f(colorLocation, 1.0, 0.0, 0.0);
        groundWorldMatrix = base * translate(mat4(1.0f), vec3(5 * gridUnit / 2, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5 * gridUnit, 1.0f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        //y gree
        glUniform3f(colorLocation, 0.0, 1.0, 0.0);
        groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f, 5 * gridUnit / 2, 0.0f)) * scale(mat4(1.0f), vec3(1.0f, 5 * gridUnit, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //z blue
        glUniform3f(colorLocation, 0.0, 0.0, 1.0);
        groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f, 0.0f, (5 * gridUnit) / 2)) * scale(mat4(1.0f), vec3(1.0f, 1.0f, 5 * gridUnit));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Court drawing

        //grass
        glUniform3f(colorLocation, 124 / 255.0, 252 / 255.0,0.0);
         groundWorldMatrix = base*translate(mat4(1.0),vec3(0.0,-1.0,0.0));
         groundWorldMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 80.0f, 0.0f, gridUnit * 80.0f));
         groundWorldMatrixTogether = groundWorldMatrix * groundWorldMatrixscale;
         worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrixTogether[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0


       
        
        //main court
        glUniform3f(colorLocation, 81/255.0, 110/255.0, 88/255.0);
        groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(78.0f, 0.05, 36.0));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        for (int i = 0; i <= 10; ++i)
        {

            glUniform3f(colorLocation, 0.0, 0.0, 0.0);
            groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f, gridUnit / 2 * i, 0.0f)) * scale(mat4(1.0f), vec3(0.1f, 0.1f, gridUnit * 36.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }


        //net  in the middle
        for (int i = 0; i < 19; ++i)
        {
            //grid along Z axis
            glUniform3f(colorLocation, 1.0, 1.0, 1.0);
            groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f + gridUnit * i)) * scale(mat4(1.0f), vec3(gridUnit * 78.0f, 0.1f, 0.1f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            //net and pole2
            glUniform3f(colorLocation, 0.0, 0.0, 0.0);
            groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f, gridUnit * 5.0 / 2, 0.0f + gridUnit * i)) * scale(mat4(1.0f), vec3(0.1f, gridUnit * 5.0, 0.1f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);


        }


        for (int i = 0; i < 19; ++i)
        {

            //grid along Z axis
            glUniform3f(colorLocation, 1.0, 1.0, 1.0);
            groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f - gridUnit * i)) * scale(mat4(1.0f), vec3(gridUnit * 78.0f, 0.1f, 0.1f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            //net and pole2
            glUniform3f(colorLocation, 0.0, 0.0, 0.0);
            groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f, gridUnit * 5.0 / 2, 0.0f - gridUnit * i)) * scale(mat4(1.0f), vec3(0.1f, gridUnit * 5.0, 0.1f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }

        //grid along X axis

        for (int i = 0; i < 40; ++i)
        {
            glUniform3f(colorLocation, 1.0, 1.0, 1.0);
            groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f + gridUnit * i, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.1f, 0.1f, gridUnit * 36.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }


        //grid along X axis
        for (int i = 0; i < 40; ++i)
        {

            glUniform3f(colorLocation, 1.0, 1.0, 1.0);
            groundWorldMatrix = base * translate(mat4(1.0f), vec3(0.0f - gridUnit * i, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.1f, 0.1f, gridUnit * 36.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }
        //4 instance here, using a for loop
        for(int num=1;num<=4;num++)
        {
            
    //lower arm   45 degree
           Groupbase = translate(mat4(1.0f), vec3(-2 * 10 * gridUnit +num*10*gridUnit,0.0,0.0)) * scale(mat4(1.0), vec3(0.7, 0.7, 0.7));
           glm::mat4 groupMatrix = base * Groupbase * OrigionIndex[num-1];//change origion for each racket as a function of number of loops(nums)
          
           //each character has 3 layer as stacked style.
           for (int layer = 0; layer <= 2; layer++)
           {
               if (num == 1)//for left most racket
               {
                   //drawing character G
                   //change color as a function of number of loops
                   glUniform3f(colorLocation, 1.0, 1.0-0.15*layer, 0.0);
                   mat4 CharacterG = groupMatrix * translate(mat4(1.0f), vec3(0.0, 60.0, layer));
                   mat4 CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 5.0f, gridUnit * 1.0f, gridUnit * 1.0f));
                   mat4 CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                   GLuint CharacterGLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);

                   CharacterGMatrixTogether = CharacterGMatrixTogether * translate(mat4(1.0f), vec3(0.0, 7.0f, 0.0f));
                   CharacterGLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);

                   CharacterG = CharacterG * translate(mat4(1.0f), vec3(-3.0, 3.5, 0.0));
                   CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 1.0f, gridUnit * 8.0f, gridUnit * 1.0f));
                   CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                   CharacterGLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);

                   CharacterG = CharacterG * translate(mat4(1.0f), vec3(6.0, 2.5, 0.0));
                   CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 1.0f, gridUnit * 3.0f, gridUnit * 1.0f));
                   CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                   CharacterGLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);

                   CharacterG = CharacterG * translate(mat4(1.0f), vec3(0.0, -5, 0.0));
                   CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 1.0f, gridUnit * 3.0f, gridUnit * 1.0f));
                   CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                   CharacterGLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);

                   CharacterG = CharacterG * translate(mat4(1.0f), vec3(-1.5, 1.5, 0.0));
                   CharacterGMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 4.0f, gridUnit * 1.0f, gridUnit * 1.0f));
                   CharacterGMatrixTogether = CharacterG * CharacterGMatrixscale;
                   CharacterGLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterGMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);
               }


               if (num == 2)//for 2ed racket
               {
                   //drawing character A
                   glUniform3f(colorLocation, 1.0 - 0.15 * layer, 0.0, 1.0);
                   mat4 CharacterA = groupMatrix * translate(mat4(1.0f), vec3(0.0, 40.0, layer));
                   mat4 CharacterAMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 7.0f, gridUnit * 1.0f, gridUnit * 1.0f));
                   mat4 CharacterAMatrixTogether = CharacterA * CharacterAMatrixscale;
                   GLuint CharacterALocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterAMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);

                   CharacterA = CharacterA * translate(mat4(1.0f), vec3(0.0, 4, 0.0));
                   CharacterAMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 7.0f, gridUnit * 1.0f, gridUnit * 1.0f));
                   CharacterAMatrixTogether = CharacterA * CharacterAMatrixscale;
                   CharacterALocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterAMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);

                   CharacterA = CharacterA * translate(mat4(1.0f), vec3(-3.5, -4.0, 0.0));
                   CharacterAMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 1.0f, gridUnit * 8.0f, gridUnit * 1.0f));
                   CharacterAMatrixTogether = CharacterA * CharacterAMatrixscale;
                   CharacterALocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterAMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);

                   CharacterA = CharacterA * translate(mat4(1.0f), vec3(+7, 0.0, 0.0));
                   CharacterAMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 1.0f, gridUnit * 8.0f, gridUnit * 1.0f));
                   CharacterAMatrixTogether = CharacterA * CharacterAMatrixscale;
                   CharacterALocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterAMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);


               }



               if (num == 3)//for 3rd racket
               {
                   //drawing character I
                   glUniform3f(colorLocation, 0.0, 1.0, 1.0 - 0.15 * layer);
                   mat4 CharacterI = groupMatrix * translate(mat4(1.0f), vec3(0.0, 40, layer));
                   mat4 CharacterIMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 1.0f, gridUnit * 8.0f, gridUnit * 1.0f));
                   mat4 CharacterIMatrixTogether = CharacterI * CharacterIMatrixscale;
                   GLuint CharacterALocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterIMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);
               }


               if (num == 4)//for right most racket
               {
                   //drawing character J
                   glUniform3f(colorLocation, 1.0, 1.0 - 0.15 * layer, 1.0);
                   mat4 CharacterJ = groupMatrix * translate(mat4(1.0f), vec3(4.0, 40, layer));
                   mat4 CharacterJMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 1.0f, gridUnit * 8.0f, gridUnit * 1.0f));
                   mat4 CharacterJMatrixTogether = CharacterJ * CharacterJMatrixscale;
                   GLuint CharacterALocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterJMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);

                   CharacterJ = CharacterJ * translate(mat4(1.0f), vec3(-1.5, +4, 0.0));
                   CharacterJMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 4.0f, gridUnit * 1.0f, gridUnit * 1.0f));
                   CharacterJMatrixTogether = CharacterJ * CharacterJMatrixscale;
                   CharacterALocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterJMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);

                   CharacterJ = CharacterJ * translate(mat4(1.0f), vec3(-2.5, -8.0, 0.0));
                   CharacterJMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 8.0f, gridUnit * 1.0f, gridUnit * 1.0f));
                   CharacterJMatrixTogether = CharacterJ * CharacterJMatrixscale;
                   CharacterALocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterJMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);

                   CharacterJ = CharacterJ * translate(mat4(1.0f), vec3(-4, 1.0, 0.0));
                   CharacterJMatrixscale = scale(mat4(1.0f), vec3(gridUnit * 1.0f, gridUnit * 3.0f, gridUnit * 1.0f));
                   CharacterJMatrixTogether = CharacterJ * CharacterJMatrixscale;
                   CharacterALocation = glGetUniformLocation(shaderProgram, "worldMatrix");
                   glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &CharacterJMatrixTogether[0][0]);
                   glDrawArrays(GL_TRIANGLES, 0, 36);
               }
           }

           

            //Drawing racket
            //racket pole
            glUniform3f(colorLocation, 133 / 255.0f, 0.0f, 0.0f);
            glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.6f, armlength, 0.6f));
            Tpole = translate(mat4(1.0f), vec3(0.0f, armlength, 0.0f));
            glm::mat4 partMatrix = Tpole * scalingMatrix;
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


            //glm::mat4 Tlowerarm, Tupperarm, Tpole, TleftWhiteEdge1, TleftRedEdge1, TleftWhiteEdge2, TleftRedEdge2, TtopRedEdge3;





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
             //vertical

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

            //Horizontal
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
        }





       
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // Select 1st racket
        {
            base = mat4(1.0);
            cameraPosition = vec3(-2 * 10 * gridUnit + 1 * 10 * gridUnit, 20.0f, 60.0f);
            cameraLookAt = vec3(-2 * 10 * gridUnit + 1 * 10 * gridUnit, 20.0f, -1.0f);
            cameraUp = vec3(0.0f, 1.0f, 0.0f);
            SelectedIndex = 0;
            //change the flag to current racket so we can only rotate current racket

        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // Select 2ed racket
        {
            base = mat4(1.0);
            cameraPosition = vec3(-2 * 10 * gridUnit + 2 * 10 * gridUnit, 20.0f, 60.0f);
            cameraLookAt = vec3(-2 * 10 * gridUnit + 2 * 10 * gridUnit, 20.0f, -1.0f);
            cameraUp = vec3(0.0f, 1.0f, 0.0f);
            SelectedIndex = 1;
            //change the flag to current racket so we can only rotate current racket
        }

        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) // Select 3rd racket
        {
            base = mat4(1.0);
            cameraPosition = vec3(-2 * 10 * gridUnit + 3 * 10 * gridUnit, 20.0f, 60.0f);
            cameraLookAt = vec3(-2 * 10 * gridUnit + 3 * 10 * gridUnit, 20.0f, -1.0f);
            cameraUp = vec3(0.0f, 1.0f, 0.0f);
            SelectedIndex = 2;
            //change the flag to current racket so we can only rotate current racket
        }

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) // Select 4th racket
        {
            base = mat4(1.0);
            cameraPosition = vec3(-2 * 10 * gridUnit + 4 * 10 * gridUnit, 20.0f, 60.0f);
            cameraLookAt = vec3(-2 * 10 * gridUnit + 4 * 10 * gridUnit, 20.0f, -1.0f);
            cameraUp = vec3(0.0f, 1.0f, 0.0f);
            SelectedIndex = 3;
            //change the flag to current racket so we can only rotate current racket
        }



        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) // clockwise rotation of the whole module about X axis
        {
            base = base * rotate(mat4(1.0f), radians(-3.0f), vec3(1.0, 0.0, 0.0));
            SelectedIndex = 999;
            //change the flag to 999 ,so we can't rotate any racket
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) // couter clockwise rotation of the whole module about X axis
        {
            base = base * rotate(mat4(1.0f), radians(3.0f), vec3(1.0, 0.0, 0.0));
            SelectedIndex = 999;
            //change the flag to 999 ,so we can't rotate any racket
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // // couter clockwise rotation of the whole module about Y axis
        {
            base = base * rotate(mat4(1.0f), radians(-3.0f), vec3(0.0, 1.0, 0.0));
            SelectedIndex = 999;
            //change the flag to 999 ,so we can't rotate any racket
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // // clockwise rotation of the whole module about Y axis
        {
            base = base * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
            SelectedIndex = 999;
            //change the flag to 999 ,so we can't rotate any racket
        }
        if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) // reset the camera and module to his initial state
        {
            base = mat4(1.0);
            cameraPosition = vec3(0.0f, 40.0f, 90.0f);
            cameraLookAt = vec3(0.0f, 20.0f, -1.0f);
            cameraUp = vec3(0.0f, 1.0f, 0.0f);
            SelectedIndex = 999;
            //change the flag to 999 ,so we can't rotate any racket

        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // reset racket to it's initial state
        {
            if (SelectedIndex != 999)
            {
                OrigionIndex[SelectedIndex] = mat4(1.0);
            }
            else
            {
                OrigionIndex[0] = mat4(1.0);
                OrigionIndex[1] = mat4(1.0);
                OrigionIndex[2] = mat4(1.0);
                OrigionIndex[3] = mat4(1.0);
            }
        }



        // This was solution for Lab02 - Moving camera exercise
        // We'll change this to be a first or third person camera

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
        {
            base = base * translate(mat4(1.0f), vec3(0.2, 0.0, 0.0));
            SelectedIndex = 999;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
        {
            base = base * translate(mat4(1.0f), vec3(-0.2, 0.0, 0.0));
            SelectedIndex = 999;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera down
        {
            base = base * translate(mat4(1.0f), vec3(0.0, 0.2, 0.0));
            SelectedIndex = 999;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera up
        {
            base = base * translate(mat4(1.0f), vec3(0.0, -0.2, 0.0));
            SelectedIndex = 999;
        }


        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // counter clockwise rotate racket about its own Y axis
        {
            if (SelectedIndex != 999)
            {
                OrigionIndex[SelectedIndex] = OrigionIndex[SelectedIndex] * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
            }
          
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)  // clockwise rotate racket about its own Y axis
        {
            if (SelectedIndex != 999)
            {
                OrigionIndex[SelectedIndex] = OrigionIndex[SelectedIndex] * rotate(mat4(1.0f), radians(-3.0f), vec3(0.0, 1.0, 0.0));
            }
           
        }



        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) // move camera to the right
        {
            base = base * scale(mat4(1.0f), vec3(1.01, 1.01, 1.01));
        }

        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) // move camera to the right
        {
            base = base * scale(mat4(1.0f), vec3(0.95, 0.95, 0.95));
        }


        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) // move camera to the right
        {
            cameraLookAt = cameraLookAt + vec3(1.0, 0.0, 0.0);
        }

        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) // move camera to the right
        {
            cameraLookAt = cameraLookAt - vec3(1.0, 0.0, 0.0);
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) // move camera to the right
        {
            cameraLookAt = cameraLookAt + vec3(0.0, 1.0, 0.0);
        }

        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) // move camera to the right
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


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
		"layout (location = 2) in vec3 Normal;"
		""
		"uniform mat4 worldMatrix;"
		"uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
		"uniform mat4 projectionMatrix = mat4(1.0);"
		""
		"out vec3 vertexColor;"
		"out vec3 aNormal;"
		"void main()"
		"{"
		"   vertexColor = aColor;"
		"   aNormal=Normal ; "
		"   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
		"   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
		"}";
}


const char* getFragmentShaderSource()
{
	return
		"#version 330 core\n"
		"in vec3 vertexColor;"
		"in vec3 aNormal;"
		"uniform vec3 mycolor;" // Uniform variable for color
		"uniform vec3 lightPosition=vec3(0.0,80,0.0);" // Position of the light source
		"uniform vec3 lightColor=vec3(1,0.0,0.0);" // Color of the light source
		"uniform vec3 viewPosition= vec3(0.0f, 0.0f, 45.0f);" // Position of the camera (viewer)
		"out vec4 FragColor;"
		"void main()"
		"{"
		"   vec3 ambient = 0.1 * mycolor;" // Ambient lighting intensity
		"   vec3 normal = normalize(aNormal);"
		"   vec3 lightDir = normalize(lightPosition - gl_FragCoord.xyz);" // Direction from the fragment to the light source
		"   vec3 viewDir = normalize(viewPosition - gl_FragCoord.xyz);" // Direction from the fragment to the camera (viewer)
		"   vec3 reflectDir = reflect(-lightDir, normal);" // Reflection direction of the light
		"   float specStrength = 0.5;" // Specular intensity (adjust this as needed)
		"   vec3 specular = specStrength * lightColor * pow(max(dot(viewDir, reflectDir), 0.0), 32);" // Specular lighting
		"   vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;" // Diffuse lighting
		"   vec3 resultColor = ambient + diffuse + specular;" // Final color with lighting
		"   FragColor = vec4(resultColor * mycolor, 1.0);" // Apply the color and alpha
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
		vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),vec3(-1,0.0,0.0), //left - red
		vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),vec3(-1,0.0,0.0),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),vec3(-1,0.0,0.0),

		vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),vec3(-1,0.0,0.0),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),vec3(-1,0.0,0.0),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),vec3(-1,0.0,0.0),

		vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),vec3(0.0,0.0,-1.0), // far - blue
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),vec3(0.0,0.0,-1.0),
		vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),vec3(0.0,0.0,-1.0),

		vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),vec3(0.0,0.0,-1.0),
		vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),vec3(0.0,0.0,-1.0),
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),vec3(0.0,0.0,-1.0),

		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec3(0.0,-1.0,0.0),// bottom - turquoise
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),vec3(0.0,-1.0,0.0),
		vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),vec3(0.0,-1.0,0.0),

		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),vec3(0.0,-1.0,0.0),
		vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),vec3(0.0,-1.0,0.0),
		vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),vec3(0.0,-1.0,0.0),

		vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),vec3(0.0,0.0,1.0), // near - green
		vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),vec3(0.0,0.0,1.0),
		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),vec3(0.0,0.0,1.0),

		vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),vec3(0.0,0.0,1.0),
		vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),vec3(0.0,0.0,1.0),
		vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),vec3(0.0,0.0,1.0),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec3(1.0,0.0,0.0),// right - purple
		vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),vec3(1.0,0.0,0.0),
		vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),vec3(1.0,0.0,0.0),

		vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),vec3(1.0,0.0,0.0),
		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),vec3(1.0,0.0,0.0),
		vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),vec3(1.0,0.0,0.0),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec3(0.0,1.0,0.0),// top - yellow
		vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),vec3(0.0,1.0,0.0),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),vec3(0.0,1.0,0.0),

		vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f),vec3(0.0,1.0,0.0),
		vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),vec3(0.0,1.0,0.0),
		vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f),vec3(0.0,1.0,0.0)
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
		3 * sizeof(vec3), // stride - each vertex contain 2 vec3 (position, color)
		(void*)0             // array buffer offset
	);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(vec3),
		(void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2,                            // attribute 1 matches aColor in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(vec3),
		(void*)(2 * sizeof(vec3))      // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(2);

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



	mat4 OrigionIndex[4] = { mat4(1.0)*translate(mat4(1.0f),vec3(-20 * gridUnit,0.0,0.0)), mat4(1.0), mat4(1.0), mat4(1.0) };


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
;
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

	// cube1
	glUniform3f(colorLocation, 1.0, 174 / 255.0, 201 / 255.0);
	mat4 groundWorldMatrix = base * Groupbase *translate(mat4(1.0f), vec3(10.0f, -0.01f, 0.0f)) * scale(mat4(1.0f), vec3(3.0f, 3.0f, 3.0f));
	GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0


	//cube2
	glUniform3f(colorLocation, 1, 1.0, 0.0);
	groundWorldMatrix = base * Groupbase* translate(mat4(1.0f), vec3(3.0f, -0.01f, 0.0f)) * translate(mat4(1.0f), vec3(10.0f, -0.01f, 0.0f)) * scale(mat4(1.0f), vec3(3.0f, 5.0f, 5.0f));
	worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0



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



	//glm::mat4 Tlowerarm, Tupperarm, Tpole, TleftWhiteEdge1, TleftRedEdge1, TleftWhiteEdge2, TleftRedEdge2, TtopRedEdge3;
   //lower arm   45 degree
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






	// End Frame
	glfwSwapBuffers(window);
	glfwPollEvents();

	// Handle inputs
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // move camera down
	{
		cameraFirstPerson = true;
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // move camera down
	{
		cameraFirstPerson = false;
	}

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) // move camera down
	{
		fov += 0.01;
		projectionMatrix = glm::perspective(fov,            // field of view in degrees
			800.0f / 600.0f,  // aspect ratio
			0.01f, 100.0f);   // near and far (near > 0)

		GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	}

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) // move camera down
	{
		fov -= 0.01;
		mat4 projectionMatrix = glm::perspective(fov,            // field of view in degrees
			800.0f / 600.0f,  // aspect ratio
			0.01f, 100.0f);   // near and far (near > 0)

		GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	}



	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) // move camera down
	{
		base = base * rotate(mat4(1.0f), radians(3.0f), vec3(1.0, 0.0, 0.0));
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) // move camera down
	{
		base = base * rotate(mat4(1.0f), radians(-3.0f), vec3(1.0, 0.0, 0.0));
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // move camera down
	{
		base = base * rotate(mat4(1.0f), radians(3.0f), vec3(0.0, 1.0, 0.0));
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // move camera down
	{
		base = base * rotate(mat4(1.0f), radians(-3.0f), vec3(0.0, 1.0, 0.0));
	}
	if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) // move camera down
	{
		base = mat4(1.0);
		cameraPosition = vec3(0.0f, 40.0f, 90.0f);
		cameraLookAt = vec3(0.0f, 20.0f, -1.0f);
		cameraUp = vec3(0.0f, 1.0f, 1.0f);

	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // move camera down
	{
		Groupbase = mat4(1.0);
	}




	// This was solution for Lab02 - Moving camera exercise
	// We'll change this to be a first or third person camera
	bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
	float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;


	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
	{
		Groupbase = Groupbase * translate(mat4(1.0f), vec3(-0.2, 0.0, 0.0));
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
	{
		Groupbase = Groupbase * translate(mat4(1.0f), vec3(0.2, 0.0, 0.0));
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera up
	{
		Groupbase = Groupbase * translate(mat4(1.0f), vec3(0.0, -0.2, 0.0));
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera down
	{
		Groupbase = Groupbase * translate(mat4(1.0f), vec3(0.0, 0.2, 0.0));
	}


	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // move camera to the left
	{
		Groupbase = rotate(mat4(1.0f), radians(3.0f), vec3(1.0, 0.0, 0.0))*Groupbase;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // move camera to the right
	{
		Groupbase = rotate(mat4(1.0f), radians(-3.0f), vec3(1.0, 0.0, 0.0))*Groupbase;
	}

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) // move camera to the left
	{
		Groupbase = Groupbase * rotate(mat4(1.0f), radians(3.0f), vec3(1.0, 0.0, 0.0));
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) // move camera to the right
	{
		Groupbase = Groupbase * rotate(mat4(1.0f), radians(-3.0f), vec3(1.0, 0.0, 0.0));
	}


	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) // move camera to the right
	{
		Groupbase = Groupbase * scale(mat4(1.0f), vec3(1.01, 1.01, 1.01));
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) // move camera to the right
	{
		Groupbase = Groupbase * scale(mat4(1.0f), vec3(0.95, 0.95, 0.95));
	}


	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) // move camera to the right
	{
		cameraLookAt = cameraLookAt + vec3(1, 0.0, 0.0);
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) // move camera to the right
	{
		cameraLookAt = cameraLookAt - vec3(1, 0.0, 0.0);
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

#version 330 core

layout (location = 0) in vec3 aPosition; // Input attribute: Vertex position
layout (location = 1) in vec3 Normal;    // Input attribute: Vertex color
layout (location = 2) in vec2 aTexCoord;    // Input attribute: Vertex color

out vec3 vertexColor; // Output variable to pass color to the fragment shader
out vec2 TexCoord;
out vec3 outNormal;
out vec3 outFragPos;
out vec4 FragPosLightSpace;

uniform mat4 model= mat4(1.0);
uniform mat4 view= mat4(1.0);
uniform mat4 projection= mat4(1.0);
uniform mat4 lightSpaceMatrix;


void main()
{
	outFragPos=vec3(model*vec4(aPosition,1.0f));
	outNormal=mat3(transpose(inverse(model)))*Normal;
	TexCoord = aTexCoord;
	FragPosLightSpace= lightSpaceMatrix * vec4(outFragPos, 1.0);
	//none uniform scaling
	gl_Position = projection*view*model*vec4(aPosition, 1.0); // Set the position of the vertex
}

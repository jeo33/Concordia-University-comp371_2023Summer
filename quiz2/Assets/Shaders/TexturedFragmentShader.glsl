

#version 330 core
out vec4 FragColor;
in vec3 vertexColor;
in vec2 TexCoord;
in vec3 outFragPos;
in vec3 outNormal;
in vec4 FragPosLightSpace;


uniform sampler2D shadowMap;
uniform sampler2D ourTexture;

uniform float transparent=0.0;

uniform vec3 viewPoint; //camera
uniform float ambientStrength=0.9;
uniform vec3 lightcolor=vec3(1.0,1.0,1.0);

uniform vec3 lightpos;
uniform vec3 object_color ;
uniform float is_tex = 1.0 ; 

uniform float is_shadow = 1.0 ; 

uniform float spotlight = 1.0 ; 

uniform float pureTexture = 0.0 ; 

uniform float LightOff = 0.0 ; 


uniform vec3 lightcolor2=vec3(1.0,1.0,1.0);

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	
	
	float bias = 0.005;
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  
    //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
	if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}  

void main()
{
	


	vec3 ambinent=ambientStrength*lightcolor;
	
	vec3 norm=normalize(outNormal);
	vec3 lightDir=normalize(lightpos-outFragPos);
	float diff=max(dot(lightDir,norm),0.0);
	vec3 diffuse=diff*lightcolor;
	
	
	
	
	float specularStrength=0.8;
	vec3 viewDir=normalize(viewPoint-outFragPos);
	vec3 reflectDir=reflect(-lightDir,norm);
	float spect=pow(max(dot(viewDir,reflectDir),0.0),32);
	vec3 specular=specularStrength*spect*lightcolor;
	
	//**************************************************
	//camera2
	
	vec3 ambinent2=0.1*lightcolor2;
	
	vec3 norm2=normalize(outNormal);
	vec3 lightDir2=normalize(viewPoint-outFragPos);
	float diff2=max(dot(lightDir2,norm2),0.0);
	vec3 diffuse2=diff2*lightcolor2;
	
	
	vec3 viewDir2=normalize(viewPoint-outFragPos);
	vec3 reflectDir2=reflect(-lightDir2,norm2);
	float spect2=pow(max(dot(viewDir2,reflectDir2),0.0),256);
	vec3 specular2=specularStrength*spect2*lightcolor2;
	

	
	vec3 Cmera2lighting = (ambinent2 + diffuse2 + specular2);  
	
	
	//**************************************************
	
	
	
	float constnum=1;
	float linear=0.09;
	float quadratic=0.03;
	float distance    = length(lightpos - outFragPos);
	float attenuation = 1.0 / (constnum + linear * distance + quadratic * (distance * distance));  
	//ambinent  *= attenuation; 
	//diffuse  *= attenuation;
	//specular *= attenuation;  
	
	
	vec3 texColor =texture(ourTexture, TexCoord).rgb;
	
	
    
    // Output the final color with the original alpha value
	float shadow = (is_shadow==1.0)?ShadowCalculation(FragPosLightSpace):0.0;  
	vec3 lighting,TopLight;
	TopLight=(LightOff==0.0)?(ambinent + (1.0 - shadow) * (diffuse + specular)):vec3(0.0);
	if( is_tex == 1.0)
		lighting = (TopLight+(spotlight==1.0?Cmera2lighting:vec3(0.0))) * texColor;  
	else
		lighting = (TopLight+ (spotlight==1.0?Cmera2lighting:vec3(0.0)))* object_color;  
		
	float tlevel=(transparent==0.0)?1.0:0.5;
	
	if(pureTexture==1.0)
	{
		lighting = texColor;  
	}
	FragColor = vec4(lighting, tlevel);
}


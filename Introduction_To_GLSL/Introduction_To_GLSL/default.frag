#version 330 core

struct Material
{
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct PointLightData
{
	float aConst;
	float bConst;
};

struct SpotLightData
{
	vec3 spotLightPosition;
	vec3 cameraFront;
	vec3 spotLightDirection;
	float outerCone;
	float innerCone;
};

uniform Material cubeMaterial;

//uniform bool useBlinn;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform vec3 lightColor;

//Point Light Settings
uniform bool usePointLight;
uniform PointLightData pointLightData;

//Directional Light Settings
uniform bool useDirectionalLight;
uniform vec3 directionalLightDirection;

//Spot Light Settings
uniform bool useSpotLight;
uniform SpotLightData spotLightData; 

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPosition;
in vec2 TexCoord;

//Light map behaviour...
vec3 lightMap()
{
	vec3 ambient = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	
	//Ambient component calculation
	ambient = texture(cubeMaterial.diffuse, TexCoord).rgb * cubeMaterial.ambient * lightColor;

	//Diffuse component calculation
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPosition - FragPosition);
	//diffuse cosine
	float diff = max(dot(normal, lightDirection), 0.0);
	diffuse = diff * texture(cubeMaterial.diffuse, TexCoord).rgb * lightColor;

	//Specular component calculation
	vec3 viewDirection = normalize(cameraPosition - FragPosition);
	//Calculate reflection direction
	vec3 reflectionDir = reflect(-lightDirection, normal);
	//specular cosine (raised to the power of shininess)
	float spec = pow(max(dot(viewDirection, reflectionDir), 0.0), cubeMaterial.shininess);
	specular = spec * texture(cubeMaterial.specular, TexCoord).rgb * lightColor;
	vec3 finalColor = ambient + diffuse + specular;
	return finalColor;
}

//Point Light behaviour...
vec3 pointLight()
{
	vec3 lightVec = lightPosition - FragPosition;
	float distance = length(lightVec);
	float a = 0.7;
	float b = 0.04;
	float intensity = 1.0f / (pointLightData.aConst * distance * distance + pointLightData.bConst + distance + 1.0f);

	vec3 ambient = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	
	//Ambient component calculation
	ambient = texture(cubeMaterial.diffuse, TexCoord).rgb * cubeMaterial.ambient * lightColor;

	//Diffuse component calculation
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	//diffuse cosine
	float diff = max(dot(normal, lightDirection), 0.0);
	diffuse = diff * texture(cubeMaterial.diffuse, TexCoord).rgb * intensity * lightColor;

	//Specular component calculation
	vec3 viewDirection = normalize(cameraPosition - FragPosition);
	//Calculate reflection direction
	vec3 reflectionDir = reflect(-lightDirection, normal);
	//specular cosine (raised to the power of shininess)
	float spec = pow(max(dot(viewDirection, reflectionDir), 0.0), cubeMaterial.shininess);
	specular = spec * texture(cubeMaterial.specular, TexCoord).rgb * intensity * lightColor;
	vec3 finalColor = ambient + diffuse + specular;
	return finalColor;
}


//Directional Light behaviour...
vec3 directionalLight()
{
	vec3 ambient = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	
	//Ambient component calculation
	ambient = texture(cubeMaterial.diffuse, TexCoord).rgb * cubeMaterial.ambient * lightColor;

	//Diffuse component calculation
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(directionalLightDirection);
	//diffuse cosine
	float diff = max(dot(normal, lightDirection), 0.0);
	diffuse = diff * texture(cubeMaterial.diffuse, TexCoord).rgb * lightColor;

	//Specular component calculation
	vec3 viewDirection = normalize(cameraPosition - FragPosition);
	//Calculate reflection direction
	vec3 reflectionDir = reflect(-lightDirection, normal);
	//specular cosine (raised to the power of shininess)
	float spec = pow(max(dot(viewDirection, reflectionDir), 0.0), cubeMaterial.shininess);
	specular = spec * texture(cubeMaterial.specular, TexCoord).rgb * lightColor;
	vec3 finalColor = ambient + diffuse + specular;
	return finalColor;
}

//Spot light behaviour...
vec3 spotLight()
{
	vec3 ambient = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	
	//Ambient component calculation
	ambient = texture(cubeMaterial.diffuse, TexCoord).rgb * cubeMaterial.ambient * lightColor;


	//Diffuse component calculation
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPosition - FragPosition);

	
	//diffuse cosine
	float diff = max(dot(normal, lightDirection), 0.0);
	diffuse = diff * texture(cubeMaterial.diffuse, TexCoord).rgb * lightColor;

	//Specular component calculation
	vec3 viewDirection = normalize(cameraPosition - FragPosition);
	
	//Calculate reflection direction
	vec3 reflectionDir = reflect(-lightDirection, normal);
	
	//specular cosine (raised to the power of shininess)
	float spec = pow(max(dot(viewDirection, reflectionDir), 0.0), cubeMaterial.shininess);

	specular = spec * texture(cubeMaterial.specular, TexCoord).rgb * lightColor;
	
	float theta = dot(lightDirection, normalize(-spotLightData.spotLightDirection));
	float epsilon = (spotLightData.innerCone - spotLightData.outerCone);
	float intensity = clamp((theta - spotLightData.outerCone) / epsilon, 0.0f, 1.0f);

	diffuse *= intensity;
	specular *= intensity;

	vec3 finalColor = ambient + diffuse + specular;
	return finalColor;
}

void main()
{
	if(usePointLight) {
		FragColor = vec4(pointLight(), 1.0);
	}
	else if(useDirectionalLight) {
		FragColor = vec4(directionalLight(), 1.0);
	}
	else if(useSpotLight) {
		FragColor = vec4(spotLight(), 1.0);
	}
	else {
		FragColor = vec4(lightMap(), 1.0);
	}
}
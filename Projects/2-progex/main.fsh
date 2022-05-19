#version 330

// UV-coordinate of the fragment (interpolated by the rasterization stage)
in vec2 outUV;

// Color of the fragment received from the vertex shader (interpolated by the rasterization stage)
in vec3 outColor;

// Normal
in vec3 outNormal;


// Vertex Position 
in vec3 outPosition;

// Final color of the fragment that will be rendered on the screen
out vec4 fragColor;

// Texture unit of the texture
uniform sampler2D tex;

// Struct for material
struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};

//Struct for directional light
struct DirLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//Struct for Point Light
struct PointLight {
    vec3 position;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

//Struct for SpotLight
struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};



uniform mat4 view;
uniform vec3 camPosition;
uniform DirLight dLight;
uniform SpotLight spotLight;
uniform PointLight pLight;
uniform Material mat;

void main()
{
	//fragColor = vec4(1.0f,1.0f,1.0f, 1.0f);
	//fragColor = texture(tex, outUV);

    //--------------------------------------------------------------------------------
	// DIRECTIONAL LIGHT
    //Ambient lighting
	vec3 ambient = dLight.ambient * vec3(texture(tex, outUV)); 
	//vec3 ambient = dLight.ambient; 

    // Diffuse
    vec3 norm = normalize(outNormal);
    vec3 lightDirection = normalize(-1 * dLight.direction);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = dLight.diffuse * diff;
    //vec3 diffuse = dLight.diffuse * diff * vec3(texture(mat.diffuse, outUV));
    
    // Specular
    vec3 viewDir = normalize(camPosition - outPosition);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDirection), 0.0), mat.shininess);
    //vec3 specular = dLight.specular * spec * vec3(texture(mat.specular, outUV));
    vec3 specular = dLight.specular * spec;

    vec3 dirLightAns = (ambient + diffuse + specular);
    //--------------------------------------------------------------------------------
    // POINT LIGHT
    // Ambient
    vec3 pLightAmbient = pLight.ambient * vec3(texture(mat.diffuse, outUV));

    // Diffuse
    vec3 pLightNorm = normalize(outNormal);
    vec3 pLightDirection = normalize(pLight.position - outPosition);
    float pLightDiff = max(dot(pLightNorm, pLightDirection), 0.0);
    vec3 pLightDiffuse = pLight.diffuse * pLightDiff * vec3(texture(mat.diffuse, outUV));

    // Specular
    vec3 pLightViewDir = normalize(camPosition - outPosition);
    vec3 pLightReflectDirection = reflect(-pLightDirection, pLightNorm);
    float pLightSpec = pow(max(dot(pLightViewDir, pLightReflectDirection), 0.0), mat.shininess);
    vec3 pLightSpecular = pLight.specular * pLightSpec * vec3(texture(mat.specular, outUV));

    // Point Light Attenuation Calculations
    float pLightDistance = length(pLight.position-outPosition);
    float pLightAttenuation = 1.0f/(pLight.constant + pLight.linear * pLightDistance + pLight.quadratic * (pLightDistance * pLightDistance));

    pLightAmbient *= pLightAttenuation;
    pLightDiffuse *= pLightAttenuation;
    pLightSpecular *= pLightAttenuation;

    vec3 pLightAns = pLightAmbient + pLightDiffuse + pLightSpecular;
    //--------------------------------------------------------------------------------
    // SPOTLIGHT
    //Ambient lighting
	vec3 ambientSpot = spotLight.ambient * vec3(texture(mat.diffuse, outUV)); 

    // Diffuse
    vec3 normSpot = normalize(outNormal);
    vec3 lightDirectionSpot = normalize(spotLight.position - outPosition);
    float diffSpot = max(dot(normSpot, lightDirectionSpot), 0.0);
    vec3 diffuseSpot = spotLight.diffuse * diffSpot * vec3(texture(mat.diffuse, outUV));
    
    // Specular
    vec3 viewDirSpot = normalize(camPosition - outPosition);
    vec3 reflectDirectionSpot = reflect(-lightDirectionSpot, normSpot);
    float specSpot = pow(max(dot(viewDirSpot, reflectDirectionSpot), 0.0), mat.shininess);
    vec3 specularSpot = spotLight.specular * specSpot * vec3(texture(mat.specular, outUV));

    //Spotlight with soft edge
    float theta = dot(lightDirectionSpot, normalize(-spotLight.direction));
    float epsilon = (spotLight.cutOff - spotLight.outerCutOff);
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
    diffuseSpot = diffuseSpot * intensity;
    specularSpot = specularSpot * intensity;

    // Attenuation
    float distance = length(camPosition - outPosition);
    float attenuation = (1.0f)/(spotLight.constant + (spotLight.linear * distance) + spotLight.quadratic * (distance * distance));
    ambientSpot = ambientSpot * attenuation;
    diffuseSpot = diffuseSpot * attenuation;
    specularSpot = specularSpot * attenuation;

    vec3 spotLightAns = ambientSpot + diffuseSpot + specularSpot;
    //--------------------------------------------------------------------------------


    //fragColor = texture(tex, outUV) * vec4(dirLightAns + pLightAns + spotLightAns, 1.0f);
	fragColor = texture(tex, outUV) * vec4(dirLightAns, 1.0f);
	//fragColor = texture(tex, outUV) * vec4(spotLightAns, 1.0f);
	//fragColor = texture(tex, outUV) * vec4(pLightAns, 1.0f);

}

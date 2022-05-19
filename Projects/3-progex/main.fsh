#version 330

// Color of the fragment received from the vertex shader (interpolated by the rasterization stage)
in vec3 outColor;

// Normal
in vec3 outNormal;


// Vertex Position 
in vec3 outPosition;

in vec4 shadowOutPosition;

// Final color of the fragment that will be rendered on the screen
out vec4 fragColor;


// Struct for material
struct Material
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

//Struct for directional light
struct DirLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform sampler2D depthMap;
uniform mat4 view;
uniform vec3 camPosition;
uniform DirLight dLight;
uniform Material mat;

void main()
{

    //--------------------------------------------------------------------------------
	// DIRECTIONAL LIGHT
    //Ambient lighting
	vec3 ambient = dLight.ambient * mat.diffuse; 

    // Diffuse
    vec3 norm = normalize(outNormal);
    vec3 lightDirection = normalize(-1 * dLight.direction);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * mat.diffuse * dLight.diffuse ;
    
    // Specular
    vec3 viewDir = normalize(camPosition - outPosition);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDirection), 0.0), mat.shininess);
    vec3 specular = dLight.specular * spec * mat.specular;

    //--------------------------------------------------------------------------------
    //SHADOW CALCULATION 
    vec3 fragLightNDC = shadowOutPosition.xyz / shadowOutPosition.w;
    
    // transform to [0,1] range
    fragLightNDC = (fragLightNDC / 2.0f) + 0.5f;
    
    // nearest depth value from light POV
    float nearestDepth = texture(depthMap, fragLightNDC.xy).r; 

	float bias = max(0.05 * (1- dot(norm,lightDirection)), 0.005);
    
    // get depth of current fragment from light POV
    float fragCurrDepth = fragLightNDC.z - bias;
    float shadow = 1.0f;
    // if the depth of frag is higher than the depth of the closest then it is in the shadow
    if(fragCurrDepth > nearestDepth){
        shadow = 1.0f;
    }
    else{
        shadow = 0.0f;
        //phong = (ambient + diffuse + specular);
    }

	// Since it was a bonus task, we tried PCF to see changes, however the soft edges destroy the other two blocks
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(depthMap, fragLightNDC.xy + vec2(x, y) * texelSize).r;
            if(fragCurrDepth - 0.01f > pcfDepth)
            {
                shadow += 1.0f;
            }
			//shadow += fragCurrDepth - 0.01f > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

    //--------------------------------------------------------------------------------
    //FINAL OUTPUT
    //vec3 phong = (ambient + diffuse + specular);
    //vec3 phong =  (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    //vec3 dirLightAns = outColor * phong;
    vec3 dirLightAns = (ambient + ((1.0 - shadow) * (diffuse + specular))) ;
	fragColor = vec4(dirLightAns, 1.0f)* vec4(outColor,1.0f);
}
                                     
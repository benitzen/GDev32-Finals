#version 330

// Vertex position
layout(location = 0) in vec3 vertexPosition;

// Vertex color
layout(location = 1) in vec3 vertexColor;

// Vertex normal coordinate
layout(location = 2) in vec3 vertexNormal;



// Color (will be passed to the fragment shader)
out vec3 outColor;

// Normal (will be passed to the fragment shader)
out vec3 outNormal;

// Vertex Position (will be passed to the fragment shader)
out vec3 outPosition;

out vec4 shadowOutPosition;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 dirLightSpaceMatrix;

void main()
{
	// Convert our vertex position to homogeneous coordinates by introducing the w-component.
	// Vertex positions are ... positions, so we specify the w-coordinate as 1.0.
	vec4 semiFinalPosition = vec4(vertexPosition, 1.0f);

	// We multiply the vertex position with our transformation matrix, which will
	// ultimately transform our vertex based on the combination of transformations that
	// is contained inside the transformationMatrix uniform.
    semiFinalPosition = projection * view * model * semiFinalPosition;
    gl_Position = semiFinalPosition;

    outPosition = vec3(model * vec4(vertexPosition, 1.0f));
    outNormal = mat3(transpose(inverse(model))) * vertexNormal;
	outColor = vertexColor;
    shadowOutPosition = dirLightSpaceMatrix * vec4(outPosition, 1.0f);
}

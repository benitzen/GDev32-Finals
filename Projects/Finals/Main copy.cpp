// Quick note: GLAD needs to be included first before GLFW.
// Otherwise, GLAD will complain about gl.h being already included.
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// ---------------
// Function declarations
// ---------------

/// <summary>
/// Creates a shader program based on the provided file paths for the vertex and fragment shaders.
/// </summary>
/// <param name="vertexShaderFilePath">Vertex shader file path</param>
/// <param name="fragmentShaderFilePath">Fragment shader file path</param>
/// <returns>OpenGL handle to the created shader program</returns>
GLuint CreateShaderProgram(const std::string &vertexShaderFilePath, const std::string &fragmentShaderFilePath);

/// <summary>
/// Creates a shader based on the provided shader type and the path to the file containing the shader source.
/// </summary>
/// <param name="shaderType">Shader type</param>
/// <param name="shaderFilePath">Path to the file containing the shader source</param>
/// <returns>OpenGL handle to the created shader</returns>
GLuint CreateShaderFromFile(const GLuint &shaderType, const std::string &shaderFilePath);

/// <summary>
/// Creates a shader based on the provided shader type and the string containing the shader source.
/// </summary>
/// <param name="shaderType">Shader type</param>
/// <param name="shaderSource">Shader source string</param>
/// <returns>OpenGL handle to the created shader</returns>
GLuint CreateShaderFromSource(const GLuint &shaderType, const std::string &shaderSource);

/// <summary>
/// Function for handling the event when the size of the framebuffer changed.
/// </summary>
/// <param name="window">Reference to the window</param>
/// <param name="width">New width</param>
/// <param name="height">New height</param>
void FramebufferSizeChangedCallback(GLFWwindow *window, int width, int height);

/// <summary>
/// Struct containing data about a vertex
/// </summary>
struct Vertex
{
	GLfloat x, y, z;	// Position
	GLubyte r, g, b;	// Color
	GLfloat u, v;		// UV coordinates
	GLfloat nx, ny, nz; // Normal
};

GLfloat ambientStrength = 2.0f;
float fov = 80.0f;

/// <summary>
/// Main function.
/// </summary>
/// <returns>An integer indicating whether the program ended successfully or not.
/// A value of 0 indicates the program ended succesfully, while a non-zero value indicates
/// something wrong happened during execution.</returns>
int main()
{
	// Initialize GLFW
	int glfwInitStatus = glfwInit();
	if (glfwInitStatus == GLFW_FALSE)
	{
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return 1;
	}

	// Tell GLFW that we prefer to use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Tell GLFW that we prefer to use the modern OpenGL
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Tell GLFW to create a window
	float windowWidth = 800;
	float windowHeight = 800;
	GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "Final Project", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return 1;
	}

	// Tell GLFW to use the OpenGL context that was assigned to the window that we just created
	glfwMakeContextCurrent(window);

	// Register the callback function that handles when the framebuffer size has changed
	glfwSetFramebufferSizeCallback(window, FramebufferSizeChangedCallback);

	// Tell GLAD to load the OpenGL function pointers
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cerr << "Failed to initialize GLAD!" << std::endl;
		return 1;
	}

	
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	// VERTEX END
	// Create a vertex buffer object (VBO), and upload our vertices data to the VBO
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create a vertex array object that contains data on how to map vertex attributes
	// (e.g., position, color) to vertex shader properties.
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Vertex attribute 0 - Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, x));

	// Vertex attribute 1 - Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *)(offsetof(Vertex, r)));

	// Vertex attribute 2 - UV coordinate
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, u)));

	// Vertex attribute 2 - Normal
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, nx)));

	glBindVertexArray(0);

	//  Create a shader program
	GLuint program = CreateShaderProgram("main.vsh", "main.fsh");

	// Tell OpenGL the dimensions of the region where stuff will be drawn.
	// For now, tell OpenGL to use the whole screen
	glViewport(0, 0, windowWidth, windowHeight);

	// Create a variable that will contain the ID for our texture,
	// and use glGenTextures() to generate the texture itself
	GLuint tex;
	glGenTextures(1, &tex);

	// --- Load our image using stb_image ---

	// Im image-space (pixels), (0, 0) is the upper-left corner of the image
	// However, in u-v coordinates, (0, 0) is the lower-left corner of the image
	// This means that the image will appear upside-down when we use the image data as is
	// This function tells stbi to flip the image vertically so that it is not upside-down when we use it
	stbi_set_flip_vertically_on_load(true);

	// 'imageWidth' and imageHeight will contain the width and height of the loaded image respectively
	int imageWidth, imageHeight, numChannels;

	// Read the image data and store it in an unsigned char array
	unsigned char *imageData = stbi_load("RoomTexture2.png", &imageWidth, &imageHeight, &numChannels, 0);

	// Make sure that we actually loaded the image before uploading the data to the GPU
	if (imageData != nullptr)
	{
		// Our texture is 2D, so we bind our texture to the GL_TEXTURE_2D target
		glBindTexture(GL_TEXTURE_2D, tex);

		// Set the filtering methods for magnification and minification
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Set the wrapping method for the s-axis (x-axis) and t-axis (y-axis)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Upload the image data to GPU memory
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

		// If we set minification to use mipmaps, we can tell OpenGL to generate the mipmaps for us
		// glGenerateMipmap(GL_TEXTURE_2D);

		// Once we have copied the data over to the GPU, we can delete
		// the data on the CPU side, since we won't be using it anymore
		stbi_image_free(imageData);
		imageData = nullptr;
	}
	else
	{
		std::cerr << "Failed to load image" << std::endl;
	}

	GLuint tex1;
	glGenTextures(1, &tex1);

	// Read the image data for a second texture, and store it in our unsigned char array
	// We can reuse the "imageData" array since we already uploaded the previous image data
	// to GPU memory. The same applies for imageWidth, imageHeight, and numChannels
	imageData = stbi_load("stone.jpg", &imageWidth, &imageHeight, &numChannels, 0);
	// Make sure that we actually loaded the image before uploading the data to the GPU
	if (imageData != nullptr)
	{
		// Our texture is 2D, so we bind our texture to the GL_TEXTURE_2D target
		glBindTexture(GL_TEXTURE_2D, tex1);

		// Set the filtering methods for magnification and minification
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Set the wrapping method for the s-axis (x-axis) and t-axis (y-axis)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Upload the image data to GPU memory
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

		// If we set minification to use mipmaps, we can tell OpenGL to generate the mipmaps for us
		// glGenerateMipmap(GL_TEXTURE_2D);

		// Once we have copied the data over to the GPU, we can delete
		// the data on the CPU side, since we won't be using it anymore
		stbi_image_free(imageData);
		imageData = nullptr;
	}
	else
	{
		std::cerr << "Failed to load image" << std::endl;
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	float cameraMoveForwardBackward = 1.0f;
	float cameraMoveLeftRight = 0.0f;
	float cameraLookUpDown = 0.0f;
	float cameraLookLeftRight = 0.0f;
	float cameraLookForwardBackward = 0.0f;
	float horizAngle = 3.14f;
	float vertAngle = 0.0f;
	float mouseSpeed = 0.05f;
	float speed = 2.0f;
	float deltaTime = 0.0f;
	double lastTime = 0;
	glm::vec3 position = glm::vec3(
		0.0f, 0.0f, 0.0f);
	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// Hide Cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Clear the color and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Get time for rotation
		time = glfwGetTime();
		deltaTime = float(time - lastTime);

		// Use the shader program that we created
		glUseProgram(program);

		// Use the vertex array object that we created
		glBindVertexArray(vao);

		// Bind our texture to texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);

		// Bind our second texture to texture unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex1);

		// Make our sampler in the fragment shader use texture unit 0
		GLint texUniformLocation = glGetUniformLocation(program, "tex");
		glUniform1i(texUniformLocation, 0);

		// Mouse initialization
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
		horizAngle += mouseSpeed * 1.25 * float(windowWidth / 2 - mouseX);
		vertAngle += mouseSpeed * 1.25 * float(windowHeight / 2 - mouseY);

		// Instantiating position and direction vectors
		glm::vec3 direction = glm::vec3(
			cos(glm::radians(vertAngle)) * sin(glm::radians(horizAngle)),
			sin(glm::radians(vertAngle)),
			cos(glm::radians(vertAngle)) * cos(glm::radians(horizAngle)));

		glm::vec3 up = glm::vec3(
			0.0f, -1.0f, 0.0f);

		glm::vec3 right = glm::vec3(
			sin(horizAngle - 3.14f / 2.0f),
			0,
			cos(horizAngle - 3.14f / 2.0f));

		// View Matrix and Perspective Projection Matrix
		glm::mat4 viewMatrix = glm::mat4(1.0f);

		viewMatrix = glm::lookAt(
			position,
			position + direction,
			up);

		float aspectRatio = windowWidth / windowHeight;
		glm::mat4 perspectiveProjMatrix = glm::perspective(fov, aspectRatio, 0.1f, 100.0f);

		// View and Projection Uniform Init
		GLint viewMatrixUniformLocation = glGetUniformLocation(program, "view");
		glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		GLint projectionMatrixUniformLocation = glGetUniformLocation(program, "projection");
		glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(perspectiveProjMatrix));

		// Room Cube
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
		// modelMatrix = glm::rotate(modelMatrix, glm::radians(time), glm::vec3(0.0f, .0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(12.0f, 6.0f, 12.0f));

		glm::mat4 finalMatrix = perspectiveProjMatrix * viewMatrix * modelMatrix;

		// Transformation Matrix Init
		GLint transformationMatrixUniformLocation = glGetUniformLocation(program, "transformationMatrix");
		glUniformMatrix4fv(transformationMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(finalMatrix));

		// Model Matrix init
		GLint modelMatrixUniformLocation = glGetUniformLocation(program, "model");
		glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		/*
		glDrawArrays(GL_TRIANGLES, 78, 6);
		glDrawArrays(GL_TRIANGLES, 84, 6);
		glDrawArrays(GL_TRIANGLES, 90, 6);
		glDrawArrays(GL_TRIANGLES, 96, 6);
		glDrawArrays(GL_TRIANGLES, 102, 6);
		glDrawArrays(GL_TRIANGLES, 108, 6);
		*/
		// change texture to next
		glUniform1i(texUniformLocation, 1);

		// Table Cube
		glm::mat4 secondCube = glm::mat4(1.0f);
		secondCube = glm::translate(secondCube, glm::vec3(0.0f, -1.5f, -1.0f));
		secondCube = glm::scale(secondCube, glm::vec3(1.75f, 0.75f, 1.0f));

		finalMatrix = perspectiveProjMatrix * viewMatrix * secondCube;
		glUniformMatrix4fv(transformationMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(finalMatrix));
		glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(secondCube));

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawArrays(GL_TRIANGLES, 6, 6);
		glDrawArrays(GL_TRIANGLES, 12, 6);
		glDrawArrays(GL_TRIANGLES, 18, 6);
		glDrawArrays(GL_TRIANGLES, 24, 6);
		glDrawArrays(GL_TRIANGLES, 30, 6);

		// Bottom Cube
		glm::mat4 thirdCube = glm::mat4(1.0f);
		thirdCube = glm::translate(thirdCube, glm::vec3(0.0f, -1.75f, 1.0f));
		thirdCube = glm::scale(thirdCube, glm::vec3(0.5f, 0.5f, 0.5f));
		thirdCube = glm::rotate(thirdCube, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		finalMatrix = perspectiveProjMatrix * viewMatrix * thirdCube;
		glUniformMatrix4fv(transformationMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(finalMatrix));
		glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(thirdCube));

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawArrays(GL_TRIANGLES, 6, 6);
		glDrawArrays(GL_TRIANGLES, 12, 6);
		glDrawArrays(GL_TRIANGLES, 18, 6);
		glDrawArrays(GL_TRIANGLES, 24, 6);
		glDrawArrays(GL_TRIANGLES, 30, 6);

		// Top Cube
		glm::mat4 fourthCube = glm::mat4(1.0f);
		fourthCube = glm::translate(fourthCube, glm::vec3(0.0f, -1.25f, 1.0f));
		fourthCube = glm::scale(fourthCube, glm::vec3(0.5f, 0.5f, 0.5f));

		finalMatrix = perspectiveProjMatrix * viewMatrix * fourthCube;
		glUniformMatrix4fv(transformationMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(finalMatrix));
		glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(fourthCube));

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawArrays(GL_TRIANGLES, 6, 6);
		glDrawArrays(GL_TRIANGLES, 12, 6);
		glDrawArrays(GL_TRIANGLES, 18, 6);
		glDrawArrays(GL_TRIANGLES, 24, 6);
		glDrawArrays(GL_TRIANGLES, 30, 6);

		// Octahedron
		glm::mat4 octahedron = glm::mat4(1.0f);
		octahedron = glm::translate(octahedron, glm::vec3(0.0f, 1.0f, 1.0f));
		octahedron = glm::scale(octahedron, glm::vec3(0.75f, 0.75f, 0.75f));
		octahedron = glm::rotate(octahedron, glm::radians(time * 60), glm::vec3(1.0f, 1.0f, 0.0f));
		octahedron = glm::translate(octahedron, glm::vec3(float(cos(time)), float(sin(time)), 0.0f));

		finalMatrix = perspectiveProjMatrix * viewMatrix * octahedron;
		glUniformMatrix4fv(transformationMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(finalMatrix));
		glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(octahedron));

		glDrawArrays(GL_TRIANGLES, 36, 3);
		glDrawArrays(GL_TRIANGLES, 39, 3);
		glDrawArrays(GL_TRIANGLES, 42, 3);
		glDrawArrays(GL_TRIANGLES, 45, 3);
		glDrawArrays(GL_TRIANGLES, 48, 3);
		glDrawArrays(GL_TRIANGLES, 51, 3);
		glDrawArrays(GL_TRIANGLES, 54, 3);
		glDrawArrays(GL_TRIANGLES, 57, 3);

		// Floating Pyramid
		glm::mat4 pyramid = glm::mat4(1.0f);
		pyramid = glm::translate(pyramid, glm::vec3(0.0f, -1.0f, -1.0f));
		pyramid = glm::scale(pyramid, glm::vec3(0.25f, 0.25f, 0.25f));
		pyramid = glm::rotate(pyramid, glm::radians(time * 160), glm::vec3(0.0f, 1.0f, 0.0f));
		pyramid = glm::translate(pyramid, glm::vec3(0.0f, float(sin(time)) + 1.0f, 0.0f));

		finalMatrix = perspectiveProjMatrix * viewMatrix * pyramid;
		glUniformMatrix4fv(transformationMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(finalMatrix));
		glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(pyramid));

		glDrawArrays(GL_TRIANGLES, 60, 3);
		glDrawArrays(GL_TRIANGLES, 63, 3);
		glDrawArrays(GL_TRIANGLES, 66, 3);
		glDrawArrays(GL_TRIANGLES, 69, 3);
		glDrawArrays(GL_TRIANGLES, 72, 3);
		glDrawArrays(GL_TRIANGLES, 75, 3);

		// DIRECTIONAL LIGHT
		GLint camPositionUniformLocation = glGetUniformLocation(program, "camPosition");
		glUniform3fv(camPositionUniformLocation, 1, glm::value_ptr(position));

		/*
		Light directionalLight = {};
		directionalLight.lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		directionalLight.lightDirection = glm::vec3(-1.0f, 6.0f, 2.3f);
		directionalLight.diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);
		directionalLight.ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
		directionalLight.specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
		directionalLight.ambientMaterial = glm::vec3(1.0f, 0.5f, 0.2f);
		directionalLight.diffuseMaterial = glm::vec3(0.0f, 0.0f, 0.0f);
		directionalLight.specularMaterial = glm::vec3(0.5f, 0.5f, 0.5f);
		*/

		// GLint dirLightUniformLocation = glGetUniformLocation(program, "dirLight.position");
		// glUniform3fv(dirLightUniformLocation, 1, glm::value_ptr(dirLight.lightPosition));

		GLint dirLightUniformDirection = glGetUniformLocation(program, "dLight.direction");
		// glUniform3fv(dirLightUniformDirection, 1, glm::value_ptr(dirLight.lightDirection));
		glUniform3f(dirLightUniformDirection, -1.2f, -1.0f, -2.3f);

		GLint dirLightUniformAmbient = glGetUniformLocation(program, "dLight.ambient");
		// glUniform3fv(dirLightUniformAmbient, 1, glm::value_ptr(dirLight.ambientColor));
		glUniform3f(dirLightUniformAmbient, 0.2f, 0.2f, 0.2f);

		GLint dirLightUniformDiffuse = glGetUniformLocation(program, "dLight.diffuse");
		// glUniform3fv(dirLightUniformDiffuse, 1, glm::value_ptr(dirLight.diffuseColor));
		glUniform3f(dirLightUniformDiffuse, 0.5f, 0.5f, 0.5f);

		GLint dirLightUniformSpecular = glGetUniformLocation(program, "dLight.specular");
		// glUniform3fv(dirLightUniformSpecular, 1, glm::value_ptr(dirLight.specularColor));
		glUniform3fv(dirLightUniformSpecular, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));

		GLint materialUniformDiffuse = glGetUniformLocation(program, "mat.diffuse");
		// glUniform3fv(materialUniformDiffuse, 1, glm::value_ptr(dirLight.diffuseMaterial));
		glUniform3fv(materialUniformDiffuse, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));

		GLint materialUniformSpecular = glGetUniformLocation(program, "mat.specular");
		// glUniform3fv(materialUniformSpecular, 1, glm::value_ptr(dirLight.specularMaterial));
		glUniform3fv(materialUniformSpecular, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

		GLint materialUniformShiny = glGetUniformLocation(program, "mat.shininess");
		glUniform1f(materialUniformShiny, 16.0f);

		// POINT LIGHT
		glUniform1i(texUniformLocation, 0);
		glm::mat4 pointLight = glm::mat4(1.0f);
		pointLight = glm::translate(pointLight, glm::vec3(0.0f, -2.0f, 0.0f));
		pointLight = glm::scale(pointLight, glm::vec3(24.0f, 0.01f, 24.0f));

		finalMatrix = perspectiveProjMatrix * viewMatrix * pointLight;
		glUniformMatrix4fv(transformationMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(finalMatrix));
		glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(pointLight));
		/*
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawArrays(GL_TRIANGLES, 6, 6);
		glDrawArrays(GL_TRIANGLES, 12, 6);
		glDrawArrays(GL_TRIANGLES, 18, 6);
		glDrawArrays(GL_TRIANGLES, 24, 6);
		*/
		glDrawArrays(GL_TRIANGLES, 30, 6);

		GLint lightPosUniformLocation = glGetUniformLocation(program, "pLight.position");
		glUniform3f(lightPosUniformLocation, 0.0f, 0.15f, -0.5f);

		GLint pLightUniformAmbient = glGetUniformLocation(program, "pLight.ambient");
		glUniform3f(pLightUniformAmbient, 0.2f, 0.2f, 0.2f);

		GLint pLightUniformDiffuse = glGetUniformLocation(program, "pLight.diffuse");
		glUniform3f(pLightUniformDiffuse, sin(lastTime * 0.3f) + 0.2f, sin(lastTime * 0.2f) + 0.2f, cos(lastTime * 0.6f));

		GLint pLightUniformSpecular = glGetUniformLocation(program, "pLight.specular");
		glUniform3fv(dirLightUniformSpecular, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));

		GLint pointLightConstant = glGetUniformLocation(program, "pLight.constant");
		glUniform1f(pointLightConstant, 1.0f);

		GLint pointLightLinear = glGetUniformLocation(program, "pLight.linear");
		glUniform1f(pointLightLinear, 0.7f);

		GLint pointLightQuadratic = glGetUniformLocation(program, "pLight.quadratic");
		glUniform1f(pointLightQuadratic, 1.8f);

		// SPOTLIGHT
		GLint spotLightUniformLocation = glGetUniformLocation(program, "spotLight.position");
		glUniform3fv(spotLightUniformLocation, 1, glm::value_ptr(position));

		GLint spotLightUniformDirection = glGetUniformLocation(program, "spotLight.direction");
		glUniform3fv(spotLightUniformDirection, 1, glm::value_ptr(direction));

		GLint spotLightUniformAmbient = glGetUniformLocation(program, "spotLight.ambient");
		glUniform3f(spotLightUniformAmbient, 0.2f, 0.2f, 0.2f);

		GLint spotLightUniformDiffuse = glGetUniformLocation(program, "spotLight.diffuse");
		glUniform3f(spotLightUniformDiffuse, 0.5f, 0.5f, 0.5f);

		GLint spotLightUniformSpecular = glGetUniformLocation(program, "spotLight.specular");
		glUniform3fv(spotLightUniformSpecular, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));

		GLint spotLightUniformCutoff = glGetUniformLocation(program, "spotLight.cutOff");
		glUniform1f(spotLightUniformCutoff, glm::cos(glm::radians(8.0f)));

		GLint spotLightUniformOuterCutoff = glGetUniformLocation(program, "spotLight.outerCutOff");
		glUniform1f(spotLightUniformOuterCutoff, glm::cos(glm::radians(12.0f)));

		GLint spotLightUniformConstant = glGetUniformLocation(program, "spotLight.constant");
		glUniform1f(spotLightUniformConstant, 1.0f);

		GLint spotLightUniformLinear = glGetUniformLocation(program, "spotLight.linear");
		glUniform1f(spotLightUniformLinear, 0.5f);

		GLint spotLightUniformQuadratic = glGetUniformLocation(program, "spotLight.quadratic");
		glUniform1f(spotLightUniformQuadratic, 0.32f);

		// "Unuse" the vertex array object
		glBindVertexArray(0);

		// Tell GLFW to swap the screen buffer with the offscreen buffer
		glfwSwapBuffers(window);

		// Tell GLFW to process window events (e.g., input events, window closed events, etc.)
		glfwPollEvents();

		// CAMERA STUFF

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			position -= right * speed * deltaTime;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			position += right * speed * deltaTime;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			position += direction * speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			position -= direction * speed * deltaTime;
		}

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			// Make sure to delete the shader program
			glDeleteProgram(program);

			// Delete the VBO that contains our vertices
			glDeleteBuffers(1, &vbo);

			// Delete the vertex array object
			glDeleteVertexArrays(1, &vao);

			// Remember to tell GLFW to clean itself up before exiting the application
			glfwTerminate();
		}

		lastTime = time;
		glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(perspectiveProjMatrix));
	}

	// --- Cleanup ---

	// Make sure to delete the shader program
	glDeleteProgram(program);

	// Delete the VBO that contains our vertices
	glDeleteBuffers(1, &vbo);

	// Delete the vertex array object
	glDeleteVertexArrays(1, &vao);

	// Remember to tell GLFW to clean itself up before exiting the application
	glfwTerminate();

	return 0;
}

/// <summary>
/// Creates a shader program based on the provided file paths for the vertex and fragment shaders.
/// </summary>
/// <param name="vertexShaderFilePath">Vertex shader file path</param>
/// <param name="fragmentShaderFilePath">Fragment shader file path</param>
/// <returns>OpenGL handle to the created shader program</returns>
GLuint CreateShaderProgram(const std::string &vertexShaderFilePath, const std::string &fragmentShaderFilePath)
{
	GLuint vertexShader = CreateShaderFromFile(GL_VERTEX_SHADER, vertexShaderFilePath);
	GLuint fragmentShader = CreateShaderFromFile(GL_FRAGMENT_SHADER, fragmentShaderFilePath);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	glDetachShader(program, vertexShader);
	glDeleteShader(vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(fragmentShader);

	// Check shader program link status
	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE)
	{
		char infoLog[512];
		GLsizei infoLogLen = sizeof(infoLog);
		glGetProgramInfoLog(program, infoLogLen, &infoLogLen, infoLog);
		std::cerr << "program link error: " << infoLog << std::endl;
	}

	return program;
}

/// <summary>
/// Creates a shader based on the provided shader type and the path to the file containing the shader source.
/// </summary>
/// <param name="shaderType">Shader type</param>
/// <param name="shaderFilePath">Path to the file containing the shader source</param>
/// <returns>OpenGL handle to the created shader</returns>
GLuint CreateShaderFromFile(const GLuint &shaderType, const std::string &shaderFilePath)
{
	std::ifstream shaderFile(shaderFilePath);
	if (shaderFile.fail())
	{
		std::cerr << "Unable to open shader file: " << shaderFilePath << std::endl;
		return 0;
	}

	std::string shaderSource;
	std::string temp;
	while (std::getline(shaderFile, temp))
	{
		shaderSource += temp + "\n";
	}
	shaderFile.close();

	return CreateShaderFromSource(shaderType, shaderSource);
}

/// <summary>
/// Creates a shader based on the provided shader type and the string containing the shader source.
/// </summary>
/// <param name="shaderType">Shader type</param>
/// <param name="shaderSource">Shader source string</param>
/// <returns>OpenGL handle to the created shader</returns>
GLuint CreateShaderFromSource(const GLuint &shaderType, const std::string &shaderSource)
{
	GLuint shader = glCreateShader(shaderType);

	const char *shaderSourceCStr = shaderSource.c_str();
	GLint shaderSourceLen = static_cast<GLint>(shaderSource.length());
	glShaderSource(shader, 1, &shaderSourceCStr, &shaderSourceLen);
	glCompileShader(shader);

	// Check compilation status
	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		char infoLog[512];
		GLsizei infoLogLen = sizeof(infoLog);
		glGetShaderInfoLog(shader, infoLogLen, &infoLogLen, infoLog);
		std::cerr << "shader compilation error: " << infoLog << std::endl;
	}

	return shader;
}

/// <summary>
/// Function for handling the event when the size of the framebuffer changed.
/// </summary>
/// <param name="window">Reference to the window</param>
/// <param name="width">New width</param>
/// <param name="height">New height</param>
void FramebufferSizeChangedCallback(GLFWwindow *window, int width, int height)
{
	// Whenever the size of the framebuffer changed (due to window resizing, etc.),
	// update the dimensions of the region to the new size
	glViewport(0, 0, width, height);
}

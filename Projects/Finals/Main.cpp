// Quick note: GLAD needs to be included first before GLFW.
// Otherwise, GLAD will complain about gl.h being already included.
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ctime>
#include <cstdlib>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
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

float fov = 80.0f;


// Updated Camera with Mouse Callback
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraPosition = glm::vec3(0.0f, 1.5f, -3.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


float lastX = 400.0, lastY = 400.0;
float pitch = 0.0f;
float yaw = 90.0f;
bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 cameraDirection;
	cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraDirection.y = sin(glm::radians(pitch));
	cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(cameraDirection);
}
/// <summary>
/// Struct containing data about a vertex
/// </summary>
struct Vertex
{
	GLfloat x, y, z;	// Position
	GLubyte r, g, b;	// Color
	GLfloat nx, ny, nz; // Normal
};

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

	Vertex vertices[42];

	// Front
	vertices[0] = {-0.5f, -0.5f, 0.5f, 255, 51, 255,  -1.0f, -1.0f, 1.0f}; // Lower-left
	vertices[1] = {0.5f, -0.5f, 0.5f, 255, 51, 255, 1.0f, -1.0f, 1.0f};   // Lower-right
	vertices[2] = {0.5f, 0.5f, 0.5f, 255, 51, 255, 1.0f, 1.0f, 1.0f};	   // Upper-right

	vertices[3] = {0.5f, 0.5f, 0.5f, 255, 51, 255, 1.0f, 1.0f, 1.0f};	   // Upper-right
	vertices[4] = {-0.5f, 0.5f, 0.5f, 255, 51, 255, -1.0f, 1.0f, 1.0f};  // Upper-left
	vertices[5] = {-0.5f, -0.5f, 0.5f, 255, 51, 255, -1.0f, -1.0f, 1.0f}; // Lower-left

	// Back
	vertices[6] = {-0.5f, -0.5f, -0.5f, 255, 51, 255, -1.0f, -1.0f, -1.0f}; // Lower-left
	vertices[7] = {0.5f, -0.5f, -0.5f, 255, 51, 255, 1.0f, -1.0f, -1.0f}; // Lower-right
	vertices[8] = {0.5f, 0.5f, -0.5f, 255, 51, 255, 1.0f, 1.0f, -1.0f};	// Upper-right

	vertices[9] = {0.5f, 0.5f, -0.5f, 255, 51, 255, 1.0f, 1.0f, -1.0f};	 // Upper-right
	vertices[10] = {-0.5f, 0.5f, -0.5f, 255, 51, 255, -1.0f, 1.0f, -1.0f};	 // Upper-left
	vertices[11] = {-0.5f, -0.5f, -0.5f, 255, 51, 255, -1.0f, -1.0f, -1.0f}; // Lower-left

	// Left Face
	vertices[12] = {-0.5f, -0.5f, -0.5f, 255, 51, 255, -1.0f, -1.0f, -1.0f}; // Lower-left back
	vertices[13] = {-0.5f, -0.5f, 0.5f, 255, 51, 255, -1.0f, -1.0f, 1.0f};	 // Lower-left front
	vertices[14] = {-0.5f, 0.5f, 0.5f, 255, 51, 255, -1.0f, 1.0f, 1.0f};	 // Upper-left front

	vertices[15] = {-0.5f, 0.5f, 0.5f, 255, 51, 255, -1.0f, 1.0f, 1.0f};	 // Upper-left front
	vertices[16] = {-0.5f, 0.5f, -0.5f, 255, 51, 255, -1.0f, 1.0f, -1.0f};	 // Upper-left back
	vertices[17] = {-0.5f, -0.5f, -0.5f, 255, 51, 255, -1.0f, -1.0f, -1.0f}; // Lower-left back

	// Right Face
	vertices[18] = {0.5f, -0.5f, 0.5f, 255, 51, 255, 1.0f, -1.0f, 1.0f};	 // Lower-right Front
	vertices[19] = {0.5f, -0.5f, -0.5f, 255, 51, 255, 1.0f, -1.0f, -1.0f}; // Lower-right Back
	vertices[20] = {0.5f, 0.5f, -0.5f, 255, 51, 255, 1.0f, 1.0f, -1.0f};	 // Upper-right Back

	vertices[21] = {0.5f, 0.5f, -0.5f, 255, 51, 255, 1.0f, 1.0f, -1.0f}; // Upper-right Back
	vertices[22] = {0.5f, 0.5f, 0.5f, 255, 51, 255, 1.0f, 1.0f, 1.0f};	// Upper-right Front
	vertices[23] = {0.5f, -0.5f, 0.5f, 255, 51, 255, 1.0f, -1.0f, 1.0f};	// Lower-right Front

	// Top
	vertices[24] = {-0.5f, 0.5f, 0.5f, 255, 51, 255, -1.0f, 1.0f, 1.0f}; // Upper-left Front
	vertices[25] = {0.5f, 0.5f, 0.5f, 255, 51, 255, 1.0f, 1.0f, 1.0f};   // Upper-right Front
	vertices[26] = {0.5f, 0.5f, -0.5f, 255, 51, 255, 1.0f, 1.0f, -1.0f};  // Upper-right Back

	vertices[27] = {0.5f, 0.5f, -0.5f, 255, 51, 255, 1.0f, 1.0f, -1.0f};	// Upper-right Back
	vertices[28] = {-0.5f, 0.5f, -0.5f, 255, 51, 255, -1.0f, 1.0f, -1.0f}; // Upper-left Back
	vertices[29] = {-0.5f, 0.5f, 0.5f, 255, 51, 255, -1.0f, 1.0f, 1.0f};	// Upper-left Front

	// Bottom
	vertices[30] = {-0.5f, -0.5f, -0.5f, 255, 51, 255, -1.0f, -1.0f, -1.0f}; // Lower-left Back
	vertices[31] = {0.5f, -0.5f, -0.5f, 255, 51, 255, 1.0f, -1.0f, -1.0f};	 // Lower-right Back
	vertices[32] = {0.5f, -0.5f, 0.5f, 255, 51, 255, 1.0f, -1.0f, 1.0f};	 // Lower-right Front

	vertices[33] = {0.5f, -0.5f, 0.5f, 255, 51, 255, 1.0f, -1.0f, 1.0f};	 // Lower-right Front
	vertices[34] = {-0.5f, -0.5f, 0.5f, 255, 51, 255, -1.0f, -1.0f, 1.0f};	 // Lower-left Front
	vertices[35] = {-0.5f, -0.5f, -0.5f, 255, 51, 255, -1.0f, -1.0f, -1.0f}; // Lower-left Back

	// Top
	vertices[36] = {-0.5f, 0.0f, 0.5f, 213, 184, 225, -1.0f, 1.0f, 1.0f}; // Upper-left Front
	vertices[37] = {0.5f, 0.0f, 0.5f, 213, 184, 225, 1.0f, 1.0f, 1.0f};   // Upper-right Front
	vertices[38] = {0.5f, 0.0f, -0.5f, 213, 184, 225, 1.0f, 1.0f, -1.0f};  // Upper-right Back

	vertices[39] = {0.5f, 0.0f, -0.5f, 213, 184, 225, 1.0f, 1.0f, -1.0f};	// Upper-right Back
	vertices[40] = {-0.5f, 0.0f, -0.5f, 213, 184, 225, -1.0f, 1.0f, -1.0f}; // Upper-left Back
	vertices[41] = {-0.5f, 0.0f, 0.5f, 213, 184, 225, -1.0f, 1.0f, 1.0f};	// Upper-left Front
	
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

	// Vertex attribute 2 - Normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, nx)));

	glBindVertexArray(0);

	//  Create a shader program
	GLuint program = CreateShaderProgram("main.vsh", "main.fsh");

	//For depth map shader
	GLuint depthMapVAO;
	glGenVertexArrays(1, &depthMapVAO);
	glBindVertexArray(depthMapVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glBindVertexArray(0);
	GLuint depthMapShader = CreateShaderProgram("depthmap.vsh", "depthmap.fsh");


	// Tell OpenGL the dimensions of the region where stuff will be drawn.
	// For now, tell OpenGL to use the whole screen
	glViewport(0, 0, windowWidth, windowHeight);
	
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	float time = 0;
	float deltaTime = 0.0f;
	double lastTime = 0;

	// NUMBER 7
	// Generate Framebuffer Object
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	// NUMBER 7-A
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	// NUMBER 7-B
	GLuint depthMapTex;
	glGenTextures(1, &depthMapTex);
	glBindTexture(GL_TEXTURE_2D, depthMapTex); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	

	// NUMBER 7-C
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTex, 0);

	// NUMBER 7-D
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Error! Framebuffer not complete!" << std::endl;
	}

	glfwSetCursorPosCallback(window, mouse_callback);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// Hide Cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		// CAMERA STUFF
		const float cameraSpeed = 2.5 * deltaTime;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			cameraPosition += cameraSpeed * cameraFront;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			cameraPosition -= cameraSpeed * cameraFront;
		}

		// Camera Stuff
		float aspectRatio = windowWidth / windowHeight;
		glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
		glm::mat4 perspectiveProjMatrix = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);

		// --------------------------------------------------------------
		// --------------------------------------------------------------
		// --------------------------------------------------------------
		// --------------------------------------------------------------
		// --------------------------------------------------------------
		// --------------------------------------------------------------
		//FIRST PASS
		
		// Use the shader program that we created
		glUseProgram(depthMapShader);

		// Use the vertex array object that we created
		glBindVertexArray(depthMapVAO);


		//NUMBER 10
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

		// Get time for rotation
		time = glfwGetTime();
		deltaTime = float(time - lastTime);


		//NUMBER 9
		glm::mat4 dirLightProjMatrix = glm::ortho(-20.0f, 20.0f, -10.0f, 10.0f, -20.0f, 20.0f);
		glm::mat4 dirLightViewMatrix = glm::lookAt(glm::vec3(-0.1f, 3.0f, -0.1f), glm::vec3(0.6f, 0.0f, 0.6f),  glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 dirLightSpaceMatrix = dirLightProjMatrix * dirLightViewMatrix;
		
		//init for dirlight proj and view matrices
		GLint dirLightProjMatrixUniformLocation = glGetUniformLocation(depthMapShader, "dirLightProjMatrix");
		glUniformMatrix4fv(dirLightProjMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(dirLightProjMatrix));
		GLint dirLightViewMatrixUniformLocation = glGetUniformLocation(depthMapShader, "dirLightViewMatrix");
		glUniformMatrix4fv(dirLightViewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(dirLightViewMatrix));
		
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT); 
		
		

		// Making all cubes
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::mat4 firstCube = glm::mat4(1.0f);
		glm::mat4 secondCube = glm::mat4(1.0f);
		glm::mat4 thirdCube = glm::mat4(1.0f);

		firstCube = glm::translate(firstCube, glm::vec3(0.0f, 1.5f, 0.0f));
		firstCube = glm::rotate(firstCube, glm::radians(25.0f), glm::vec3(0.f, 1.0f, 0.0f));
		secondCube = glm::translate(secondCube, glm::vec3(-3.0f, 2.0f, -1.0f));
		secondCube = glm::rotate(secondCube, glm::radians(25.0f), glm::vec3(0.f, 1.0f, 0.0f));
		secondCube = glm::scale(secondCube, glm::vec3(1.75f, 1.75f, 1.75f));
		thirdCube = glm::translate(thirdCube, glm::vec3(2.5f, 1.0f, -3.0f));
		thirdCube = glm::scale(thirdCube, glm::vec3(0.5f, 0.5f, 0.5f));
		
		// Model Matrix init FOR DEPTH MAP SHADER
		GLint depthMapModelMatrixUniformLocation = glGetUniformLocation(depthMapShader, "model");

		//NUMBER 2-A
		glUniformMatrix4fv(depthMapModelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(firstCube));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// NUMBER 4
		glUniformMatrix4fv(depthMapModelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(secondCube));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glUniformMatrix4fv(depthMapModelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(thirdCube));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindFramebuffer(GL_FRAMEBUFFER, 0); 
		

		// --------------------------------------------------------------
		// --------------------------------------------------------------
		// --------------------------------------------------------------
		// --------------------------------------------------------------
		// --------------------------------------------------------------
		// --------------------------------------------------------------
		// SECOND PASS
		glViewport(0, 0, windowWidth, windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Use the shader program that we created
		glUseProgram(program);
		// Use the vertex array object that we created
		glBindVertexArray(vao);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTex);
		GLint depthMapTexUniformLocation = glGetUniformLocation(depthMapTex, "depthMap");
		glUniform1i(depthMapTexUniformLocation, 0);
		
		//for main shaders
		GLint mainDirLightSpaceMatrixUniformLocation = glGetUniformLocation(program, "dirLightSpaceMatrix");
		GLint modelMatrixUniformLocation = glGetUniformLocation(program, "model");
		GLint viewMatrixUniformLocation = glGetUniformLocation(program, "view");
		GLint projectionMatrixUniformLocation = glGetUniformLocation(program, "projection");

		// Model Matrix init FOR NORMAL SHADER
		// View and Projection Uniform Init
		glUniformMatrix4fv(mainDirLightSpaceMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(dirLightSpaceMatrix));
		glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(perspectiveProjMatrix));
		


		//NUMBER 2-A
		glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(firstCube));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// NUMBER 4
		glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(secondCube));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(thirdCube));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// // NUMBER 2-B
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glDrawArrays(GL_TRIANGLES, 36, 6);


		// DIRECTIONAL LIGHT
		GLint camPositionUniformLocation = glGetUniformLocation(program, "camPosition");
		glUniform3fv(camPositionUniformLocation, 1, glm::value_ptr(cameraPosition));

		GLint dirLightUniformDirection = glGetUniformLocation(program, "dLight.direction");
		glUniform3fv(dirLightUniformDirection, 1, glm::value_ptr(glm::vec3(0.6f, -1.0f, 0.6f)));

		GLint dirLightUniformAmbient = glGetUniformLocation(program, "dLight.ambient");
		glUniform3fv(dirLightUniformAmbient, 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));

		GLint dirLightUniformDiffuse = glGetUniformLocation(program, "dLight.diffuse");
		// glUniform3fv(dirLightUniformDiffuse, 1, glm::value_ptr(dirLight.diffuseColor));
		glUniform3fv(dirLightUniformDiffuse, 1, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));

		GLint dirLightUniformSpecular = glGetUniformLocation(program, "dLight.specular");
		// glUniform3fv(dirLightUniformSpecular, 1, glm::value_ptr(dirLight.specularColor));
		glUniform3fv(dirLightUniformSpecular, 1, glm::value_ptr(glm::vec3(0.3f, 0.3f, 0.3f)));

		GLint materialUniformDiffuse = glGetUniformLocation(program, "mat.diffuse");
		glUniform3fv(materialUniformDiffuse, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

		GLint materialUniformSpecular = glGetUniformLocation(program, "mat.specular");
		glUniform3fv(materialUniformSpecular, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

		GLint materialUniformShiny = glGetUniformLocation(program, "mat.shininess");
		glUniform1f(materialUniformShiny, 16.0f);


		// "Unuse" the vertex array object
		glBindVertexArray(0);

		// Tell GLFW to swap the screen buffer with the offscreen buffer
		glfwSwapBuffers(window);

		// Tell GLFW to process window events (e.g., input events, window closed events, etc.)
		glfwPollEvents();

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

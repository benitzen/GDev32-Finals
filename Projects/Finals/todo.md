#ProgEx 3

- [x] Create a brand new OpenGL project (Main.cpp, main pair of vertex and fragment shaders). Set up the Vertex struct to contain a position (x, y, z), a color (r, g, b), and a normal (nx, ny, nz). Note that we are not applying any textures to our geometry, hence the absence of the uv-coordinates for the Vertex.
- [x] In a single array/vector, set up the vertices of the following.
    - [x] A cube whose center is at (0, 0, 0), with each edge having a length of 1.
    - [x] A plane whose center is at (0, 0, 0), with each edge having a length of 1. The plane should be facing upwards.
- [x] Generate the VBO that will contain the vertices of the cube and the plane, as well as the corresponding VAO for the VBO. (5 points)
- [x] Set up the scene such that you have a 10 x 10 plane located at the world origin facing the standard up direction (0, 1, 0), and at least 3 cubes that are positioned on or above the plane. Optionally, each cube can have a different orientation and scale. (5 points)
- [x] Set up a first person camera controllable via WASD keys and mouse. (5 points)
- [x] Set up a directional light that shines down towards the cubes and the plane. This means setting up the uniform variables (light direction, ambient/diffuse/specular). No need to set up the material properties for your objects, but feel free to do so. The direction of the directional light is up to you, e.g., (0, -1, 0), (1, -1, 0), etc. (5 points)
- [x] Generate a framebuffer object (FBO), as well as the depth texture that is to be attached to the framebuffer as a depth attachment. (5 points)
    - [x] Instead of setting the texture's size to be that of the window's width and height, it will be the resolution of the shadow map. 1024 x 1024 is a good starting point, but feel free to go higher (e.g., 2048 x 2048) if your computer can handle it.
    - [x] Since we are creating a texture for storing depth information, instead of specifying GL_RGB as the texture format, set it to GL_DEPTH_COMPONENT, which is the format for storing depth values.
    - [x] When attaching the depth texture to the framebuffer, you can specify that you are attaching the texture as a depth attachment by specifying GL_DEPTH_ATTACHMENT instead of GL_COLOR_ATTACHMENT_0.
    - [x] Since we are not specifying any color attachment, you can tell OpenGL that this is the case via glDrawBuffer(GL_NONE).
    - [x] Finally, for good practice, make sure to check the status of the framebuffer.
####Note: You do NOT have to attach a RenderBuffer to the framebuffer.

- [x] Create a new pair of vertex and fragment shaders (file name is up to you). Note that this pair of shaders is different from your main vertex and fragment shaders. This shader program will be used to generate the depth map texture, and as such, will be focusing on rendering the geometry alone (without any lighting calculations). (5 points)
    - [x] The vertex shader should accept only the vertex position as input, and not output anything to the fragment shader. Leave the main function empty for now.
    - [x] The fragment shader should not accept any input, and the main function should be empty (since we are not outputting any color information).
    - [x] Make sure to create a shader program for this pair of shaders

####Remember that shadow mapping involves rendering the same scene twice:
- The first pass involves rendering the scene from the perspective of the light source to generate the depth texture (a.k.a. shadow map) for the shadow calculation.
- The second pass involves rendering the scene normally, but this time using information from the shadow map.
#### For the first pass (shadow pass):
- [] We are rendering the scene from the perspective of the light source, so instead of using the projection and view matrices of the camera to transform vertex positions from local space to clip space, we will be using the projection and view matrices of the light source. (5 points)
    - [-] Since we have a directional light, we will be using orthographic projection for the directional light. Use GLM's glm::ortho() function to create an orthographic projection matrix by specifying the left/right, up/down, and near/far values that define the range of the frustum.
    - [x] Use GLM's glm::lookAt() function to generate the directional light's view matrix. Specify the "eye position" to be on a high location, preferably above all your objects in the scene, and derive the look target based on the direction of the directional light and the "eye position".
    - [x] Modify your vertex shader from step 8 to accept the light's projection and view matrices, as well as the object's model matrix, as uniform variables. In the vertex shader's main function, set gl_Position to be the vertex position in the light's clip space.
NUM 10
- [] Render the scene to the depth texture via the FBO that you generated back in step 7. (5 points)
    - [x] Make sure to clear the depth buffer via glClear() before drawing anything into the framebuffer
    - [x] Set the viewport via glViewport() to be the same resolution as your shadow map from step 7.
    - [x] Make sure to use the shader program that you created from step 8.
    - [x] Make sure to assign the uniforms for the light's projection and view matrices, and the object's model matrix.

####For the second pass:
- [x] Prepare to render the scene to the default framebuffer
    - [x] Make sure to bind to the default framebuffer so that you are now rendering onto the on-screen framebuffer.
    - [x] Make sure to clear the color and depth buffer via glClear() before drawing anything.
    - [x] Set the viewport via glViewport() to your window's width and height.
    - [x] Make sure to use the main pair of vertex and fragment shaders (the ones with the lighting calculation)
- [x] To check whether a fragment is in shadow, we need to retrieve the depth of the current fragment from the perspective of the light source, which can be done by converting the current fragment's position to the light's clip space (process is similar to step 9). Modify your main vertex shader to accept two mat4 uniforms representing the light's projection and view matrix, and to output to the fragment shader the fragment's position in the light's clip space (in homogeneous coordinates, i.e., vec4). (5 points)
- [] In the main fragment shader, we will then perform the shadow calculation. Modify the main fragment shader as follows. (10 points)
    - [x] Accept the fragment's position in light's clip space from the vertex shader
    - [x] Create a sampler2D uniform variable for the shadow map. Don't forget to bind the shadow map texture back in the C++ application.
    - [] In the main function:
        - [] Transform the fragment's position in the light's clip space (vec4) to NDC (vec3), and store the result in a variable called fragLightNDC. (Hint: How do you transform a homogeneous coordinate to Cartesian coordinate?)
        - [] fragLightNDC.xy can be used as a texture coordinate to retrieve depth values from the depth map, while fragLightNDC.z is the depth of the fragment in light space. However, its values are in the [-1, 1] range, while UV-coordinates and texture values are in the [0, 1] range. Convert fragLightNDC from [-1, 1] to [0, 1].
        - [] Now that we have fragLightNDC to be within the [0, 1] range, we can now properly retrieve depth values from our shadow map. Sample the shadow map using fragLightNDC to retrieve the depth value of the object closest to the light source from the shadow map.
        - [] Compare the depth value retrieved from the shadow map with the depth of the fragment's position in light space. If the depth value from the shadow map is less than the depth of the fragment's position in light space, then the fragment is in shadow, i.e., skip lighting calculations for that fragment.
- [] Assuming you have done the previous steps, you should see the same scene, but with shadows applied. However, you'll notice that there is shadow acne happening. Modify your shadow calculation to fix the shadow acne based on what was discussed in class. (5 points)
- [] (Optional) At this point, we now have shadows, but the edges of the shadows appear very jaggy. Modify your shadow calculation to introduce softer edges to the shadows via Percentage-Closer Filtering (PCF).
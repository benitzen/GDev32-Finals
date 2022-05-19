#ProgEx 2 TODO 

- [x] Duplicate your OpenGL project from Programming Exercise 01, and use the duplicated project as the base for this exercise. This is in case you want to keep your project from Programming Exercise 01 intact.
- [ ] Modify the Vertex struct of your program such that it now contains 3 values nx, ny, and nz which represents the normal vector of the vertex. Set up your vertex array objects (VAOs) also to accommodate this change. Set up the normals for the vertices of all the objects in your scene. Note that depending on the shapes that you use, you may need to duplicate vertices to set up the normals correctly.
- [ ] Modify your vertex shader to receive the vertex normal as an input (the same way you receive other vertex attributes like position, UV), and also to output the vertex normal to the fragment shader. Remember that we are supposed to pass the normal vector to the fragment shader after multiplying it with the normal matrix.
- [ ] Modify your fragment shader to receive the normal vector from the vertex shader. Run your program just to make sure that everything is still working as normal.
- [ ] Add a point light to the scene by creating uniform variables in the fragment shader that are relevant to a point light. Set the values of these uniforms from your C++ application as desired
    - [ ] Point light position (x, y, z)
    - [ ] Point light ambient, diffuse, and specular intensities (r, g, b)
    
- [ ] For the object's material properties, create a uniform variable for the object's specular component (𝑘<sub>𝑠</sub>), and a uniform variable for the object's shininess (𝑠). Set the values of these uniforms as desired. What about the material's ambient and diffuse components? (Hint: all of our objects use textures).

- [ ] Perform the lighting calculations for the point light using the Phong lighting equation. Create additional uniforms for any other information that is required for the lighting calculation.
- [ ] Add a directional light to your scene on top of your point light. Set up additional uniform variables needed for your directional light source. Now that you have two light sources, apply the Phong lighting equation for multiple light sources.
- [ ] Finally, add a spot light to your scene on top of the two other light sources. Once again, set up additional uniform variables needed for your spot light, and add the spot light calculation to your Phong lighting calculation.
    - [ ] Bonus: Applying soft edges to your spot light
- [ ] Incorporate attenuation to your light sources. For each light source, you can set up uniform variables for their corresponding constant, linear, and quadratic terms.
- [ ] Feel free to make adjustments to your original scene to fit the current exercise. Also, feel free to add more instances of each light source (e.g., 3 point lights, 1 directional light, 1 spot light). Finally, feel free to explore using structs and arrays in shaders to make uniforms easier to manage
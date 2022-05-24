#Ray tracing

- [ ] Study and understand the structs, functions, and the basic structure of the template. As a warm-up, try to make it such that the output image has a different color instead of black.
- [x] Set up camera
    - [x] Position: (0, 0, 3)
    - [x] Look Target: (0, 0, 0)
    - [x] Global Up: (0, 1, 0)
    - [x] Vertical FOV: 45 degrees
    - [x] Focal Length: 1m
    - [x] Image Resolution 640x480
- [x] Let's construct a simple scene by adding a sphere to the scene object, with the sphere centered at (0, 0, 0) and with a radius of 1. Set the sphere's diffuse material to be red (1, 0, 0). You can use the new operator to create new scene objects, e.g.,
    - [x] Sphere *sphere = new Sphere();
    - [x] Add sphere to the list of objects in the scene obj
- [x] Implement the GetRayThruPixel() function such that it returns the ray that starts from the camera's position and passes through the point in the viewport that corresponds to the specified pixel position, i.e., the primary ray for the specified pixel position.
- [x] Implement the Sphere struct's Intersect() function using the procedure for Ray-Sphere intersection discussed in class.
- [x] Implement the Raycast() function such that it goes through all the objects in the scene and looks for the object that the ray intersects and is closest to the ray's origin. If the ray hits an object, it should return an IntersectionInfo struct that contains information about the intersection. Otherwise, it should still return an IntersectionInfo struct, but with the obj member variable set to nullptr.
- [x] Modify the RayTrace() function such that it uses the Raycast() function to get the first object that is hit by the ray. You can check if the ray hit something by checking if the obj member variable of the IntersectionInfo struct is null or not. If the ray hitan object, return the diffuse color of that object. Otherwise, it should return the background color (feel free to change the background color).
- [x] DO U SEE RED CIRCLE

###AT THIS POINT YOU SHOULD UNDERSTAND FLOW AND STRUCTURE OF THE CODE
- [ ] Implement the Triangle struct's Intersect() function using the procedure for Ray-Triangle intersection discussed in class.
- [ ] Add a triangle behind the sphere with a green diffuse material to the scene. Run the application again, and this time, you should see the triangle behind the sphere.

###Make the application more flexible by constructing the scene based on data obtained from reading a file. You may refer to one of the test scene files as you go along with the format explanation
- [x] Modify the application such that it asks for the file path to a scene file, and constructs the scene from that scene file. (5 points)
- [x] All scene files start with two integers representing the image width and height. This is then followed by three float values representing the camera's position (xyz), three float values representing the camera's look target (xyz), three float values representing the camera's global up vector (xyz), and finally two float values representing the camera's vertical field of view (in degrees) and focal length.
- [x] The file continues with an integer representing the maximum recursion depth for the ray-tracer, followed by an integer N representing the number of objects in the scene. N sets of 2 lines each will follow. The first line in each set starts with a string that is either a "sphere" or "tri". If the line starts with "sphere", the next 4 floats represent the sphere's center (xyz) and radius (r). If the line starts with "tri", the next 9 floats represent the three points of the triangle A (xyz), B (xyz), and C (xyz). Regardless of whether it's a sphere or a triangle, the next line in each set will contain 10 floats representing the object material's ambient (rgb), diffuse (rgb), specular (rgb), and shininess (s).
- [x] The file continues with an integer L representing the number of lights in the scene. L lines follow, with each line containing 16 floats divided as follows:
    - [x] Light position (xyzw)
    - [x] Light's ambient (RGB), diffuse (RGB), and Specular (RGB) intensities
    - [x] Light's constant, linear, and quadratic attenuation factors
- [ ] (Optional) Modify the GetRayThruPixel() and the main function to implement anti-aliasing. You are free to decide the number of samples per pixel. Just note that the higher the number of samples, the slower the rendering will be.
- [x] Modify your RayTrace() function such that it now returns the resulting color after performing calculations for Phong lighting (ambient, diffuse, and specular) for multiple light sources. The Scene struct contains the list of lights in the scene. Note that for this exercise, we only have two types of light: point and directional light. Refer to the Light struct for information on how to distinguish between the two.
- [x] Incorporate attenuation to your lighting calculation using the light's constant, linear, and quadratic attenuation terms.
- [x] Incorporate shadow calculation to your lighting calculation. Make sure to address the problem of shadow acne.
- [ ] Implement recursive ray-tracing by adding specular reflection to your color calculation for reflective surfaces.
    - [ ] This is done by creating a new ray that starts from the current point and goes towards the reflection of the direction of the incoming ray along the current normal vector, getting the color from the resulting ray-trace, and adding that color to your final color computation. Note that your application should not do recursion infinitely; rather it should do recursion up to the specified maximum depth from the scene file. Regarding the object's reflectivity, normally the reflectivity value 𝐾r is stored in the object's material. However, for this exercise, we will instead calculate the reflectivity of the object based on the material's shininess value 𝑁s as follows:
        - [ ]  𝐾r = 𝑁s / 128
- [ ] Submit all your source files (.h, .cpp), along with a filled-in Certificate of Authorship to the submission link in our class website in Canvas (Programming Exercise 04 – Ray Tracing). No need to include the stb_image_write.h file in your submission.

##References
- https://www.cplusplus.com/doc/tutorial/files/
- https://www.cplusplus.com/forum/beginner/78150/
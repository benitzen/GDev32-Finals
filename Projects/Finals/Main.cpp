#include "../../Include/glm/glm.hpp"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Ray
{
    glm::vec3 origin;    // Ray origin
    glm::vec3 direction; // Ray direction
};

struct Material
{
    glm::vec3 ambient;  // Ambient
    glm::vec3 diffuse;  // Diffuse
    glm::vec3 specular; // Specular
    float shininess;    // Shininess
};

struct SceneObject
{
    Material material; // Material

    /**
     * Template function for calculating the intersection of this object with the provided ray.
     * @param[in]   incomingRay             Ray that will be checked for intersection with this object
     * @param[out]  outIntersectionPoint    Point of intersection (in case there is an intersection)
     * @param[out]  outIntersectionNormal   Normal vector at the point of intersection (in case there is an intersection)
     * @return If there is an intersection, returns the distance from the ray origin to the intersection point. Otherwise, returns a negative number.
     */
    virtual float Intersect(const Ray &incomingRay, glm::vec3 &outIntersectionPoint, glm::vec3 &outIntersectionNormal) = 0;
};

// Subclass of SceneObject representing a Sphere scene object
struct Sphere : public SceneObject
{
    glm::vec3 center; // center
    float radius;     // radius

    /**
     * @brief Ray-sphere intersection
     * @param[in]   incomingRay             Ray that will be checked for intersection with this object
     * @param[out]  outIntersectionPoint    Point of intersection (in case there is an intersection)
     * @param[out]  outIntersectionNormal   Normal vector at the point of intersection (in case there is an intersection)
     * @return If there is an intersection, returns the distance from the ray origin to the intersection point. Otherwise, returns a negative number.
     */
    virtual float Intersect(const Ray &incomingRay, glm::vec3 &outIntersectionPoint, glm::vec3 &outIntersectionNormal)
    {
        float t = 0.0f;
        // Ray = P + td
        // P = origin of ray
        // d = direction of ray
        // t = - b +- sqrt(b^2 - c)
        // m = origin of ray - center of sphere
        // r = radius of sphere
        // b = m dot d
        // c = (m dot m) - r^2

        glm::vec3 p = incomingRay.origin;
        glm::vec3 d = incomingRay.direction;
        glm::vec3 m = p - center;
        float b = glm::dot(m, d);
        float c = glm::dot(m, m) - (radius * radius);
        float root;
        float rootTwo;

        // To check for intersection, solve b^2 - c
        float isIntersecting = (b * b) - c;

        if (isIntersecting < 0)
        {
            return -1.0f;
        }
        else if (isIntersecting == 0)
        {
            t = -1 * b;
        }
        else if (isIntersecting > 0)
        {
            root = (-1 * b) + sqrt(isIntersecting);
            rootTwo = (-1 * b) - sqrt(isIntersecting);
            if (root > 0 && rootTwo > 0)
            {
                // t = (root < rootTwo) ? root : rootTwo;
                if (root < rootTwo)
                {
                    t = root;
                }
                else
                {
                    t = rootTwo;
                }
            }
            else if (root > 0 && rootTwo < 0)
            {
                t = root;
            }
            else if (root < 0 && rootTwo > 0)
            {
                t = rootTwo;
            }
        }

        outIntersectionPoint = glm::vec3(p + (t * d));
        outIntersectionNormal = glm::normalize(outIntersectionPoint);
        //
        // In case there is an intersection, place the intersection point and intersection normal
        // that you calculated to the outIntersectionPoint and outIntersectionNormal variables.
        //
        // When you use this function from the outside, you can pass in the variables by reference.
        //
        // Example:
        // Ray ray = ...;
        // glm::vec3 point, normal;
        // float t = sphere->Intersect(ray, point, normal);
        //
        // (At this point, point and normal will now contain the intersection point and intersection normal)

        return t;
    }
};

// Subclass of SceneObject representing a Triangle scene object
struct Triangle : public SceneObject
{
    glm::vec3 A; // First point
    glm::vec3 B; // Second point
    glm::vec3 C; // Third point

    /**
     * @brief Ray-Triangle intersection
     * @param[in]   incomingRay             Ray that will be checked for intersection with this object
     * @param[out]  outIntersectionPoint    Point of intersection (in case there is an intersection)
     * @param[out]  outIntersectionNormal   Normal vector at the point of intersection (in case there is an intersection)
     * @return If there is an intersection, returns the distance from the ray origin to the intersection point. Otherwise, returns a negative number.
     */
    virtual float Intersect(const Ray &incomingRay, glm::vec3 &outIntersectionPoint, glm::vec3 &outIntersectionNormal)
    {
        float s = 0.0f;

        glm::vec3 d = incomingRay.direction;
        glm::vec3 n = glm::cross((B - A), (C - A));
        float f = glm::dot(-d, n);
        glm::vec3 e = glm::cross(-d, (incomingRay.origin - A));

        // If no intersect
        if (f <= 0)
        {
            return -1.0f;
        }

        float tNumerator = glm::dot((incomingRay.origin - A), n);
        float t = tNumerator / f;

        // If no intersect
        if (t <= 0)
        {
            return -1.0f;
        }

        float uNumerator = glm::dot((C - A), e);
        float u = uNumerator / f;

        float vNumerator = glm::dot(-(B - A), e);
        float v = vNumerator / f;

        bool uAndVPositive = u >= 0 && v >= 0;
        bool uVSumLessThanOrEq1 = u + v <= 1;
        if (uAndVPositive && uVSumLessThanOrEq1)
        {
            s = t;
        }
        else
        {
            s = -1.0f;
        }

        glm::vec3 intersectionPoint = A + (u * (B - A)) + (v * (C - A));
        glm::vec3 normalizedIntersectionPoint = glm::normalize(n);

        outIntersectionPoint = intersectionPoint;
        outIntersectionNormal = normalizedIntersectionPoint;

        return s;
    }
};

struct Camera
{
    glm::vec3 position;   // Position
    glm::vec3 lookTarget; // Look target
    glm::vec3 globalUp;   // Global up-vector
    float fovY;           // Vertical field of view
    float focalLength;    // Focal length

    int imageWidth;  // image width
    int imageHeight; // image height
};

struct Light
{
    glm::vec4 position; // Light position (w = 1 if point light, w = 0 if directional light)

    glm::vec3 ambient;  // Light's ambient intensity
    glm::vec3 diffuse;  // Light's diffuse intensity
    glm::vec3 specular; // Light's specular intensity

    // --- Attenuation variables ---
    float constant;  // Constant factor
    float linear;    // Linear factor
    float quadratic; // Quadratic factor
};

struct IntersectionInfo
{
    Ray incomingRay;              // Ray used to calculate the intersection
    float t;                      // Distance from the ray's origin to the point of intersection (if there was an intersection).
    SceneObject *obj;             // Object that the ray intersected with. If this is equal to nullptr, then no intersection occured.
    glm::vec3 intersectionPoint;  // Point where the intersection occured (if there was an intersection)
    glm::vec3 intersectionNormal; // Normal vector at the point of intersection (if there was an intersection)
};

struct Scene
{
    std::vector<SceneObject *> objects; // List of all objects in the scene
    std::vector<Light> lights;          // List of all lights in the scene
};

struct Image
{
    std::vector<unsigned char> data; // Image data
    int width;                       // Image width
    int height;                      // Image height

    /**
     * @brief Constructor
     * @param[in] w Width
     * @param[in] h Height
     */
    Image(const int &w, const int &h)
        : width(w), height(h)
    {
        data.resize(w * h * 3, 0);
    }

    /**
     * @brief Converts the provided color value from [0, 1] to [0, 255]
     * @param[in] c Color value in [0, 1] range
     * @return Color value in [0, 255] range
     */
    unsigned char ToChar(float c)
    {
        c = glm::clamp(c, 0.0f, 1.0f);
        return static_cast<unsigned char>(c * 255);
    }

    /**
     * @brief Sets the color at the specified pixel location
     * @param[in] x     X-coordinate of the pixel
     * @param[in] y     Y-coordinate of the pixel
     * @param[in] color Pixel color
     */
    void SetColor(const int &x, const int &y, const glm::vec3 &color)
    {
        int index = (y * width + x) * 3;
        data[index] = ToChar(color.r);
        data[index + 1] = ToChar(color.g);
        data[index + 2] = ToChar(color.b);
    }
};

/**
 * @brief Gets the ray that goes from the camera's position to the specified pixel at (x, y)
 * @param[in] camera Camera data
 * @param[in] x X-coordinate of the pixel (upper-left corner of the pixel)
 * @param[in] y Y-coordinate of the pixel (upper-left corner of the pixel)
 * @return Ray that passes through the pixel at (x, y)
 */
Ray GetRayThruPixel(const Camera &camera, const int &pixelX, const int &pixelY)
{
    Ray ray;
    ray.origin = camera.position;

    // viewport calculations (slide 17)
    float aspectRatio = camera.imageWidth / (float)camera.imageHeight;
    float hViewport = 2 * camera.focalLength * tan((camera.fovY * M_PI / 180) / 2);
    float wViewport = aspectRatio * hViewport;

    // vector u and vector v calculations (slide 19)
    glm::vec3 lookDirection = glm::normalize(camera.lookTarget - ray.origin);
    glm::vec3 upVector = glm::cross(lookDirection, camera.globalUp);
    glm::vec3 vVector = glm::cross(upVector, lookDirection);

    if (upVector != glm::vec3(0.0f))
    {
        upVector = glm::normalize(upVector);
    }

    if (vVector != glm::vec3(0.0f))
    {
        vVector = glm::normalize(vVector);
    }

    // lower-left corner point L calculations
    glm::vec3 L = camera.position + (lookDirection * camera.focalLength) - (upVector * (wViewport / 2)) - (vVector * (hViewport / 2));

    // calculate ray going through pixel (x,y)
    float s = ((pixelX + 0.5) / camera.imageWidth) * wViewport;
    float t = ((pixelY + 0.5) / camera.imageHeight) * hViewport;
    glm::vec3 P = L + upVector * s + vVector * t;
    glm::vec3 rayDirection = P - ray.origin;

    if (rayDirection != glm::vec3(0.0f))
    {
        rayDirection = glm::normalize(rayDirection);
    }

    ray.direction = rayDirection;
    return ray;
}

/**
 * @brief Cast a ray to the scene.
 * @param[in] ray   Ray to cast to the scene
 * @param[in] scene Scene object
 * @return Returns an IntersectionInfo object that will contain the results of the raycast
 */
IntersectionInfo Raycast(const Ray &ray, const Scene &scene)
{
    std::vector<IntersectionInfo> infoList;

    for (int i = 0; i < scene.objects.size(); i++)
    {
        glm::vec3 outIntersectionPoint(0.0f);
        glm::vec3 outIntersectionNormal(0.0f);

        IntersectionInfo ret;
        ret.incomingRay = ray;

        float rayDist = scene.objects[i]->Intersect(ray, outIntersectionPoint, outIntersectionNormal);

        // Fields that need to be populated:
        ret.intersectionPoint = outIntersectionPoint;   // Intersection point
        ret.intersectionNormal = outIntersectionNormal; // Intersection normal
        ret.t = rayDist;                                // Distance from ray origin to intersection point
        ret.obj = nullptr;                              // First object hit by the ray. Set to nullptr if the ray does not hit anything

        if (rayDist > 0)
        {
            ret.obj = scene.objects[i];
            infoList.push_back(ret);
        }
    }

    IntersectionInfo ret;

    if (infoList.size() <= 0)
    {
        ret.obj = nullptr;
    }
    else
    {
        ret = infoList[0];
        for (int i = 0; i < infoList.size(); i++)
        {
            if (ret.t < 0 && infoList[i].t >= 0)
            {
                ret = infoList[i];
            }
            if (ret.t > infoList[i].t && infoList[i].t >= 0)
            {
                ret = infoList[i];
            }
        }
    }

    return ret;
}

/**
 * @brief Perform a ray-trace to the scene
 * @param[in] ray       Ray to trace
 * @param[in] scene     Scene data
 * @param[in] camera    Camera data
 * @param[in] maxDepth  Maximum depth of the trace
 * @return Resulting color after the ray bounced around the scene
 */
glm::vec3 RayTrace(const Ray &ray, const Scene &scene, const Camera &camera, int maxDepth = 1)
{
    glm::vec3 color(0.0f);

    IntersectionInfo didRayHit = Raycast(ray, scene);
    if (didRayHit.obj == nullptr)
    {
        return glm::vec3(0.0f);
    }

    glm::vec3 ambient(0.0f);
    glm::vec3 diffuse(0.0f);
    glm::vec3 specular(0.0f);
    glm::vec3 lightDirection(0.0f);
    glm::vec3 colorTemp(0.0f);
    glm::vec3 colorCombinedTemp(0.0f);
    float zeroConst = 0.0f;

    for (int i = 0; i < scene.lights.size(); i++)
    {
        if (didRayHit.obj == nullptr)
        {
            color = glm::vec3(0.0f);
            continue;
        }

        float lightW = scene.lights[i].position.w;
        // std::cout << "Light w: " << lightW << std::endl;

        // Point Light W
        if (lightW == 1.0f)
        {
            // ambient lighting
            ambient = scene.lights[i].ambient * didRayHit.obj->material.ambient;

            // diffuse lighting
            glm::vec3 norm = didRayHit.intersectionNormal;
            lightDirection = glm::normalize(glm::vec3(scene.lights[i].position) - didRayHit.intersectionPoint);
            float diff = std::max(glm::dot(norm, lightDirection), zeroConst);
            diffuse = scene.lights[i].diffuse * (diff * didRayHit.obj->material.diffuse);

            // specular lighting
            glm::vec3 viewDirection = glm::normalize(camera.position - didRayHit.intersectionPoint);
            glm::vec3 reflectDirection = glm::reflect(-lightDirection, norm);
            float spec = pow(std::max(glm::dot(viewDirection, reflectDirection), zeroConst), didRayHit.obj->material.shininess);
            specular = scene.lights[i].specular * (spec * didRayHit.obj->material.specular);

            // point light attenuation
            float distance = glm::length(glm::vec3(scene.lights[i].position) - didRayHit.intersectionPoint);
            float attenuation = 1.0f / (scene.lights[i].constant + scene.lights[i].linear * distance + scene.lights[i].quadratic * pow(distance, 2));

            ambient *= attenuation;
            diffuse *= attenuation;
            specular *= attenuation;
        }
        else if (lightW == 0.0f)
        {
            // Directional Light
            // ambient lighting
            ambient = scene.lights[i].ambient * didRayHit.obj->material.ambient;

            // diffuse lighting
            glm::vec3 norm = didRayHit.intersectionNormal;
            lightDirection = glm::normalize(-1.0f * glm::vec3(scene.lights[i].position));
            float diff = std::max(glm::dot(norm, lightDirection), zeroConst);
            diffuse = diff * (didRayHit.obj->material.diffuse * scene.lights[i].diffuse);

            // specular lighting
            glm::vec3 viewDirection = glm::normalize(camera.position - didRayHit.intersectionPoint);
            glm::vec3 reflectDirection = glm::reflect(-lightDirection, norm);
            float spec = pow(std::max(glm::dot(viewDirection, reflectDirection), zeroConst), didRayHit.obj->material.shininess);
            specular = scene.lights[i].specular * (spec * didRayHit.obj->material.specular);
        }

        Ray shadow;
        bool isShadow = false;
        float shadowVal = 0.0f;

        for (int j = 0; j < scene.objects.size(); j++)
        {
            shadow.origin = didRayHit.intersectionPoint + (didRayHit.intersectionNormal * 0.01f);
            float lightW = scene.lights[i].position.w;

            if (lightW == 0.0f)
            {
                /* code */
                shadow.direction = glm::normalize(-glm::vec3(scene.lights[i].position));
            }
            else if (lightW == 1.0f)
            {
                /* code */
                shadow.direction = glm::normalize(glm::vec3(scene.lights[i].position) - shadow.origin);
            }

            glm::vec3 outIntersectionPoint(0.0f);
            glm::vec3 outIntersectionNormal(0.0f);

            float rayDist = scene.objects[j]->Intersect(shadow, outIntersectionPoint, outIntersectionNormal);
            // In your shadow calculation, remember that directional lights do not have a position,
            // so you cannot really measure the distance between the intersection point and the
            // directional light. Also, you are getting the length of lightDirection, which if you
            // notice is already normalized. Hence, it will always have a length of 1.
            // if (glm::length(lightDirection) > rayDist && rayDist > 0)
            if (rayDist < 1 && rayDist > 0)
            {
                isShadow = true;
            }
        }
        if (isShadow)
        {
            // std::cout << "shadow" << std::endl;
            shadowVal = 1.0f;
        }
        else
        {
            shadowVal = 0.0f;
            if (maxDepth > 0)
            {
                Ray reflection;
                reflection.origin = didRayHit.intersectionPoint + (didRayHit.intersectionNormal * 0.001f);
                reflection.direction = glm::reflect(didRayHit.incomingRay.direction, didRayHit.intersectionNormal);
                float kr = didRayHit.obj->material.shininess / 128;
                colorCombinedTemp += (kr * RayTrace(reflection, scene, camera, maxDepth - 1));
            }
        }

        colorTemp = ambient + (1.0f - shadowVal) * (diffuse + specular);
        colorCombinedTemp += colorTemp;
    }

    color = colorCombinedTemp;
    return color;
}

/**
 * Main function
 */
int main()
{
    int bounceY[16] = {8, 7, 6, 5, 4, 3, 2, 1, 1, 2, 3, 4, 5, 6, 7, 8};
    float pyramidSide1BX[16] = {-9,-8.90625, -8.8125,-8.71875, -8.625, -8.53125, -8.4375, -8.34375, -8.25, -8.15625, -8.0625, -7.96875, -7.875,-7.78125,-7.6875,-7.59375};
    float pyramidSide1BZ[16] = {4.5, 4.40625, 4.3125, 4.21875, 4.125, 4.03125, 3.9375, 3.84375, 3.75, 3.65625, 3.5625, 3.46875, 3.375, 3.28125, 3.1875, 3.09375};
    float pyramidSide1CX[16] = {-7.5, -7.40625, -7.3125, -7.21875, -7.125, -7.03125, -6.9375, -6.84375, -6.75, -6.65625, -6.5625, -6.46875, -6.375, -6.28125, -6.1875, -6.09375 };
    float pyramidSide1CZ[16] = {3, 3.09375, 3.1875, 3.28125, 3.375, 3.46875, 3.5625, 3.65625, 3.75, 3.84375, 3.9375, 4.03125, 4.125, 4.21875, 4.3125, 4.40625 };

    float pyramidSide2BX[16] = {-7.5, -7.40625, -7.3125, -7.21875, -7.125, -7.03125, -6.9375, -6.84375, -6.75, -6.65625, -6.5625, -6.46875, -6.375, -6.28125, -6.1875, -6.09375 };
    float pyramidSide2BZ[16] = {3, 3.09375, 3.1875, 3.28125, 3.375, 3.46875, 3.5625, 3.65625, 3.75, 3.84375, 3.9375, 4.03125, 4.125, 4.21875, 4.3125, 4.40625 };
    float pyramidSide2CX[16] = {-6, -6.09375, -6.1875, -6.28125, -6.375, -6.46875, -6.5625, -6.65625, -6.75, -6.84375, -6.9375, -7.03125, -7.125, -7.21875, -7.3125, -7.40625};
    float pyramidSide2CZ[16] = {4.5, 4.59375, 4.6875, 4.78125, 4.875, 4.96875,5.0625,5.15625,5.25, 5.34375,5.4375,5.53125,5.625,5.71875, 5.8125,5.90625};

    float pyramidSide3BX[16] = {-6, -6.09375, -6.1875, -6.28125, -6.375, -6.46875, -6.5625, -6.65625, -6.75, -6.84375, -6.9375, -7.03125, -7.125, -7.21875, -7.3125, -7.40625 };
    float pyramidSide3BZ[16] = { 4.5, 4.59375, 4.6875, 4.78125, 4.875, 4.96875,5.0625,5.15625,5.25, 5.34375,5.4375,5.53125,5.625,5.71875, 5.8125,5.90625 };
    float pyramidSide3CX[16] = {-7.5,-7.59375, -7.6875, -7.78125, -7.875, -7.96875, -8.0625, -8.15625, -8.25, -8.34375, -8.4375, -8.53125, -8.625,-8.71875, -8.8125,-8.90625 };
    float pyramidSide3CZ[16] = { 6, 5.90625,  5.8125, 5.71875,  5.625, 5.53125,  5.4375, 5.34375, 5.25, 5.15625, 5.0625, 4.96875, 4.875, 4.78125,4.6875, 4.59375 };

    float pyramidSide4BX[16] = {-7.5,-7.59375, -7.6875, -7.78125, -7.875, -7.96875, -8.0625, -8.15625, -8.25, -8.34375, -8.4375, -8.53125, -8.625,-8.71875, -8.8125,-8.90625 };;
    float pyramidSide4BZ[16] = { 6, 5.90625,  5.8125, 5.71875,  5.625, 5.53125,  5.4375, 5.34375, 5.25, 5.15625, 5.0625, 4.96875, 4.875, 4.78125,4.6875, 4.59375};
    float pyramidSide4CX[16] = {-9,-8.90625, -8.8125,-8.71875, -8.625, -8.53125, -8.4375, -8.34375, -8.25, -8.15625, -8.0625, -7.96875, -7.875,-7.78125,-7.6875,-7.59375 };
    float pyramidSide4CZ[16] = {4.5, 4.40625, 4.3125, 4.21875, 4.125, 4.03125, 3.9375, 3.84375, 3.75, 3.65625, 3.5625, 3.46875, 3.375, 3.28125, 3.1875, 3.09375 };
    for (int animationIndex = 0; animationIndex < 16; animationIndex++)
    {

        Scene scene;
        Camera camera;
        Sphere *sphere;
        Triangle *triangle;

        std::string filepath;
        std::fstream scenefile;
        std::vector<std::string> filecontent;
        // std::cout << "Enter Scene (e.g. scene2d.test)" << std::endl;
        // std::cin >> filepath;
        // std::cout << std::endl << filepath << std::endl;

        // testing filepath comment out before submission
        filepath = "checkboard.test";
        // filepath = "scene2b.test";
        scenefile.open(filepath, std::ios::in);
        if (scenefile)
        {
            std::cout << "File exist" << std::endl;
            // while (!scenefile.eof())
            while (!scenefile.eof())
            {
                char foo[16];
                scenefile >> foo;
                filecontent.push_back(foo);
            }
            // filecontent.pop_back();
            scenefile.close();
        }

        for (int i = 0; i < filecontent.size(); i++)
        {
            std::cout << "File index " << i << ": " << filecontent[i] << std::endl;
        }

        // Camera Initialization
        camera.imageWidth = std::stoi(filecontent[0]);
        camera.imageHeight = std::stoi(filecontent[1]);
        camera.position = glm::vec3(
            std::stof(filecontent[2]),
            std::stof(filecontent[3]),
            std::stof(filecontent[4]));
        camera.lookTarget = glm::vec3(
            std::stof(filecontent[5]),
            std::stof(filecontent[6]),
            std::stof(filecontent[7]));
        camera.globalUp = glm::vec3(
            std::stof(filecontent[8]),
            std::stof(filecontent[9]),
            std::stof(filecontent[10]));
        camera.fovY = std::stof(filecontent[11]);
        camera.focalLength = std::stof(filecontent[12]);
        int maxDepth = std::stoi(filecontent[13]);
        int numObj = std::stoi(filecontent[14]);

        int startNum = 15;
        for (int i = 0; i < numObj; i++)
        {
            if (filecontent[startNum] == "sphere")
            {
                // Sphere Initialization
                sphere = new Sphere();
                sphere->center = glm::vec3(
                    std::stof(filecontent[startNum + 1]),
                    std::stof(filecontent[startNum + 2]),
                    std::stof(filecontent[startNum + 3]));
                sphere->radius = std::stof(filecontent[startNum + 4]);
                sphere->material.ambient = glm::vec3(
                    std::stof(filecontent[startNum + 5]),
                    std::stof(filecontent[startNum + 6]),
                    std::stof(filecontent[startNum + 7]));
                sphere->material.diffuse = glm::vec3(
                    std::stof(filecontent[startNum + 8]),
                    std::stof(filecontent[startNum + 9]),
                    std::stof(filecontent[startNum + 10]));
                sphere->material.specular = glm::vec3(
                    std::stof(filecontent[startNum + 11]),
                    std::stof(filecontent[startNum + 12]),
                    std::stof(filecontent[startNum + 13]));
                sphere->material.shininess = std::stof(filecontent[startNum + 14]);

                startNum += 15;
                scene.objects.push_back(sphere);
            }
            if (filecontent[startNum] == "sphereBounce")
            {
                // Sphere Initialization
                sphere = new Sphere();
                sphere->center = glm::vec3(
                    std::stof(filecontent[startNum + 1]),
                    bounceY[animationIndex],
                    std::stof(filecontent[startNum + 3]));
                sphere->radius = std::stof(filecontent[startNum + 4]);
                sphere->material.ambient = glm::vec3(
                    std::stof(filecontent[startNum + 5]),
                    std::stof(filecontent[startNum + 6]),
                    std::stof(filecontent[startNum + 7]));
                sphere->material.diffuse = glm::vec3(
                    std::stof(filecontent[startNum + 8]),
                    std::stof(filecontent[startNum + 9]),
                    std::stof(filecontent[startNum + 10]));
                sphere->material.specular = glm::vec3(
                    std::stof(filecontent[startNum + 11]),
                    std::stof(filecontent[startNum + 12]),
                    std::stof(filecontent[startNum + 13]));
                sphere->material.shininess = std::stof(filecontent[startNum + 14]);

                startNum += 15;
                scene.objects.push_back(sphere);
            }

            if (filecontent[startNum] == "tri")
            {
                // Triangle Initialization
                triangle = new Triangle();
                triangle->A = glm::vec3(
                    std::stof(filecontent[startNum + 1]),
                    std::stof(filecontent[startNum + 2]),
                    std::stof(filecontent[startNum + 3]));
                triangle->B = glm::vec3(
                    std::stof(filecontent[startNum + 4]),
                    std::stof(filecontent[startNum + 5]),
                    std::stof(filecontent[startNum + 6]));
                triangle->C = glm::vec3(
                    std::stof(filecontent[startNum + 7]),
                    std::stof(filecontent[startNum + 8]),
                    std::stof(filecontent[startNum + 9]));
                triangle->material.ambient = glm::vec3(
                    std::stof(filecontent[startNum + 10]),
                    std::stof(filecontent[startNum + 11]),
                    std::stof(filecontent[startNum + 12]));
                triangle->material.diffuse = glm::vec3(
                    std::stof(filecontent[startNum + 13]),
                    std::stof(filecontent[startNum + 14]),
                    std::stof(filecontent[startNum + 15]));
                triangle->material.specular = glm::vec3(
                    std::stof(filecontent[startNum + 16]),
                    std::stof(filecontent[startNum + 17]),
                    std::stof(filecontent[startNum + 18]));
                triangle->material.shininess = std::stof(filecontent[startNum + 19]);
                startNum += 20;
                scene.objects.push_back(triangle);
            }
            if (filecontent[startNum] == "triSide1")
            {
                // Triangle Initialization
                triangle = new Triangle();
                triangle->A = glm::vec3(
                    std::stof(filecontent[startNum + 1]),
                    std::stof(filecontent[startNum + 2]),
                    std::stof(filecontent[startNum + 3]));
                triangle->B = glm::vec3(
                    pyramidSide1BX[animationIndex],
                    std::stof(filecontent[startNum + 5]),
                    pyramidSide1BZ[animationIndex]);
                triangle->C = glm::vec3(
                    pyramidSide1CX[animationIndex],
                    std::stof(filecontent[startNum + 8]),
                    pyramidSide1CZ[animationIndex]);
                triangle->material.ambient = glm::vec3(
                    std::stof(filecontent[startNum + 10]),
                    std::stof(filecontent[startNum + 11]),
                    std::stof(filecontent[startNum + 12]));
                triangle->material.diffuse = glm::vec3(
                    std::stof(filecontent[startNum + 13]),
                    std::stof(filecontent[startNum + 14]),
                    std::stof(filecontent[startNum + 15]));
                triangle->material.specular = glm::vec3(
                    std::stof(filecontent[startNum + 16]),
                    std::stof(filecontent[startNum + 17]),
                    std::stof(filecontent[startNum + 18]));
                triangle->material.shininess = std::stof(filecontent[startNum + 19]);
                startNum += 20;
                scene.objects.push_back(triangle);
            }
            if (filecontent[startNum] == "triSide2")
            {
                // Triangle Initialization
                triangle = new Triangle();
                triangle->A = glm::vec3(
                    std::stof(filecontent[startNum + 1]),
                    std::stof(filecontent[startNum + 2]),
                    std::stof(filecontent[startNum + 3]));
                triangle->B = glm::vec3(
                    pyramidSide2BX[animationIndex],
                    std::stof(filecontent[startNum + 5]),
                    pyramidSide2BZ[animationIndex]);
                triangle->C = glm::vec3(
                    pyramidSide2CX[animationIndex],
                    std::stof(filecontent[startNum + 8]),
                    pyramidSide2CZ[animationIndex]);
                triangle->material.ambient = glm::vec3(
                    std::stof(filecontent[startNum + 10]),
                    std::stof(filecontent[startNum + 11]),
                    std::stof(filecontent[startNum + 12]));
                triangle->material.diffuse = glm::vec3(
                    std::stof(filecontent[startNum + 13]),
                    std::stof(filecontent[startNum + 14]),
                    std::stof(filecontent[startNum + 15]));
                triangle->material.specular = glm::vec3(
                    std::stof(filecontent[startNum + 16]),
                    std::stof(filecontent[startNum + 17]),
                    std::stof(filecontent[startNum + 18]));
                triangle->material.shininess = std::stof(filecontent[startNum + 19]);
                startNum += 20;
                scene.objects.push_back(triangle);
            }
            if (filecontent[startNum] == "triSide3")
            {
                // Triangle Initialization
                triangle = new Triangle();
                triangle->A = glm::vec3(
                    std::stof(filecontent[startNum + 1]),
                    std::stof(filecontent[startNum + 2]),
                    std::stof(filecontent[startNum + 3]));
                triangle->B = glm::vec3(
                    pyramidSide3BX[animationIndex],
                    std::stof(filecontent[startNum + 5]),
                    pyramidSide3BZ[animationIndex]);
                triangle->C = glm::vec3(
                    pyramidSide3CX[animationIndex],
                    std::stof(filecontent[startNum + 8]),
                    pyramidSide3CZ[animationIndex]);
                triangle->material.ambient = glm::vec3(
                    std::stof(filecontent[startNum + 10]),
                    std::stof(filecontent[startNum + 11]),
                    std::stof(filecontent[startNum + 12]));
                triangle->material.diffuse = glm::vec3(
                    std::stof(filecontent[startNum + 13]),
                    std::stof(filecontent[startNum + 14]),
                    std::stof(filecontent[startNum + 15]));
                triangle->material.specular = glm::vec3(
                    std::stof(filecontent[startNum + 16]),
                    std::stof(filecontent[startNum + 17]),
                    std::stof(filecontent[startNum + 18]));
                triangle->material.shininess = std::stof(filecontent[startNum + 19]);
                startNum += 20;
                scene.objects.push_back(triangle);
            }
            if (filecontent[startNum] == "triSide4")
            {
                // Triangle Initialization
                triangle = new Triangle();
                triangle->A = glm::vec3(
                    std::stof(filecontent[startNum + 1]),
                    std::stof(filecontent[startNum + 2]),
                    std::stof(filecontent[startNum + 3]));
                triangle->B = glm::vec3(
                    pyramidSide4BX[animationIndex],
                    std::stof(filecontent[startNum + 5]),
                    pyramidSide4BZ[animationIndex]);
                triangle->C = glm::vec3(
                    pyramidSide4CX[animationIndex],
                    std::stof(filecontent[startNum + 8]),
                    pyramidSide4CZ[animationIndex]);
                triangle->material.ambient = glm::vec3(
                    std::stof(filecontent[startNum + 10]),
                    std::stof(filecontent[startNum + 11]),
                    std::stof(filecontent[startNum + 12]));
                triangle->material.diffuse = glm::vec3(
                    std::stof(filecontent[startNum + 13]),
                    std::stof(filecontent[startNum + 14]),
                    std::stof(filecontent[startNum + 15]));
                triangle->material.specular = glm::vec3(
                    std::stof(filecontent[startNum + 16]),
                    std::stof(filecontent[startNum + 17]),
                    std::stof(filecontent[startNum + 18]));
                triangle->material.shininess = std::stof(filecontent[startNum + 19]);
                startNum += 20;
                scene.objects.push_back(triangle);
            }
        }

        // Light Initialization
        int lightNum = std::stoi(filecontent[startNum]);
        std::cout << "Start num: " << startNum << std::endl;
        std::cout << "Light w: " << std::stof(filecontent[startNum + 4]) << std::endl;
        for (int i = 0; i < lightNum; i++)
        {
            Light light;

            light.position = glm::vec4(
                std::stof(filecontent[startNum + 1]),  // x
                std::stof(filecontent[startNum + 2]),  // y
                std::stof(filecontent[startNum + 3]),  // z
                std::stof(filecontent[startNum + 4])); // w
            light.ambient = glm::vec3(
                std::stof(filecontent[startNum + 5]),  // r
                std::stof(filecontent[startNum + 6]),  // g
                std::stof(filecontent[startNum + 7])); // b
            light.diffuse = glm::vec3(
                std::stof(filecontent[startNum + 8]),   // r
                std::stof(filecontent[startNum + 9]),   // g
                std::stof(filecontent[startNum + 10])); // b
            light.specular = glm::vec3(
                std::stof(filecontent[startNum + 11]),  // r
                std::stof(filecontent[startNum + 12]),  // g
                std::stof(filecontent[startNum + 13])); // b
            light.constant = std::stof(filecontent[startNum + 14]);
            light.linear = std::stof(filecontent[startNum + 15]);
            light.quadratic = std::stof(filecontent[startNum + 16]);

            startNum += 16;
            scene.lights.push_back(light);
        }

        Image image(camera.imageWidth, camera.imageHeight);
        for (int y = 0; y < image.height; ++y)
        {
            for (int x = 0; x < image.width; ++x)
            {
                Ray ray = GetRayThruPixel(camera, x, image.height - y - 1);

                glm::vec3 color = RayTrace(ray, scene, camera, maxDepth);
                image.SetColor(x, y, color);
            }

            std::cout << "Row: " << std::setfill(' ') << std::setw(4) << (y + 1) << " / " << std::setfill(' ') << std::setw(4) << image.height << "\r" << std::flush;
        }
        std::cout << std::endl;

        std::string imageFileName = "frame" + std::to_string(animationIndex) + ".png"; // You might need to make this a full path if you are on Mac
        stbi_write_png(imageFileName.c_str(), image.width, image.height, 3, image.data.data(), 0);

        for (size_t i = 0; i < scene.objects.size(); ++i)
        {
            delete scene.objects[i];
        }
    }
    return 0;
}

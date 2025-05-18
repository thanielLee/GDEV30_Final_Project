/******************************************************************************
 * This demo draws a triangle by defining its vertices in 3 dimensions
 * (the 3rd dimension is currently ignored and is just set to 0).
 *
 * The drawing is accomplished by:
 * - Uploading the vertices to the GPU using a Vertex Buffer Object (VBO).
 * - Specifying the vertices' format using a Vertex Array Object (VAO).
 * - Using a GLSL shader program (consisting of a simple vertex shader and a
 *   simple fragment shader) to actually draw the vertices as a triangle.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <gdev.h>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

// change this to your desired window attributes
#define WINDOW_WIDTH  1600
#define WINDOW_HEIGHT 900
#define WINDOW_TITLE  "Hello Frieren"
GLFWwindow *pWindow;

GLuint barkTexture;
GLuint leafTexture;
GLuint groundTexture;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 lightPosition;
float lightHeight = 2.0f;
float lightSpeed = 1.0f;
// float lightBrightness = 1.0f;
float lightBrightness = 0.75f;
float specular = 128.0f;
// float specular = 16.0f;
float prev_time = 0.0f;
float current_time = 0.0f;
float cur = 0.0f;

float yaw = -90.0f;
float pitch = 0.0f;
float fov = 90.0f;

float lastX = WINDOW_WIDTH/2.0f;
float lastY = WINDOW_HEIGHT/2.0f;
bool firstMouse = true;


#define tree_width 0.15
#define bark_color 0.41f, 0.29f, 0.21f
#define leaf_color 0.51f, 0.61f, 0.41f 

float icosphere[] = {
    -0.0438109, 0.0708876, 0,  
    0.0438109, 0.0708876, 0,  
    0, 0.0438109, -0.0708876,  
    
    0.0438109, 0.0708876, 0,  
    -0.0438109, 0.0708876, 0,  
    0, 0.0438109, 0.0708876,  
    
    -0.0708876, 0, 0.0438109,  
    0, -0.0438109, 0.0708876,  
    0, 0.0438109, 0.0708876,  
    
    0, -0.0438109, 0.0708876,  
    0.0708876, 0, 0.0438109,  
    0, 0.0438109, 0.0708876,  
    
    0.0708876, 0, -0.0438109,  
    0, -0.0438109, -0.0708876,  
    0, 0.0438109, -0.0708876,  
    
    0, -0.0438109, -0.0708876,  
    -0.0708876, 0, -0.0438109,  
    0, 0.0438109, -0.0708876,  
    
    -0.0438109, -0.0708876, 0,  
    0.0438109, -0.0708876, 0,  
    0, -0.0438109, 0.0708876,  
    
    0.0438109, -0.0708876, 0,  
    -0.0438109, -0.0708876, 0,  
    0, -0.0438109, -0.0708876,  
    
    -0.0708876, 0, -0.0438109,  
    -0.0708876, 0, 0.0438109,  
    -0.0438109, 0.0708876, 0,  
    
    -0.0708876, 0, 0.0438109,  
    -0.0708876, 0, -0.0438109,  
    -0.0438109, -0.0708876, 0,  
    
    0.0708876, 0, 0.0438109,  
    0.0708876, 0, -0.0438109,  
    0.0438109, 0.0708876, 0,  
    
    0.0708876, 0, -0.0438109,  
    0.0708876, 0, 0.0438109,  
    0.0438109, -0.0708876, 0,  
    
    -0.0438109, 0.0708876, 0,  
    -0.0708876, 0, 0.0438109,  
    0, 0.0438109, 0.0708876,  
    
    0.0708876, 0, 0.0438109,  
    0.0438109, 0.0708876, 0,  
    0, 0.0438109, 0.0708876,  
    
    -0.0708876, 0, -0.0438109,  
    -0.0438109, 0.0708876, 0,  
    0, 0.0438109, -0.0708876,  
    
    0.0438109, 0.0708876, 0,  
    0.0708876, 0, -0.0438109,  
    0, 0.0438109, -0.0708876,  
    
    -0.0438109, -0.0708876, 0,  
    -0.0708876, 0, -0.0438109,  
    0, -0.0438109, -0.0708876,  
    
    0.0708876, 0, -0.0438109,  
    0.0438109, -0.0708876, 0,  
    0, -0.0438109, -0.0708876,  
    
    -0.0708876, 0, 0.0438109,  
    -0.0438109, -0.0708876, 0,  
    0, -0.0438109, 0.0708876,  
    
    0.0438109, -0.0708876, 0,  
    0.0708876, 0, 0.0438109,  
    0, -0.0438109, 0.0708876,  
    
};

float leaf_dip = -0.05f;
float vertices[] = 
{
    // first bark section
    // bottom face
    0.120000f, -1.000000f, 0.000000f, bark_color,
    0.060000f, -1.000000f, 0.103923f, bark_color,
    0.000000f, -1.000000f, 0.000000f, bark_color,

    0.060000f, -1.000000f, 0.103923f, bark_color,
    -0.060000f, -1.000000f, 0.103923f, bark_color,
    0.000000f, -1.000000f, 0.000000f, bark_color,

    -0.060000f, -1.000000f, 0.103923f, bark_color,
    -0.120000f, -1.000000f, 0.000000f, bark_color,
    0.000000f, -1.000000f, 0.000000f, bark_color,

    -0.120000f, -1.000000f, 0.000000f, bark_color,
    -0.060000f, -1.000000f, -0.103923f, bark_color,
    0.000000f, -1.000000f, 0.000000f, bark_color,

    -0.060000f, -1.000000f, -0.103923f, bark_color,
    0.060000f, -1.000000f, -0.103923f, bark_color,
    0.000000f, -1.000000f, 0.000000f, bark_color,

    0.060000f, -1.000000f, -0.103923f, bark_color,
    0.120000f, -1.000000f, 0.000000f, bark_color,
    0.000000f, -1.000000f, 0.000000f, bark_color,

    // no top face
    0.047500f, -0.500000f, 0.082272f, bark_color,
    0.060000f, -1.000000f, 0.103923f, bark_color,
    0.120000f, -1.000000f, 0.000000f, bark_color,

    0.095000f, -0.500000f, 0.000000f, bark_color,
    0.047500f, -0.500000f, 0.082272f, bark_color,
    0.120000f, -1.000000f, 0.000000f, bark_color,

    -0.047500f, -0.500000f, 0.082272f, bark_color,
    -0.060000f, -1.000000f, 0.103923f, bark_color,
    0.060000f, -1.000000f, 0.103923f, bark_color,

    0.047500f, -0.500000f, 0.082272f, bark_color,
    -0.047500f, -0.500000f, 0.082272f, bark_color,
    0.060000f, -1.000000f, 0.103923f, bark_color,

    -0.095000f, -0.500000f, 0.000000f, bark_color,
    -0.120000f, -1.000000f, 0.000000f, bark_color,
    -0.060000f, -1.000000f, 0.103923f, bark_color,

    -0.047500f, -0.500000f, 0.082272f, bark_color,
    -0.095000f, -0.500000f, 0.000000f, bark_color,
    -0.060000f, -1.000000f, 0.103923f, bark_color,

    -0.047500f, -0.500000f, -0.082272f, bark_color,
    -0.060000f, -1.000000f, -0.103923f, bark_color,
    -0.120000f, -1.000000f, 0.000000f, bark_color,

    -0.095000f, -0.500000f, 0.000000f, bark_color,
    -0.047500f, -0.500000f, -0.082272f, bark_color,
    -0.120000f, -1.000000f, 0.000000f, bark_color,

    0.047500f, -0.500000f, -0.082272f, bark_color,
    0.060000f, -1.000000f, -0.103923f, bark_color,
    -0.060000f, -1.000000f, -0.103923f, bark_color,

    -0.047500f, -0.500000f, -0.082272f, bark_color,
    0.047500f, -0.500000f, -0.082272f, bark_color,
    -0.060000f, -1.000000f, -0.103923f, bark_color,

    0.095000f, -0.500000f, 0.000000f, bark_color,
    0.120000f, -1.000000f, 0.000000f, bark_color,
    0.060000f, -1.000000f, -0.103923f, bark_color,

    0.047500f, -0.500000f, -0.082272f, bark_color,
    0.095000f, -0.500000f, 0.000000f, bark_color,
    0.060000f, -1.000000f, -0.103923f, bark_color,


    // first leaf section

    // 1
    0.095000f, -0.250000f, 0.000000f, leaf_color,
    0.047500f, -0.250000f, 0.082272f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,

    0.095000f, -0.250000f, 0.000000f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,
    0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,

    0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,
    0.047500f, -0.500000f, 0.082272f, leaf_color,
    0.095000f, -0.500000f, 0.000000f, leaf_color,

    0.095000f, -0.500000f, 0.000000f, leaf_color,
    0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,

    // 2
    0.047500f, -0.250000f, 0.082272f, leaf_color,
    -0.047500f, -0.250000f, 0.082272f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,

    0.047500f, -0.250000f, 0.082272f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,

    -0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,
    -0.047500f, -0.500000f, 0.082272f, leaf_color,
    0.047500f, -0.500000f, 0.082272f, leaf_color,

    0.047500f, -0.500000f, 0.082272f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,

    // 3
    -0.047500f, -0.250000f, 0.082272f, leaf_color,
    -0.095000f, -0.250000f, 0.000000f, leaf_color,
    -0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,

    -0.047500f, -0.250000f, 0.082272f, leaf_color,
    -0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,

    -0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,
    -0.095000f, -0.500000f, 0.000000f, leaf_color,
    -0.047500f, -0.500000f, 0.082272f, leaf_color,

    -0.047500f, -0.500000f, 0.082272f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,
    -0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,

    // 4
    -0.095000f, -0.250000f, 0.000000f, leaf_color,
    -0.047500f, -0.250000f, -0.082272f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,

    -0.095000f, -0.250000f, 0.000000f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,
    -0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,

    -0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,
    -0.047500f, -0.500000f, -0.082272f, leaf_color,
    -0.095000f, -0.500000f, 0.000000f, leaf_color,

    -0.095000f, -0.500000f, 0.000000f, leaf_color,
    -0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,

    // 5
    -0.047500f, -0.250000f, -0.082272f, leaf_color,
    0.047500f, -0.250000f, -0.082272f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,

    -0.047500f, -0.250000f, -0.082272f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,

    0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,
    0.047500f, -0.500000f, -0.082272f, leaf_color,
    -0.047500f, -0.500000f, -0.082272f, leaf_color,

    -0.047500f, -0.500000f, -0.082272f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,


    // 6
    0.047500f, -0.250000f, -0.082272f, leaf_color,
    0.095000f, -0.250000f, 0.000000f, leaf_color,
    0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,

    0.047500f, -0.250000f, -0.082272f, leaf_color,
    0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,

    0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,
    0.095000f, -0.500000f, 0.000000f, leaf_color,
    0.047500f, -0.500000f, -0.082272f, leaf_color,

    0.047500f, -0.500000f, -0.082272f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,
    0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,

    // second bark section
    0.037500f, -0.150000f, 0.064952f, bark_color,
    0.047500f, -0.250000f, 0.082272f, bark_color,
    0.095000f, -0.250000f, 0.000000f, bark_color,

    0.075000f, -0.150000f, 0.000000f, bark_color,
    0.037500f, -0.150000f, 0.064952f, bark_color,
    0.095000f, -0.250000f, 0.000000f, bark_color,

    -0.037500f, -0.150000f, 0.064952f, bark_color,
    -0.047500f, -0.250000f, 0.082272f, bark_color,
    0.047500f, -0.250000f, 0.082272f, bark_color,

    0.037500f, -0.150000f, 0.064952f, bark_color,
    -0.037500f, -0.150000f, 0.064952f, bark_color,
    0.047500f, -0.250000f, 0.082272f, bark_color,

    -0.075000f, -0.150000f, 0.000000f, bark_color,
    -0.095000f, -0.250000f, 0.000000f, bark_color,
    -0.047500f, -0.250000f, 0.082272f, bark_color,

    -0.037500f, -0.150000f, 0.064952f, bark_color,
    -0.075000f, -0.150000f, 0.000000f, bark_color,
    -0.047500f, -0.250000f, 0.082272f, bark_color,

    -0.037500f, -0.150000f, -0.064952f, bark_color,
    -0.047500f, -0.250000f, -0.082272f, bark_color,
    -0.095000f, -0.250000f, 0.000000f, bark_color,

    -0.075000f, -0.150000f, 0.000000f, bark_color,
    -0.037500f, -0.150000f, -0.064952f, bark_color,
    -0.095000f, -0.250000f, 0.000000f, bark_color,

    0.037500f, -0.150000f, -0.064952f, bark_color,
    0.047500f, -0.250000f, -0.082272f, bark_color,
    -0.047500f, -0.250000f, -0.082272f, bark_color,

    -0.037500f, -0.150000f, -0.064952f, bark_color,
    0.037500f, -0.150000f, -0.064952f, bark_color,
    -0.047500f, -0.250000f, -0.082272f, bark_color,

    0.075000f, -0.150000f, 0.000000f, bark_color,
    0.095000f, -0.250000f, 0.000000f, bark_color,
    0.047500f, -0.250000f, -0.082272f, bark_color,

    0.037500f, -0.150000f, -0.064952f, bark_color,
    0.075000f, -0.150000f, 0.000000f, bark_color,
    0.047500f, -0.250000f, -0.082272f, bark_color,


    // second leaf section
    // length = 0.5
    // 1
    0.075000f, 0.050000f, 0.000000f, leaf_color,
    0.037500f, 0.050000f, 0.064952f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,

    0.075000f, 0.050000f, 0.000000f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,
    0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,

    0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,
    0.037500f, -0.150000f, 0.064952f, leaf_color,
    0.075000f, -0.150000f, 0.000000f, leaf_color,

    0.075000f, -0.150000f, 0.000000f, leaf_color,
    0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,

    // 2
    0.037500f, 0.050000f, 0.064952f, leaf_color,
    -0.037500f, 0.050000f, 0.064952f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,

    0.037500f, 0.050000f, 0.064952f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,

    -0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,
    -0.037500f, -0.150000f, 0.064952f, leaf_color,
    0.037500f, -0.150000f, 0.064952f, leaf_color,

    0.037500f, -0.150000f, 0.064952f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,

    // 3
    -0.037500f, 0.050000f, 0.064952f, leaf_color,
    -0.075000f, 0.050000f, 0.000000f, leaf_color,
    -0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,

    -0.037500f, 0.050000f, 0.064952f, leaf_color,
    -0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,

    -0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,
    -0.075000f, -0.150000f, 0.000000f, leaf_color,
    -0.037500f, -0.150000f, 0.064952f, leaf_color,

    -0.037500f, -0.150000f, 0.064952f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,
    -0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,

    //4
    -0.075000f, 0.050000f, 0.000000f, leaf_color,
    -0.037500f, 0.050000f, -0.064952f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,

    -0.075000f, 0.050000f, 0.000000f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,
    -0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,

    -0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,
    -0.037500f, -0.150000f, -0.064952f, leaf_color,
    -0.075000f, -0.150000f, -0.000000f, leaf_color,

    -0.075000f, -0.150000f, -0.000000f, leaf_color,
    -0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,

    // 5
    -0.037500f, 0.050000f, -0.064952f, leaf_color,
    0.037500f, 0.050000f, -0.064952f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,

    -0.037500f, 0.050000f, -0.064952f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,

    0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,
    0.037500f, -0.150000f, -0.064952f, leaf_color,
    -0.037500f, -0.150000f, -0.064952f, leaf_color,

    -0.037500f, -0.150000f, -0.064952f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,


    // 6
    0.037500f, 0.050000f, -0.064952f, leaf_color,
    0.075000f, 0.050000f, 0.000000f, leaf_color,
    0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,

    0.037500f, 0.050000f, -0.064952f, leaf_color,
    0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,

    0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,
    0.075000f, -0.150000f, 0.000000f, leaf_color,
    0.037500f, -0.150000f, -0.064952f, leaf_color,

    0.037500f, -0.150000f, -0.064952f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,
    0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,


    // third bark section
    0.032500f, 0.120000f, 0.056292f, bark_color,
    0.037500f, 0.050000f, 0.064952f, bark_color,
    0.075000f, 0.050000f, 0.000000f, bark_color,

    0.065000f, 0.120000f, 0.000000f, bark_color,
    0.032500f, 0.120000f, 0.056292f, bark_color,
    0.075000f, 0.050000f, 0.000000f, bark_color,

    -0.032500f, 0.120000f, 0.056292f, bark_color,
    -0.037500f, 0.050000f, 0.064952f, bark_color,
    0.037500f, 0.050000f, 0.064952f, bark_color,

    0.032500f, 0.120000f, 0.056292f, bark_color,
    -0.032500f, 0.120000f, 0.056292f, bark_color,
    0.037500f, 0.050000f, 0.064952f, bark_color,

    -0.065000f, 0.120000f, 0.000000f, bark_color,
    -0.075000f, 0.050000f, 0.000000f, bark_color,
    -0.037500f, 0.050000f, 0.064952f, bark_color,

    -0.032500f, 0.120000f, 0.056292f, bark_color,
    -0.065000f, 0.120000f, 0.000000f, bark_color,
    -0.037500f, 0.050000f, 0.064952f, bark_color,

    -0.032500f, 0.120000f, -0.056292f, bark_color,
    -0.037500f, 0.050000f, -0.064952f, bark_color,
    -0.075000f, 0.050000f, 0.000000f, bark_color,

    -0.065000f, 0.120000f, 0.000000f, bark_color,
    -0.032500f, 0.120000f, -0.056292f, bark_color,
    -0.075000f, 0.050000f, 0.000000f, bark_color,

    0.032500f, 0.120000f, -0.056292f, bark_color,
    0.037500f, 0.050000f, -0.064952f, bark_color,
    -0.037500f, 0.050000f, -0.064952f, bark_color,

    -0.032500f, 0.120000f, -0.056292f, bark_color,
    0.032500f, 0.120000f, -0.056292f, bark_color,
    -0.037500f, 0.050000f, -0.064952f, bark_color,

    0.065000f, 0.120000f, 0.000000f, bark_color,
    0.075000f, 0.050000f, 0.000000f, bark_color,
    0.037500f, 0.050000f, -0.064952f, bark_color,

    0.032500f, 0.120000f, -0.056292f, bark_color,
    0.065000f, 0.120000f, 0.000000f, bark_color,
    0.037500f, 0.050000f, -0.064952f, bark_color,


    // top leaf
    // 1
    0.00000f, 0.40000f, 0.000000f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,
    0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,

    0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,
    0.032500f, 0.120000f, 0.056292f, leaf_color,
    0.065000f, 0.120000f, 0.000000f, leaf_color,

    0.065000f, 0.120000f, 0.000000f, leaf_color,
    0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,

    // 2
    0.00000f, 0.40000f, 0.000000f,leaf_color,
    -0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,

    -0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,
    -0.032500f, 0.120000f, 0.056292f, leaf_color,
    0.032500f, 0.120000f, 0.056292f, leaf_color,

    0.032500f, 0.120000f, 0.056292f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,
    -0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,

    // 3
    0.00000f, 0.40000f, 0.000000f, leaf_color,
    -0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,
    -0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,

    -0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,
    -0.065000f, 0.120000f, 0.000000f, leaf_color,
    -0.032500f, 0.120000f, 0.056292f, leaf_color,

    -0.032500f, 0.120000f, 0.056292f, leaf_color,
    -0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,
    -0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,

    // 4
    0.00000f, 0.40000f, 0.000000f, leaf_color,
    -0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,
    -0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,

    -0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,
    -0.032500f, 0.120000f, -0.056292f, leaf_color,
    -0.065000f, 0.120000f, 0.000000f, leaf_color,

    -0.065000f, 0.120000f, 0.000000f, leaf_color,
    -0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,
    -0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,

    // 5
    0.00000f, 0.40000f, 0.00000f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,
    -0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,

    0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,
    0.032500f, 0.120000f, -0.056292f, leaf_color,
    -0.032500f, 0.120000f, -0.056292f, leaf_color,

    -0.032500f, 0.120000f, -0.056292f, leaf_color,
    -0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,


    // 6
    0.00000f, 0.40000f, 0.00000f, leaf_color,
    0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,

    0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,
    0.065000f, 0.120000f, 0.000000f, leaf_color,
    0.032500f, 0.120000f, -0.056292f, leaf_color,

    0.032500f, 0.120000f, -0.056292f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,
    0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,


};

const std::vector<float> tree_data ={
    // position (x, y, z) color (r, g, b)

    // first bark section
    // bottom face
    0.120000f, -1.000000f, 0.000000f, bark_color,
    0.060000f, -1.000000f, 0.103923f, bark_color,
    0.000000f, -1.000000f, 0.000000f, bark_color,

    0.060000f, -1.000000f, 0.103923f, bark_color,
    -0.060000f, -1.000000f, 0.103923f, bark_color,
    0.000000f, -1.000000f, 0.000000f, bark_color,

    -0.060000f, -1.000000f, 0.103923f, bark_color,
    -0.120000f, -1.000000f, 0.000000f, bark_color,
    0.000000f, -1.000000f, 0.000000f, bark_color,

    -0.120000f, -1.000000f, 0.000000f, bark_color,
    -0.060000f, -1.000000f, -0.103923f, bark_color,
    0.000000f, -1.000000f, 0.000000f, bark_color,

    -0.060000f, -1.000000f, -0.103923f, bark_color,
    0.060000f, -1.000000f, -0.103923f, bark_color,
    0.000000f, -1.000000f, 0.000000f, bark_color,

    0.060000f, -1.000000f, -0.103923f, bark_color,
    0.120000f, -1.000000f, 0.000000f, bark_color,
    0.000000f, -1.000000f, 0.000000f, bark_color,

    // no top face
    0.047500f, -0.500000f, 0.082272f, bark_color,
    0.060000f, -1.000000f, 0.103923f, bark_color,
    0.120000f, -1.000000f, 0.000000f, bark_color,

    0.095000f, -0.500000f, 0.000000f, bark_color,
    0.047500f, -0.500000f, 0.082272f, bark_color,
    0.120000f, -1.000000f, 0.000000f, bark_color,

    -0.047500f, -0.500000f, 0.082272f, bark_color,
    -0.060000f, -1.000000f, 0.103923f, bark_color,
    0.060000f, -1.000000f, 0.103923f, bark_color,

    0.047500f, -0.500000f, 0.082272f, bark_color,
    -0.047500f, -0.500000f, 0.082272f, bark_color,
    0.060000f, -1.000000f, 0.103923f, bark_color,

    -0.095000f, -0.500000f, 0.000000f, bark_color,
    -0.120000f, -1.000000f, 0.000000f, bark_color,
    -0.060000f, -1.000000f, 0.103923f, bark_color,

    -0.047500f, -0.500000f, 0.082272f, bark_color,
    -0.095000f, -0.500000f, 0.000000f, bark_color,
    -0.060000f, -1.000000f, 0.103923f, bark_color,

    -0.047500f, -0.500000f, -0.082272f, bark_color,
    -0.060000f, -1.000000f, -0.103923f, bark_color,
    -0.120000f, -1.000000f, 0.000000f, bark_color,

    -0.095000f, -0.500000f, 0.000000f, bark_color,
    -0.047500f, -0.500000f, -0.082272f, bark_color,
    -0.120000f, -1.000000f, 0.000000f, bark_color,

    0.047500f, -0.500000f, -0.082272f, bark_color,
    0.060000f, -1.000000f, -0.103923f, bark_color,
    -0.060000f, -1.000000f, -0.103923f, bark_color,

    -0.047500f, -0.500000f, -0.082272f, bark_color,
    0.047500f, -0.500000f, -0.082272f, bark_color,
    -0.060000f, -1.000000f, -0.103923f, bark_color,

    0.095000f, -0.500000f, 0.000000f, bark_color,
    0.120000f, -1.000000f, 0.000000f, bark_color,
    0.060000f, -1.000000f, -0.103923f, bark_color,

    0.047500f, -0.500000f, -0.082272f, bark_color,
    0.095000f, -0.500000f, 0.000000f, bark_color,
    0.060000f, -1.000000f, -0.103923f, bark_color,


    // first leaf section

    // 1
    0.095000f, -0.250000f, 0.000000f, leaf_color,
    0.047500f, -0.250000f, 0.082272f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,

    0.095000f, -0.250000f, 0.000000f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,
    0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,

    0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,
    0.047500f, -0.500000f, 0.082272f, leaf_color,
    0.095000f, -0.500000f, 0.000000f, leaf_color,

    0.095000f, -0.500000f, 0.000000f, leaf_color,
    0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,

    // 2
    0.047500f, -0.250000f, 0.082272f, leaf_color,
    -0.047500f, -0.250000f, 0.082272f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,

    0.047500f, -0.250000f, 0.082272f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,

    -0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,
    -0.047500f, -0.500000f, 0.082272f, leaf_color,
    0.047500f, -0.500000f, 0.082272f, leaf_color,

    0.047500f, -0.500000f, 0.082272f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,

    // 3
    -0.047500f, -0.250000f, 0.082272f, leaf_color,
    -0.095000f, -0.250000f, 0.000000f, leaf_color,
    -0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,

    -0.047500f, -0.250000f, 0.082272f, leaf_color,
    -0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,

    -0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,
    -0.095000f, -0.500000f, 0.000000f, leaf_color,
    -0.047500f, -0.500000f, 0.082272f, leaf_color,

    -0.047500f, -0.500000f, 0.082272f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, 0.649519f, leaf_color,
    -0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,

    // 4
    -0.095000f, -0.250000f, 0.000000f, leaf_color,
    -0.047500f, -0.250000f, -0.082272f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,

    -0.095000f, -0.250000f, 0.000000f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,
    -0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,

    -0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,
    -0.047500f, -0.500000f, -0.082272f, leaf_color,
    -0.095000f, -0.500000f, 0.000000f, leaf_color,

    -0.095000f, -0.500000f, 0.000000f, leaf_color,
    -0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,

    // 5
    -0.047500f, -0.250000f, -0.082272f, leaf_color,
    0.047500f, -0.250000f, -0.082272f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,

    -0.047500f, -0.250000f, -0.082272f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,

    0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,
    0.047500f, -0.500000f, -0.082272f, leaf_color,
    -0.047500f, -0.500000f, -0.082272f, leaf_color,

    -0.047500f, -0.500000f, -0.082272f, leaf_color,
    -0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,


    // 6
    0.047500f, -0.250000f, -0.082272f, leaf_color,
    0.095000f, -0.250000f, 0.000000f, leaf_color,
    0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,

    0.047500f, -0.250000f, -0.082272f, leaf_color,
    0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,

    0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,
    0.095000f, -0.500000f, 0.000000f, leaf_color,
    0.047500f, -0.500000f, -0.082272f, leaf_color,

    0.047500f, -0.500000f, -0.082272f, leaf_color,
    0.375000f, -0.500000f+leaf_dip, -0.649519f, leaf_color,
    0.750000f, -0.500000f+leaf_dip, 0.000000f, leaf_color,

    // second bark section
    0.037500f, -0.150000f, 0.064952f, bark_color,
    0.047500f, -0.250000f, 0.082272f, bark_color,
    0.095000f, -0.250000f, 0.000000f, bark_color,

    0.075000f, -0.150000f, 0.000000f, bark_color,
    0.037500f, -0.150000f, 0.064952f, bark_color,
    0.095000f, -0.250000f, 0.000000f, bark_color,

    -0.037500f, -0.150000f, 0.064952f, bark_color,
    -0.047500f, -0.250000f, 0.082272f, bark_color,
    0.047500f, -0.250000f, 0.082272f, bark_color,

    0.037500f, -0.150000f, 0.064952f, bark_color,
    -0.037500f, -0.150000f, 0.064952f, bark_color,
    0.047500f, -0.250000f, 0.082272f, bark_color,

    -0.075000f, -0.150000f, 0.000000f, bark_color,
    -0.095000f, -0.250000f, 0.000000f, bark_color,
    -0.047500f, -0.250000f, 0.082272f, bark_color,

    -0.037500f, -0.150000f, 0.064952f, bark_color,
    -0.075000f, -0.150000f, 0.000000f, bark_color,
    -0.047500f, -0.250000f, 0.082272f, bark_color,

    -0.037500f, -0.150000f, -0.064952f, bark_color,
    -0.047500f, -0.250000f, -0.082272f, bark_color,
    -0.095000f, -0.250000f, 0.000000f, bark_color,

    -0.075000f, -0.150000f, 0.000000f, bark_color,
    -0.037500f, -0.150000f, -0.064952f, bark_color,
    -0.095000f, -0.250000f, 0.000000f, bark_color,

    0.037500f, -0.150000f, -0.064952f, bark_color,
    0.047500f, -0.250000f, -0.082272f, bark_color,
    -0.047500f, -0.250000f, -0.082272f, bark_color,

    -0.037500f, -0.150000f, -0.064952f, bark_color,
    0.037500f, -0.150000f, -0.064952f, bark_color,
    -0.047500f, -0.250000f, -0.082272f, bark_color,

    0.075000f, -0.150000f, 0.000000f, bark_color,
    0.095000f, -0.250000f, 0.000000f, bark_color,
    0.047500f, -0.250000f, -0.082272f, bark_color,

    0.037500f, -0.150000f, -0.064952f, bark_color,
    0.075000f, -0.150000f, 0.000000f, bark_color,
    0.047500f, -0.250000f, -0.082272f, bark_color,


    // second leaf section
    // length = 0.5
    // 1
    0.075000f, 0.050000f, 0.000000f, leaf_color,
    0.037500f, 0.050000f, 0.064952f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,

    0.075000f, 0.050000f, 0.000000f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,
    0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,

    0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,
    0.037500f, -0.150000f, 0.064952f, leaf_color,
    0.075000f, -0.150000f, 0.000000f, leaf_color,

    0.075000f, -0.150000f, 0.000000f, leaf_color,
    0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,

    // 2
    0.037500f, 0.050000f, 0.064952f, leaf_color,
    -0.037500f, 0.050000f, 0.064952f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,

    0.037500f, 0.050000f, 0.064952f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,

    -0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,
    -0.037500f, -0.150000f, 0.064952f, leaf_color,
    0.037500f, -0.150000f, 0.064952f, leaf_color,

    0.037500f, -0.150000f, 0.064952f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,

    // 3
    -0.037500f, 0.050000f, 0.064952f, leaf_color,
    -0.075000f, 0.050000f, 0.000000f, leaf_color,
    -0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,

    -0.037500f, 0.050000f, 0.064952f, leaf_color,
    -0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,

    -0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,
    -0.075000f, -0.150000f, 0.000000f, leaf_color,
    -0.037500f, -0.150000f, 0.064952f, leaf_color,

    -0.037500f, -0.150000f, 0.064952f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, 0.433013f, leaf_color,
    -0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,

    //4
    -0.075000f, 0.050000f, 0.000000f, leaf_color,
    -0.037500f, 0.050000f, -0.064952f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,

    -0.075000f, 0.050000f, 0.000000f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,
    -0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,

    -0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,
    -0.037500f, -0.150000f, -0.064952f, leaf_color,
    -0.075000f, -0.150000f, -0.000000f, leaf_color,

    -0.075000f, -0.150000f, -0.000000f, leaf_color,
    -0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,

    // 5
    -0.037500f, 0.050000f, -0.064952f, leaf_color,
    0.037500f, 0.050000f, -0.064952f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,

    -0.037500f, 0.050000f, -0.064952f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,

    0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,
    0.037500f, -0.150000f, -0.064952f, leaf_color,
    -0.037500f, -0.150000f, -0.064952f, leaf_color,

    -0.037500f, -0.150000f, -0.064952f, leaf_color,
    -0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,


    // 6
    0.037500f, 0.050000f, -0.064952f, leaf_color,
    0.075000f, 0.050000f, 0.000000f, leaf_color,
    0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,

    0.037500f, 0.050000f, -0.064952f, leaf_color,
    0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,

    0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,
    0.075000f, -0.150000f, 0.000000f, leaf_color,
    0.037500f, -0.150000f, -0.064952f, leaf_color,

    0.037500f, -0.150000f, -0.064952f, leaf_color,
    0.250000f, -0.150000f+leaf_dip, -0.433013f, leaf_color,
    0.500000f, -0.150000f+leaf_dip, 0.000000f, leaf_color,


    // third bark section
    0.032500f, 0.120000f, 0.056292f, bark_color,
    0.037500f, 0.050000f, 0.064952f, bark_color,
    0.075000f, 0.050000f, 0.000000f, bark_color,

    0.065000f, 0.120000f, 0.000000f, bark_color,
    0.032500f, 0.120000f, 0.056292f, bark_color,
    0.075000f, 0.050000f, 0.000000f, bark_color,

    -0.032500f, 0.120000f, 0.056292f, bark_color,
    -0.037500f, 0.050000f, 0.064952f, bark_color,
    0.037500f, 0.050000f, 0.064952f, bark_color,

    0.032500f, 0.120000f, 0.056292f, bark_color,
    -0.032500f, 0.120000f, 0.056292f, bark_color,
    0.037500f, 0.050000f, 0.064952f, bark_color,

    -0.065000f, 0.120000f, 0.000000f, bark_color,
    -0.075000f, 0.050000f, 0.000000f, bark_color,
    -0.037500f, 0.050000f, 0.064952f, bark_color,

    -0.032500f, 0.120000f, 0.056292f, bark_color,
    -0.065000f, 0.120000f, 0.000000f, bark_color,
    -0.037500f, 0.050000f, 0.064952f, bark_color,

    -0.032500f, 0.120000f, -0.056292f, bark_color,
    -0.037500f, 0.050000f, -0.064952f, bark_color,
    -0.075000f, 0.050000f, 0.000000f, bark_color,

    -0.065000f, 0.120000f, 0.000000f, bark_color,
    -0.032500f, 0.120000f, -0.056292f, bark_color,
    -0.075000f, 0.050000f, 0.000000f, bark_color,

    0.032500f, 0.120000f, -0.056292f, bark_color,
    0.037500f, 0.050000f, -0.064952f, bark_color,
    -0.037500f, 0.050000f, -0.064952f, bark_color,

    -0.032500f, 0.120000f, -0.056292f, bark_color,
    0.032500f, 0.120000f, -0.056292f, bark_color,
    -0.037500f, 0.050000f, -0.064952f, bark_color,

    0.065000f, 0.120000f, 0.000000f, bark_color,
    0.075000f, 0.050000f, 0.000000f, bark_color,
    0.037500f, 0.050000f, -0.064952f, bark_color,

    0.032500f, 0.120000f, -0.056292f, bark_color,
    0.065000f, 0.120000f, 0.000000f, bark_color,
    0.037500f, 0.050000f, -0.064952f, bark_color,


    // top leaf
    // 1
    0.00000f, 0.40000f, 0.000000f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,
    0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,

    0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,
    0.032500f, 0.120000f, 0.056292f, leaf_color,
    0.065000f, 0.120000f, 0.000000f, leaf_color,

    0.065000f, 0.120000f, 0.000000f, leaf_color,
    0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,

    // 2
    0.00000f, 0.40000f, 0.000000f,leaf_color,
    -0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,

    -0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,
    -0.032500f, 0.120000f, 0.056292f, leaf_color,
    0.032500f, 0.120000f, 0.056292f, leaf_color,

    0.032500f, 0.120000f, 0.056292f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,
    -0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,

    // 3
    0.00000f, 0.40000f, 0.000000f, leaf_color,
    -0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,
    -0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,

    -0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,
    -0.065000f, 0.120000f, 0.000000f, leaf_color,
    -0.032500f, 0.120000f, 0.056292f, leaf_color,

    -0.032500f, 0.120000f, 0.056292f, leaf_color,
    -0.150000f, 0.120000f+leaf_dip, 0.259808f, leaf_color,
    -0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,

    // 4
    0.00000f, 0.40000f, 0.000000f, leaf_color,
    -0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,
    -0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,

    -0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,
    -0.032500f, 0.120000f, -0.056292f, leaf_color,
    -0.065000f, 0.120000f, 0.000000f, leaf_color,

    -0.065000f, 0.120000f, 0.000000f, leaf_color,
    -0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,
    -0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,

    // 5
    0.00000f, 0.40000f, 0.00000f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,
    -0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,

    0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,
    0.032500f, 0.120000f, -0.056292f, leaf_color,
    -0.032500f, 0.120000f, -0.056292f, leaf_color,

    -0.032500f, 0.120000f, -0.056292f, leaf_color,
    -0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,


    // 6
    0.00000f, 0.40000f, 0.00000f, leaf_color,
    0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,

    0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,
    0.065000f, 0.120000f, 0.000000f, leaf_color,
    0.032500f, 0.120000f, -0.056292f, leaf_color,

    0.032500f, 0.120000f, -0.056292f, leaf_color,
    0.150000f, 0.120000f+leaf_dip, -0.259808f, leaf_color,
    0.300000f, 0.120000f+leaf_dip, 0.000000f, leaf_color,
};

std::vector<float> generate_tree(const glm::vec3& origin, float scale = 1.0f) {
    std::vector<float> vertices;

    for (size_t i = 0; i < tree_data.size(); i += 6) {
        float y = tree_data[i+1];

        // keep base at original y level
        float scaled_y = (y - (-1.0f)) * scale + (-1.0f); // pivot around y = -1

        vertices.push_back(tree_data[i] * scale + origin.x);
        vertices.push_back(scaled_y + origin.y);
        vertices.push_back(tree_data[i+2] * scale + origin.z);

        vertices.push_back(tree_data[i+3]);
        vertices.push_back(tree_data[i+4]);
        vertices.push_back(tree_data[i+5]);

    }

    return vertices;
}

std::vector<float> final_vertices;

float bounding_box[] = 
{
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,

    -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

    -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,

    1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

    1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 
    1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,

    1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,

    1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 

    -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,

    1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
};

float ground_data[] = {
    10.00f, -1.00f, 10.00f, 1.0f, 1.0f, 1.0f,
    10.00f, -1.00f, -10.00f, 1.0f, 1.0f, 1.0f,
    -10.00f, -1.00f, -10.00f, 1.0f, 1.0f, 1.0f,

    -10.00f, -1.00f, -10.00f, 1.0f, 1.0f, 1.0f,
    -10.00f, -1.00f, 10.00f, 1.0f, 1.0f, 1.0f,
    10.00f, -1.00f, 10.00f, 1.0f, 1.0f, 1.0f,
};


// define OpenGL object IDs to represent the vertex array and the shader program in the GPU
GLuint vao;         // vertex array object (stores the render state for our vertex array)
GLuint vbo;         // vertex buffer object (reserves GPU memory for our vertex array)
GLuint shader;      // combined vertex and fragment shader
GLuint bounding_box_vbo;
GLuint bounding_box_vao;
GLuint ico_sphere_vbo;
GLuint ico_sphere_vao;
GLuint terrain_vbo;
GLuint terrain_vao;

// called by the main function to do initial setup, such as uploading vertex
// arrays, shader programs, etc.; returns true if successful, false otherwise
bool setup()
{
    // generate the VAO and VBO objects and store their IDs in vao and vbo, respectively
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &bounding_box_vao);
    glGenBuffers(1, &bounding_box_vbo);
    glGenVertexArrays(1, &ico_sphere_vao);
    glGenBuffers(1, &ico_sphere_vbo);
    glGenVertexArrays(1, &terrain_vao);
    glGenBuffers(1, &terrain_vbo);

    // bind the newly-created VAO to make it the current one that OpenGL will apply state changes to
    glBindVertexArray(vao);

    // upload our vertex array data to the newly-created VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, final_vertices.size() * sizeof(float), final_vertices.data(), GL_STATIC_DRAW);

    // on the VAO, register the current VBO with the following vertex attribute layout:
    // - layout location 0...
    // - ... shall consist of 3 GL_FLOATs (corresponding to x, y, and z coordinates)
    // - ... its values will NOT be normalized (GL_FALSE)
    // - ... the stride length is the number of bytes of all 3 floats of each vertex (hence, 3 * sizeof(float))
    // - ... and we start at the beginning of the array (hence, (void*) 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) (6 * sizeof(float)));
    // enable the newly-created layout location 0;
    // this shall be used by our vertex shader to read the vertex's x, y, and z
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(bounding_box_vao);

    glBindBuffer(GL_ARRAY_BUFFER, bounding_box_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bounding_box), bounding_box, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(ico_sphere_vao);

    glBindBuffer(GL_ARRAY_BUFFER, ico_sphere_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(icosphere), icosphere, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);

    glEnableVertexAttribArray(0);

    glBindVertexArray(terrain_vao);

    glBindBuffer(GL_ARRAY_BUFFER, terrain_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ground_data), ground_data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // important: if you have more vertex arrays to draw, make sure you separately define them
    // with unique VAO and VBO IDs, and follow the same process above to upload them to the GPU

    // loading textures
    barkTexture = gdevLoadTexture("bark_gray.jpg", GL_REPEAT, true, true);
    if (! barkTexture) return false;
    leafTexture = gdevLoadTexture("leaf_gray.jpg", GL_REPEAT, true, true);
    if (! leafTexture) return false;
    groundTexture = gdevLoadTexture("grass_tex.jpg", GL_REPEAT, true, true);
    if (! groundTexture) return false;
    

    // load our shader program
    shader = gdevLoadShader("project.vs", "project.fs");
    if (! shader)
        return false;

    return true;
}

glm::vec3 calculate_normal(glm::vec3 a, glm::vec3 b){
    return glm::normalize(cross(a, b));
}

// called by the main function to do rendering per frame
void render()
{
    // clear the whole frame
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black
    // glClearColor(0.05f, 0.05f, 0.05f, 1.0f); // very dark gray
    // glClearColor(0.02f, 0.02f, 0.08f, 1.0f); // dark blue
    // glClearColor(0.03f, 0.06f, 0.05f, 1.0f); // dark desaturated green
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // sky blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // using our shader program...
    glUseProgram(shader);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST); // enable OpenGL's hidden surface removal
    glm::mat4 projview;
    glm::mat4 view = glm::mat4(1.0f);
    projview = glm::perspective(glm::radians(fov),
                            (float) WINDOW_WIDTH / WINDOW_HEIGHT,
                            0.1f,
                            100.0f);


    glm::vec3 direction = cameraPos + cameraFront;
    float t = glfwGetTime();
    prev_time = current_time;
    current_time = t;
    cur += lightSpeed * (current_time-prev_time);

    // float clamp_t = ((sin(t) + 1.0f)/2.0f);
    float clamp_t = 1.0f;
    // float rot_speed = 50.0f;
    float rot_speed = 0.0f;


    glm::vec2 circ = glm::normalize(lightSpeed * glm::vec2(cos(cur/4.0f), sin(cur/4.0f)));
    lightPosition = glm::vec3(3.0f * circ.x, 0, 3.0f * circ.y);


    glUniform1i(glGetUniformLocation(shader, "is_light"), 0.0f);
    glUniform3f(glGetUniformLocation(shader, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
    glUniform3f(glGetUniformLocation(shader, "lightColor"), lightBrightness, lightBrightness, lightBrightness);
    glUniform1f(glGetUniformLocation(shader, "s"), specular);


    glUniform3f(glGetUniformLocation(shader, "eye"), cameraPos.x, cameraPos.y, cameraPos.z);


    projview *= glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);;
    
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(t * rot_speed),
                                glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::mat4 prev_model = model;
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f) + glm::vec3(1.0f, 1.0f, 1.0f) * clamp_t);
    
    glm::mat4 normalM;
    normalM = glm::transpose(glm::inverse(model));

    glUniformMatrix4fv(glGetUniformLocation(shader, "projview"),
                        1, GL_FALSE, glm::value_ptr(projview));
    

    glUniformMatrix4fv(glGetUniformLocation(shader, "model"),
                        1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix4fv(glGetUniformLocation(shader, "normalM"),
                        1, GL_FALSE, glm::value_ptr(normalM));    

    // set the active texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, barkTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, leafTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, groundTexture);

    // then connect each texture unit to a sampler2D in the fragment shader
    glUniform1i(glGetUniformLocation(shader,"barkTex"), 0);
    glUniform1i(glGetUniformLocation(shader,"leafTex"), 1);
    glUniform1i(glGetUniformLocation(shader,"groundTex"), 2);
    
    
    // ... draw our triangles
    glBindVertexArray(vao);
    glUniform1f(glGetUniformLocation(shader, "is_border"), 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, (final_vertices.size() * sizeof(float)) / (9 * sizeof(float)));




    glUniform1f(glGetUniformLocation(shader, "is_border"), 1.0f);
    glBindVertexArray(bounding_box_vao);
    glDrawArrays(GL_LINES, 0, sizeof(bounding_box) / (6 * sizeof(float)));


    glUniform1f(glGetUniformLocation(shader, "is_border"), 2.0f);
    glBindVertexArray(terrain_vbo);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(ground_data) / (6 * sizeof(float)));

}


void processInput(GLFWwindow *pWindow, float deltaTime) {
    float cameraSpeed = 0.5f * deltaTime;

    if (glfwGetKey(pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cameraSpeed *= 2.0f;
    }

    if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraFront * cameraSpeed;
    } 
    
    if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos += -cameraFront * cameraSpeed;
    } 

    if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += glm::cross(cameraFront, cameraUp) * cameraSpeed;
    }

    if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos += -glm::cross(cameraFront, cameraUp) * cameraSpeed;
    }

    if (glfwGetKey(pWindow, GLFW_KEY_T) == GLFW_PRESS) {
        lightSpeed += 0.01f;
    }

    if (glfwGetKey(pWindow, GLFW_KEY_G) == GLFW_PRESS) {
        lightSpeed -= 0.01f;
        lightSpeed = std::max(lightSpeed, 0.01f);
    }

    if (glfwGetKey(pWindow, GLFW_KEY_Y) == GLFW_PRESS) {
        lightBrightness += 0.01f;
        lightBrightness = std::min(1.0f, lightBrightness);
    }

    if (glfwGetKey(pWindow, GLFW_KEY_H) == GLFW_PRESS) {
        lightBrightness -= 0.01f;
        lightBrightness = std::max(-0.0f, lightBrightness);
    }

    if (glfwGetKey(pWindow, GLFW_KEY_U) == GLFW_PRESS) {
        specular += 2.0f;
        specular = std::min(2048.0f, specular);
    }

    if (glfwGetKey(pWindow, GLFW_KEY_J) == GLFW_PRESS) {
        specular -= 2.0f;
        specular = std::max(0.01f, specular);
    }


}

/*****************************************************************************/

// handler called by GLFW when there is a keyboard event
void handleKeys(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    // pressing Esc closes the window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(pWindow, GL_TRUE);
}

// handler called by GLFW when the window is resized
void handleResize(GLFWwindow* pWindow, int width, int height)
{
    // tell OpenGL to do its drawing within the entire "client area" (area within the borders) of the window
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* pWindow, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos-lastX;
    float yoffset = lastY - ypos; // reverse cause y is reversed in window space;

    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    glm::vec3 cam_dir;
    cam_dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cam_dir.y = sin(glm::radians(pitch));
    cam_dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(cam_dir);
    
}

void scroll_callback(GLFWwindow *pWindow, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f) {
        fov = 1.0f;
    }
    if (fov > 90.0f) {
        fov = 90.0f;
    }

}

struct cmp {
    bool operator()(glm::vec3 a, glm::vec3 b) const {
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        return a.z < b.z;
    }
};

auto cmp1 = [](const glm::vec3& a, const glm::vec3& b) {
    if (a.x != b.x) return a.x < b.x;
    if (a.y != b.y) return a.y < b.y;
    return a.z < b.z;
};

std::map<glm::vec3, std::set<glm::vec3, cmp>, decltype(cmp1)> test(cmp1);
std::map<glm::vec3, glm::vec3, decltype(cmp1)> point_to_normal(cmp1);

// main function
int main(int argc, char** argv)
{
    // ADD OBJECTS
    std::vector<float> objects; 
    // std::vector<float> tree1 = generate_tree({0.0f, 0.0f, 0.0f}, 0.75);
    std::vector<float> tree2 = generate_tree({1.0f, 0.0f, 1.0f}, 0.4f);

    // objects.insert(objects.end(), tree1.begin(), tree1.end());
    objects.insert(objects.end(), tree2.begin(), tree2.end());

    // generate trees at random spots (keeps it between -1, 1)
    // srand(time(0));
    // srand(1747578212); // random generated seed
    // srand(1747578678);
    srand(1747578808);
    // std::cout << "seed: " << time(0) << std::endl;
    int num_trees = 24;
    int xz_range = 20; // 0 to 20 divided by 20 // higher number, more precise floats
    int y_range = 5;
    int s_range = 10;
    for (int i = 0; i < num_trees; i++) {
        // float x = (rand() % (xz_range*2) - xz_range) / xz_range;
        float x = float(rand() % xz_range*2 - xz_range)/float(xz_range)*2;
        float z = float(rand() % xz_range*2 - xz_range)/float(xz_range)*2;
        float s = float(rand() % s_range+1)/float(s_range*10) + 0.2f; // 0.2 to 0.3 inclusive
       
        // std::cout << "x:" << x << ", z:" << z << ", s:" << s << std::endl;
       
        std::vector<float> tree = generate_tree({x, 0.0f, z}, s);
        objects.insert(objects.end(), tree.begin(), tree.end());
    }


    
    for (size_t i = 0; i < objects.size(); i += 6*3) { // 6 floats for 1 vertex, 3 vertex for 1 triangle
        glm::vec3 a = glm::vec3(objects[i+6]-objects[i+0], objects[i+7]-objects[i+1],objects[i+8]-objects[i+2]);
        glm::vec3 b = glm::vec3(objects[i+12]-objects[i+0], objects[i+13]-objects[i+1],objects[i+14]-objects[i+2]);

        glm::vec3 normal = calculate_normal(a, b);
        
        // std::cout << "(" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;

        std::vector<float> cur_triangle = {
            objects[i+0], objects[i+1], objects[i+2], objects[i+3], objects[i+4], objects[i+5], normal.x, normal.y, normal.z, 
            objects[i+6], objects[i+7], objects[i+8], objects[i+9], objects[i+10], objects[i+11], normal.x, normal.y, normal.z,
            objects[i+12], objects[i+13], objects[i+14], objects[i+15], objects[i+16], objects[i+17], normal.x, normal.y, normal.z,
        };

        final_vertices.insert(final_vertices.end(), cur_triangle.begin(), cur_triangle.end());
        
    }

    // adding vertices to vertex buffer
    // for (int i = 0; i < (sizeof(vertices)/sizeof(float)/6.0f/3.0f); i++) {
    //     int cur = i*6*3;

    //     glm::vec3 a = glm::vec3(vertices[cur+6]-vertices[cur+0], vertices[cur+7]-vertices[cur+1],vertices[cur+8]-vertices[cur+2]);
    //     glm::vec3 b = glm::vec3(vertices[cur+12]-vertices[cur+0], vertices[cur+13]-vertices[cur+1],vertices[cur+14]-vertices[cur+2]);

        
    //     glm::vec3 normal = calculate_normal(a, b);

    //     std::vector<float> cur_triangle = {
    //         vertices[cur+0], vertices[cur+1], vertices[cur+2], vertices[cur+3], vertices[cur+4], vertices[cur+5], normal.x, normal.y, normal.z, 
    //         vertices[cur+6], vertices[cur+7], vertices[cur+8], vertices[cur+9], vertices[cur+10], vertices[cur+11], normal.x, normal.y, normal.z,
    //         vertices[cur+12], vertices[cur+13], vertices[cur+14], vertices[cur+15], vertices[cur+16], vertices[cur+17], normal.x, normal.y, normal.z,
    //     };

    //     final_vertices.insert(final_vertices.end(), cur_triangle.begin(), cur_triangle.end());

    // }


    // initialize GLFW and ask for OpenGL 3.3 core
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // create a GLFW window with the specified width, height, and title
    pWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);


    if (! pWindow)
    {
        // gracefully terminate if we cannot create the window
        std::cout << "Cannot create the GLFW window.\n";
        glfwTerminate();
        return -1;
    }

    // make the window the current context of subsequent OpenGL commands,
    // and enable vertical sync and aspect-ratio correction on the GLFW window
    glfwMakeContextCurrent(pWindow);
    glfwSwapInterval(1);
    glfwSetWindowAspectRatio(pWindow, WINDOW_WIDTH, WINDOW_HEIGHT);

    // set up callback functions to handle window system events
    glfwSetKeyCallback(pWindow, handleKeys);
    glfwSetFramebufferSizeCallback(pWindow, handleResize);

    // don't miss any momentary keypresses
    glfwSetInputMode(pWindow, GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetCursorPosCallback(pWindow, mouse_callback);

    glfwSetScrollCallback(pWindow, scroll_callback);

    // initialize GLAD, which acts as a library loader for the current OS's native OpenGL library
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    float delta;
    float last_frame = 0.0f;
    
    // if our initial setup is successful...
    if (setup())
    {
        // do rendering in a loop until the user closes the window
        while (! glfwWindowShouldClose(pWindow))
        {
            // render our next frame
            // (by default, GLFW uses double-buffering with a front and back buffer;
            // all drawing goes to the back buffer, so the frame does not get shown yet)
            float current_frame = glfwGetTime();
            delta = current_frame - last_frame;
            last_frame = current_frame;

            processInput(pWindow, delta);
            render();


            // swap the GLFW front and back buffers to show the next frame
            glfwSwapBuffers(pWindow);

            // process any window events (such as moving, resizing, keyboard presses, etc.)
            glfwPollEvents();
        }
    }

    // gracefully terminate the program
    glfwTerminate();
    return 0;
}

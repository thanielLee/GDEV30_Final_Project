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
#define WINDOW_TITLE  "Arborial Skies"
GLFWwindow *pWindow;

GLuint barkTexture;
GLuint leafTexture;
GLuint groundTexture;
GLuint wallTexture;
GLuint frameTexture;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 lightPosition(0.0f, 10.0f, 0.0f);
float lightHeight = 2.0f;
float lightSpeed = 1.0f;
float sunDist = 8.0f;
// float lightBrightness = 1.0f;
float lightBrightness = 0.75f;
// float specular = 128.0f;
float specular = 2.0f;
float prev_time = 0.0f;
float current_time = 0.0f;
float cur = 0.0f;

float yaw = -90.0f;
float pitch = 0.0f;
float fov = 90.0f;

float lastX = WINDOW_WIDTH/2.0f;
float lastY = WINDOW_HEIGHT/2.0f;
bool firstMouse = true;

float focalPlane = 0.900f;
float focalRadius = 0.1100f;
float is_visualization = 0.0f;
bool p_pressed = false;
float maxFilterStren = 1.0f;

#define bark_color 0.41f, 0.29f, 0.21f
#define leaf_color 0.51f, 0.61f, 0.41f 
#define grass_color 0.49f, 0.98f, 0.0f 
#define frame_color 0.82f, 0.82f, 0.82f 
#define wall_color 0.66f, 0.29f, 0.26f 

const int TERRAIN_WIDTH = 21; // odd for absolute center
const int TERRAIN_LENGTH = 21;
const float TERRAIN_HEIGHT = 0.5f; // height scale
const float TERRAIN_SCALE = 0.5f; // width and length scale
const float BASE_Y_LEVEL = -0.415f;

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

const std::vector<float> window_data = {
    // // front 
    // 0.000000f, -0.500000f, 0.000000f, frame_color,
    // -0.050000f, -0.450000f, 0.000000f, frame_color,
    // -0.450000f, -0.450000f, 0.000000f, frame_color,
    // 0.000000f, -0.500000f, 0.000000f, frame_color,
    // -0.450000f, -0.450000f, 0.000000f, frame_color,
    // -0.500000f, -0.500000f, 0.000000f, frame_color,
    // 0.000000f, 0.000000f, 0.000000f, frame_color,
    // -0.050000f, -0.050000f, 0.000000f, frame_color,
    // -0.050000f, -0.450000f, 0.000000f, frame_color,
    // 0.000000f, 0.000000f, 0.000000f, frame_color,
    // -0.050000f, -0.450000f, 0.000000f, frame_color,
    // 0.000000f, -0.500000f, 0.000000f, frame_color,
    // -0.500000f, 0.000000f, 0.000000f, frame_color,
    // -0.450000f, -0.050000f, 0.000000f, frame_color,
    // -0.050000f, -0.050000f, 0.000000f, frame_color,
    // -0.500000f, 0.000000f, 0.000000f, frame_color,
    // -0.050000f, -0.050000f, 0.000000f, frame_color,
    // 0.000000f, 0.000000f, 0.000000f, frame_color,
    // -0.500000f, -0.500000f, 0.000000f, frame_color,
    // -0.450000f, -0.450000f, 0.000000f, frame_color,
    // -0.450000f, -0.050000f, 0.000000f, frame_color,
    // -0.500000f, -0.500000f, 0.000000f, frame_color,
    // -0.450000f, -0.050000f, 0.000000f, frame_color,
    // -0.500000f, 0.000000f, 0.000000f, frame_color,

    // // back
    // -0.500000f, -0.500000f, -0.050000f, frame_color,
    // -0.450000f, -0.450000f, -0.050000f, frame_color,
    // -0.050000f, -0.450000f, -0.050000f, frame_color,
    // -0.500000f, -0.500000f, -0.050000f, frame_color,
    // -0.050000f, -0.450000f, -0.050000f, frame_color,
    // 0.000000f, -0.500000f, -0.050000f, frame_color,
    // 0.000000f, -0.500000f, -0.050000f, frame_color,
    // -0.050000f, -0.450000f, -0.050000f, frame_color,
    // -0.050000f, -0.050000f, -0.050000f, frame_color,
    // 0.000000f, -0.500000f, -0.050000f, frame_color,
    // -0.050000f, -0.050000f, -0.050000f, frame_color,
    // 0.000000f, 0.000000f, -0.050000f, frame_color,
    // 0.000000f, 0.000000f, -0.050000f, frame_color,
    // -0.050000f, -0.050000f, -0.050000f, frame_color,
    // -0.450000f, -0.050000f, -0.050000f, frame_color,
    // 0.000000f, 0.000000f, -0.050000f, frame_color,
    // -0.450000f, -0.050000f, -0.050000f, frame_color,
    // -0.500000f, 0.000000f, -0.050000f, frame_color,
    // -0.500000f, 0.000000f, -0.050000f, frame_color,
    // -0.450000f, -0.050000f, -0.050000f, frame_color,
    // -0.450000f, -0.450000f, -0.050000f, frame_color,
    // -0.500000f, 0.000000f, -0.050000f, frame_color,
    // -0.450000f, -0.450000f, -0.050000f, frame_color,
    // -0.500000f, -0.500000f, -0.050000f, frame_color,

    // // top facing
    // -0.450000f, -0.450000f, -0.050000f, frame_color,
    // -0.450000f, -0.450000f, 0.000000f, frame_color,
    // -0.050000f, -0.450000f, 0.000000f, frame_color,
    // -0.450000f, -0.450000f, -0.050000f, frame_color,
    // -0.050000f, -0.450000f, 0.000000f, frame_color,
    // -0.050000f, -0.450000f, -0.050000f, frame_color,
    // -0.500000f, 0.000000f, -0.050000f, frame_color,
    // -0.500000f, 0.000000f, 0.000000f, frame_color,
    // 0.000000f, 0.000000f, 0.000000f, frame_color,
    // -0.500000f, 0.000000f, -0.050000f, frame_color,
    // 0.000000f, 0.000000f, 0.000000f, frame_color,
    // 0.000000f, 0.000000f, -0.050000f, frame_color,

    // // bottom facing
    // -0.450000f, -0.050000f, 0.000000f, frame_color,
    // -0.450000f, -0.050000f, -0.050000f, frame_color,
    // -0.050000f, -0.050000f, -0.050000f, frame_color,
    // -0.450000f, -0.050000f, 0.000000f, frame_color,
    // -0.050000f, -0.050000f, -0.050000f, frame_color,
    // -0.050000f, -0.050000f, 0.000000f, frame_color,
    // -0.500000f, -0.500000f, 0.000000f, frame_color,
    // -0.500000f, -0.500000f, -0.050000f, frame_color,
    // 0.000000f, -0.500000f, -0.050000f, frame_color,
    // -0.500000f, -0.500000f, 0.000000f, frame_color,
    // 0.000000f, -0.500000f, -0.050000f, frame_color,
    // 0.000000f, -0.500000f, 0.000000f, frame_color,

    // // +x facing
    // 0.000000f, 0.000000f, -0.050000f, frame_color,
    // 0.000000f, 0.000000f, 0.000000f, frame_color,
    // 0.000000f, -0.500000f, 0.000000f, frame_color,
    // 0.000000f, 0.000000f, -0.050000f, frame_color,
    // 0.000000f, -0.500000f, 0.000000f, frame_color,
    // 0.000000f, -0.500000f, -0.050000f, frame_color,
    // -0.450000f, -0.050000f, -0.050000f, frame_color,
    // -0.450000f, -0.050000f, 0.000000f, frame_color,
    // -0.450000f, -0.450000f, 0.000000f, frame_color,
    // -0.450000f, -0.050000f, -0.050000f, frame_color,
    // -0.450000f, -0.450000f, 0.000000f, frame_color,
    // -0.450000f, -0.450000f, -0.050000f, frame_color,

    // // -x facing
    // -0.500000f, 0.000000f, 0.000000f, frame_color,
    // -0.500000f, 0.000000f, -0.050000f, frame_color,
    // -0.500000f, -0.500000f, -0.050000f, frame_color,
    // -0.500000f, 0.000000f, 0.000000f, frame_color,
    // -0.500000f, -0.500000f, -0.050000f, frame_color,
    // -0.500000f, -0.500000f, 0.000000f, frame_color,
    // -0.050000f, -0.050000f, 0.000000f, frame_color,
    // -0.050000f, -0.050000f, -0.050000f, frame_color,
    // -0.050000f, -0.450000f, -0.050000f, frame_color,
    // -0.050000f, -0.050000f, 0.000000f, frame_color,
    // -0.050000f, -0.450000f, -0.050000f, frame_color,
    // -0.050000f, -0.450000f, 0.000000f, frame_color,

    0.100000f, -0.500000f, 0.000000f, frame_color,
    0.050000f, -0.450000f, 0.000000f, frame_color,
    -0.450000f, -0.450000f, 0.000000f, frame_color,
    0.100000f, -0.500000f, 0.000000f, frame_color,
    -0.450000f, -0.450000f, 0.000000f, frame_color,
    -0.500000f, -0.500000f, 0.000000f, frame_color,
    0.100000f, 0.300000f, 0.000000f, frame_color,
    0.050000f, 0.250000f, 0.000000f, frame_color,
    0.050000f, -0.450000f, 0.000000f, frame_color,
    0.100000f, 0.300000f, 0.000000f, frame_color,
    0.050000f, -0.450000f, 0.000000f, frame_color,
    0.100000f, -0.500000f, 0.000000f, frame_color,
    -0.500000f, 0.300000f, 0.000000f, frame_color,
    -0.450000f, 0.250000f, 0.000000f, frame_color,
    0.050000f, 0.250000f, 0.000000f, frame_color,
    -0.500000f, 0.300000f, 0.000000f, frame_color,
    0.050000f, 0.250000f, 0.000000f, frame_color,
    0.100000f, 0.300000f, 0.000000f, frame_color,
    -0.500000f, -0.500000f, 0.000000f, frame_color,
    -0.450000f, -0.450000f, 0.000000f, frame_color,
    -0.450000f, 0.250000f, 0.000000f, frame_color,
    -0.500000f, -0.500000f, 0.000000f, frame_color,
    -0.450000f, 0.250000f, 0.000000f, frame_color,
    -0.500000f, 0.300000f, 0.000000f, frame_color,
    -0.500000f, -0.500000f, -0.100000f, frame_color,
    -0.450000f, -0.450000f, -0.100000f, frame_color,
    0.050000f, -0.450000f, -0.100000f, frame_color,
    -0.500000f, -0.500000f, -0.100000f, frame_color,
    0.050000f, -0.450000f, -0.100000f, frame_color,
    0.100000f, -0.500000f, -0.100000f, frame_color,
    0.100000f, -0.500000f, -0.100000f, frame_color,
    0.050000f, -0.450000f, -0.100000f, frame_color,
    0.050000f, 0.250000f, -0.100000f, frame_color,
    0.100000f, -0.500000f, -0.100000f, frame_color,
    0.050000f, 0.250000f, -0.100000f, frame_color,
    0.100000f, 0.300000f, -0.100000f, frame_color,
    0.100000f, 0.300000f, -0.100000f, frame_color,
    0.050000f, 0.250000f, -0.100000f, frame_color,
    -0.450000f, 0.250000f, -0.100000f, frame_color,
    0.100000f, 0.300000f, -0.100000f, frame_color,
    -0.450000f, 0.250000f, -0.100000f, frame_color,
    -0.500000f, 0.300000f, -0.100000f, frame_color,
    -0.500000f, 0.300000f, -0.100000f, frame_color,
    -0.450000f, 0.250000f, -0.100000f, frame_color,
    -0.450000f, -0.450000f, -0.100000f, frame_color,
    -0.500000f, 0.300000f, -0.100000f, frame_color,
    -0.450000f, -0.450000f, -0.100000f, frame_color,
    -0.500000f, -0.500000f, -0.100000f, frame_color,
    -0.450000f, -0.450000f, -0.100000f, frame_color,
    -0.450000f, -0.450000f, 0.000000f, frame_color,
    0.050000f, -0.450000f, 0.000000f, frame_color,
    -0.450000f, -0.450000f, -0.100000f, frame_color,
    0.050000f, -0.450000f, 0.000000f, frame_color,
    0.050000f, -0.450000f, -0.100000f, frame_color,
    -0.500000f, 0.300000f, -0.100000f, frame_color,
    -0.500000f, 0.300000f, 0.000000f, frame_color,
    0.100000f, 0.300000f, 0.000000f, frame_color,
    -0.500000f, 0.300000f, -0.100000f, frame_color,
    0.100000f, 0.300000f, 0.000000f, frame_color,
    0.100000f, 0.300000f, -0.100000f, frame_color,
    -0.450000f, 0.250000f, 0.000000f, frame_color,
    -0.450000f, 0.250000f, -0.100000f, frame_color,
    0.050000f, 0.250000f, -0.100000f, frame_color,
    -0.450000f, 0.250000f, 0.000000f, frame_color,
    0.050000f, 0.250000f, -0.100000f, frame_color,
    0.050000f, 0.250000f, 0.000000f, frame_color,
    -0.500000f, -0.500000f, 0.000000f, frame_color,
    -0.500000f, -0.500000f, -0.100000f, frame_color,
    0.100000f, -0.500000f, -0.100000f, frame_color,
    -0.500000f, -0.500000f, 0.000000f, frame_color,
    0.100000f, -0.500000f, -0.100000f, frame_color,
    0.100000f, -0.500000f, 0.000000f, frame_color,
    0.100000f, 0.300000f, -0.100000f, frame_color,
    0.100000f, 0.300000f, 0.000000f, frame_color,
    0.100000f, -0.500000f, 0.000000f, frame_color,
    0.100000f, 0.300000f, -0.100000f, frame_color,
    0.100000f, -0.500000f, 0.000000f, frame_color,
    0.100000f, -0.500000f, -0.100000f, frame_color,
    -0.450000f, 0.250000f, -0.100000f, frame_color,
    -0.450000f, 0.250000f, 0.000000f, frame_color,
    -0.450000f, -0.450000f, 0.000000f, frame_color,
    -0.450000f, 0.250000f, -0.100000f, frame_color,
    -0.450000f, -0.450000f, 0.000000f, frame_color,
    -0.450000f, -0.450000f, -0.100000f, frame_color,
    -0.500000f, 0.300000f, 0.000000f, frame_color,
    -0.500000f, 0.300000f, -0.100000f, frame_color,
    -0.500000f, -0.500000f, -0.100000f, frame_color,
    -0.500000f, 0.300000f, 0.000000f, frame_color,
    -0.500000f, -0.500000f, -0.100000f, frame_color,
    -0.500000f, -0.500000f, 0.000000f, frame_color,
    0.050000f, 0.250000f, 0.000000f, frame_color,
    0.050000f, 0.250000f, -0.100000f, frame_color,
    0.050000f, -0.450000f, -0.100000f, frame_color,
    0.050000f, 0.250000f, 0.000000f, frame_color,
    0.050000f, -0.450000f, -0.100000f, frame_color,
    0.050000f, -0.450000f, 0.000000f, frame_color,

};

const std::vector<float> wall_data = {
    // front
    1.900000f, -1.000000f, -0.010000f, wall_color,
    0.100000f, -0.500000f, -0.010000f, wall_color,
    -0.500000f, -0.500000f, -0.010000f, wall_color,
    1.900000f, -1.000000f, -0.010000f, wall_color,
    -0.500000f, -0.500000f, -0.010000f, wall_color,
    -2.300000f, -1.000000f, -0.010000f, wall_color,
    1.900000f, 0.800000f, -0.010000f, wall_color,
    0.100000f, 0.300000f, -0.010000f, wall_color,
    0.100000f, -0.500000f, -0.010000f, wall_color,
    1.900000f, 0.800000f, -0.010000f, wall_color,
    0.100000f, -0.500000f, -0.010000f, wall_color,
    1.900000f, -1.000000f, -0.010000f, wall_color,
    -2.300000f, 0.800000f, -0.010000f, wall_color,
    -0.500000f, 0.300000f, -0.010000f, wall_color,
    0.100000f, 0.300000f, -0.010000f, wall_color,
    -2.300000f, 0.800000f, -0.010000f, wall_color,
    0.100000f, 0.300000f, -0.010000f, wall_color,
    1.900000f, 0.800000f, -0.010000f, wall_color,
    -2.300000f, -1.000000f, -0.010000f, wall_color,
    -0.500000f, -0.500000f, -0.010000f, wall_color,
    -0.500000f, 0.300000f, -0.010000f, wall_color,
    -2.300000f, -1.000000f, -0.010000f, wall_color,
    -0.500000f, 0.300000f, -0.010000f, wall_color,
    -2.300000f, 0.800000f, -0.010000f, wall_color,

    // back
    -2.300000f, -1.000000f, -0.090000f, wall_color,
    -0.500000f, -0.500000f, -0.090000f, wall_color,
    0.100000f, -0.500000f, -0.090000f, wall_color,
    -2.300000f, -1.000000f, -0.090000f, wall_color,
    0.100000f, -0.500000f, -0.090000f, wall_color,
    1.900000f, -1.000000f, -0.090000f, wall_color,
    1.900000f, -1.000000f, -0.090000f, wall_color,
    0.100000f, -0.500000f, -0.090000f, wall_color,
    0.100000f, 0.300000f, -0.090000f, wall_color,
    1.900000f, -1.000000f, -0.090000f, wall_color,
    0.100000f, 0.300000f, -0.090000f, wall_color,
    1.900000f, 0.800000f, -0.090000f, wall_color,
    1.900000f, 0.800000f, -0.090000f, wall_color,
    0.100000f, 0.300000f, -0.090000f, wall_color,
    -0.500000f, 0.300000f, -0.090000f, wall_color,
    1.900000f, 0.800000f, -0.090000f, wall_color,
    -0.500000f, 0.300000f, -0.090000f, wall_color,
    -2.300000f, 0.800000f, -0.090000f, wall_color,
    -2.300000f, 0.800000f, -0.090000f, wall_color,
    -0.500000f, 0.300000f, -0.090000f, wall_color,
    -0.500000f, -0.500000f, -0.090000f, wall_color,
    -2.300000f, 0.800000f, -0.090000f, wall_color,
    -0.500000f, -0.500000f, -0.090000f, wall_color,
    -2.300000f, -1.000000f, -0.090000f, wall_color,

    // sides
    -2.300000f, 0.800000f, -0.090000f, wall_color,
    -2.300000f, 0.800000f, -0.010000f, wall_color,
    1.900000f, 0.800000f, -0.010000f, wall_color,
    -2.300000f, 0.800000f, -0.090000f, wall_color,
    1.900000f, 0.800000f, -0.010000f, wall_color,
    1.900000f, 0.800000f, -0.090000f, wall_color,
    1.900000f, -1.000000f, -0.090000f, wall_color,
    1.900000f, -1.000000f, -0.010000f, wall_color,
    -2.300000f, -1.000000f, -0.010000f, wall_color,
    1.900000f, -1.000000f, -0.090000f, wall_color,
    -2.300000f, -1.000000f, -0.010000f, wall_color,
    -2.300000f, -1.000000f, -0.090000f, wall_color,
    1.900000f, 0.800000f, -0.090000f, wall_color,
    1.900000f, 0.800000f, -0.010000f, wall_color,
    1.900000f, -1.000000f, -0.010000f, wall_color,
    1.900000f, 0.800000f, -0.090000f, wall_color,
    1.900000f, -1.000000f, -0.010000f, wall_color,
    1.900000f, -1.000000f, -0.090000f, wall_color,
    -2.300000f, -1.000000f, -0.090000f, wall_color,
    -2.300000f, -1.000000f, -0.010000f, wall_color,
    -2.300000f, 0.800000f, -0.010000f, wall_color,
    -2.300000f, -1.000000f, -0.090000f, wall_color,
    -2.300000f, 0.800000f, -0.010000f, wall_color,
    -2.300000f, 0.800000f, -0.090000f, wall_color,
};

std::vector<float> generate_tree(const glm::vec3& origin, float scale = 1.0f) {
    std::vector<float> vertices;

    for (size_t i = 0; i < tree_data.size(); i += 6) {
        float y = tree_data[i+1];

        // keep base at original y level
        float scaled_y = (y - (-1.0f)) * scale + (-0.5f); // pivot around y = -0.5

        vertices.push_back(tree_data[i] * scale + origin.x);
        vertices.push_back(scaled_y + origin.y);
        vertices.push_back(tree_data[i+2] * scale + origin.z);

        vertices.push_back(tree_data[i+3]);
        vertices.push_back(tree_data[i+4]);
        vertices.push_back(tree_data[i+5]);

    }

    return vertices;
}

void generate_object(std::vector<float>& list,const std::vector<float>& object_data, const glm::vec3& origin, float scale = 1.0f) {
    std::vector<float> vertices;

    for (size_t i = 0; i < object_data.size(); i += 6) {
        float y = object_data[i+1];

        // keep base at original y level
        float scaled_y = (y - (-1.0f)) * scale + (-0.5f); // pivot around y = -0.5

        vertices.push_back(object_data[i] * scale + origin.x);
        vertices.push_back(scaled_y + origin.y);
        vertices.push_back(object_data[i+2] * scale + origin.z);

        vertices.push_back(object_data[i+3]);
        vertices.push_back(object_data[i+4]);
        vertices.push_back(object_data[i+5]);

    }

    list.insert(list.end(), vertices.begin(), vertices.end());
}

std::vector<float> final_vertices;

std::vector<float> ground_vertices;
std::vector<unsigned int> ground_indices;

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

// arbitrary get height function and can specify middle y-level
float get_height(float x, float z, float base_height = BASE_Y_LEVEL) {
    // normalize coords -1 to 1 
    float nx = (x - TERRAIN_WIDTH/2.0f) / (TERRAIN_WIDTH/2.0f);
    float nz = (z - TERRAIN_LENGTH/2.0f) / (TERRAIN_LENGTH/2.0f);

    // arbitrary values. waves lessen in intensity 
    float wave1 = sin(nx * 3.0f) * 0.3f;
    float wave2 = cos(nz * 4.0f) * 0.2f;
    float wave3 = sin(nx * 10.0f + nz * 12.0f) * 0.1f;

    return base_height + (wave1 + wave2 + wave3) * TERRAIN_HEIGHT;
}

glm::vec3 calculate_terrain_norm(int x, int z, const std::vector<float>& heights) {
    float left = heights[std::max(x-1, 0) + z * TERRAIN_WIDTH];
    float right = heights[std::min(x+1, TERRAIN_WIDTH-1) + z * TERRAIN_WIDTH];
    float down = heights[x + std::max(z-1, 0) * TERRAIN_WIDTH];
    float up = heights[x + std::min(z+1, TERRAIN_LENGTH-1) * TERRAIN_WIDTH];

    glm::vec3 tangent(2.0f * TERRAIN_SCALE, (right - left) * TERRAIN_HEIGHT, 0.0f);
    glm::vec3 bitangent(0.0f, (up - down) * TERRAIN_HEIGHT, 2.0f * TERRAIN_SCALE);
    return glm::normalize(cross(bitangent, tangent));
}

void generate_terrain_data() {
    // for centering at (0, 0, 0)
    float half_width = (TERRAIN_WIDTH - 1) * TERRAIN_SCALE * 0.5f;
    float half_length = (TERRAIN_LENGTH - 1) * TERRAIN_SCALE * 0.5f;
    
    // get heights
    std::vector<float> heights(TERRAIN_WIDTH * TERRAIN_LENGTH);
    for (int z = 0; z < TERRAIN_LENGTH; z++) {
        for (int x = 0; x < TERRAIN_WIDTH; x++) {
            heights[x + z * TERRAIN_WIDTH] = get_height(x, z, BASE_Y_LEVEL); 
        }
    }

    // then calculate normals
    std::vector<glm::vec3> normals;
    for (int z = 0; z < TERRAIN_LENGTH; z++) {
        for (int x = 0; x < TERRAIN_WIDTH; x++) {
            normals.push_back(calculate_terrain_norm(x, z, heights));
        }
    }
    
    // build vertices
    for (int z = 0; z < TERRAIN_LENGTH; z++) {
        for (int x = 0; x < TERRAIN_WIDTH; x++) {
            float xpos = x * TERRAIN_SCALE - half_width;
            float zpos = z * TERRAIN_SCALE - half_length;
            float ypos = heights[x + z * TERRAIN_WIDTH];

            ground_vertices.push_back(xpos);
            ground_vertices.push_back(ypos);
            ground_vertices.push_back(zpos);
            
            // temporary color
            ground_vertices.push_back(0.49f);
            ground_vertices.push_back(0.98f);
            ground_vertices.push_back(0.0f);

            // normals
            glm::vec3 normal = normals[x + z * TERRAIN_WIDTH];
            ground_vertices.push_back(normal.x);
            ground_vertices.push_back(normal.y);
            ground_vertices.push_back(normal.z);
            // std::cout << normal.x << ", " << normal.y << ", " << normal.z << std::endl;
        }
    }
    // make triangles via indices
    ground_indices.clear();

    for (int z = 0; z < TERRAIN_LENGTH - 1; z++) {
        for (int x = 0; x < TERRAIN_WIDTH - 1; x++) {
            int current = z * TERRAIN_WIDTH + x;
            int next = (z + 1) * TERRAIN_WIDTH + x;
            
            // first triangle (top-left, bottom-left, top-right)
            ground_indices.push_back(current);
            ground_indices.push_back(next);
            ground_indices.push_back(current + 1);
            
            // second triangle (top-right, bottom-left, bottom-right)
            ground_indices.push_back(current + 1);
            ground_indices.push_back(next);
            ground_indices.push_back(next + 1);
        }
    }
}

float fb_rect[] = {
    // (x, y)    // (s, t)
    1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f,

    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f
};


// define OpenGL object IDs to represent the vertex array and the shader program in the GPU
GLuint vao;         // vertex array object (stores the render state for our vertex array)
GLuint vbo;         // vertex buffer object (reserves GPU memory for our vertex array)
GLuint shader;      // combined vertex and fragment shader
GLuint bounding_box_vao;
GLuint bounding_box_vbo;
GLuint ico_sphere_vao;
GLuint ico_sphere_vbo;
GLuint terrain_vao;
GLuint terrain_vbo;
GLuint terrain_ebo; // element buffer object using indices
GLuint fbo;
GLuint fb_texture;
GLuint rect_vao;
GLuint rect_vbo;
GLuint fb_shader;
GLuint rbo;
GLuint depth_texture;
GLuint stencil_texture;
GLuint msaa_texture;
GLuint msaa_fbo;

const int samples = 8;

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
    glGenBuffers(1, &terrain_ebo);

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
    glBufferData(GL_ARRAY_BUFFER, ground_vertices.size() * sizeof(float), ground_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ground_indices.size() * sizeof(unsigned int), ground_indices.data(), GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) (6 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);


    glGenFramebuffers(1, &msaa_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, msaa_fbo);


    glGenTextures(1, &msaa_texture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msaa_texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msaa_texture, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // glGenTextures(1, &stencil_texture);
    // glBindTexture(GL_TEXTURE_2D, stencil_texture);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_STENCIL, GL_UNSIGNED_INT, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_INDEX);

    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil_texture, 0);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);


    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE_ARB);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);  

    glGenTextures(1, &fb_texture);
    glBindTexture(GL_TEXTURE_2D, fb_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_texture, 0);



    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    
    // glGenRenderbuffers(1, &rbo);
    // glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    


    glGenVertexArrays(1, &rect_vao);
    glGenBuffers(1, &rect_vbo);
    glBindVertexArray(rect_vao);
    glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fb_rect), &fb_rect, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));



    // important: if you have more vertex arrays to draw, make sure you separately define them
    // with unique VAO and VBO IDs, and follow the same process above to upload them to the GPU

    // loading textures
    barkTexture = gdevLoadTexture("bark_gray.jpg", GL_REPEAT, true, true);
    if (! barkTexture) return false;
    leafTexture = gdevLoadTexture("leaf_gray.jpg", GL_REPEAT, true, true);
    if (! leafTexture) return false;
    groundTexture = gdevLoadTexture("grass_tex.jpg", GL_REPEAT, true, true);
    if (! groundTexture) return false;
    wallTexture = gdevLoadTexture("brick.jpg", GL_REPEAT, true, true);
    if (! wallTexture) return false;
    frameTexture = gdevLoadTexture("frame.jpg", GL_REPEAT, true, true);
    if (! frameTexture) return false;


    fb_shader = gdevLoadShader("framebuffer.vs", "framebuffer.fs");
    glUniform1i(glGetUniformLocation(fb_shader, "screenTexture"), 0);
    glUniform1i(glGetUniformLocation(fb_shader, "depthTexture"), 1);

    // load our shader program
    shader = gdevLoadShader("project.vs", "project.fs");
    if (! shader)
        return false;

    return true;
}

glm::vec3 calculate_normal(glm::vec3 a, glm::vec3 b){
    return glm::normalize(cross(a, b));
}

glm::vec3 calculateSkyColor(float timeOfDay) {
    const glm::vec3 dawnColor(0.3f, 0.2f, 0.5f);   // Purple
    const glm::vec3 dayColor(0.5f, 0.7f, 1.0f);    // Sky blue
    const glm::vec3 duskColor(0.8f, 0.3f, 0.1f);   // Orange
    const glm::vec3 nightColor(0.01f, 0.02f, 0.1f);// Dark blue

    // wrap time to 0-1 cycle
    timeOfDay = std::fmod(timeOfDay, 1.0f);

    // blend between phases (same logic as shader)
    if (timeOfDay < 0.25f) {
        return glm::mix(dawnColor, dayColor, timeOfDay * 4.0f);
    } 
    else if (timeOfDay < 0.5f) {
        return glm::mix(dayColor, duskColor, (timeOfDay - 0.25f) * 4.0f);
    }
    else if (timeOfDay < 0.75f) {
        return glm::mix(duskColor, nightColor, (timeOfDay - 0.5f) * 4.0f);
    }
    else {
        return glm::mix(nightColor, dawnColor, (timeOfDay - 0.75f) * 4.0f);
    }
}


// called by the main function to do rendering per frame
void render()
{





    
    // clear the whole frame
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black
    // glClearColor(0.03f, 0.06f, 0.05f, 1.0f); // dark desaturated green
    // glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // sky blue

    glUseProgram(fb_shader);
    glBindFramebuffer(GL_FRAMEBUFFER, msaa_fbo);
    glClearColor(0.03f, 0.05f, 0.1f, 1.0f); // dark blue-black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

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
    cur += lightSpeed * (current_time-prev_time); // accumulated time

    // float clamp_t = ((sin(t) + 1.0f)/2.0f);
    float clamp_t = 1.0f;
    // float rot_speed = 50.0f;
    float rot_speed = 0.0f;
    float time_of_day = fmod(cur / 24.0, 1.0); // 24-second cycle

    // sky 
    glm::vec3 sky_color = calculateSkyColor(time_of_day);
    glClearColor(sky_color.r, sky_color.g, sky_color.b, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // for dynamic shader lighting
    glUniform1f(glGetUniformLocation(shader, "timeOfDay"), time_of_day);

    // moving sun
    glm::vec2 sunPath = glm::normalize(glm::vec2(cos(time_of_day * 2.0f * glm::pi<float>()), sin(time_of_day * 2.0f * glm::pi<float>())));
    //sunPath = glm::normalize(glm::vec2(cos(0.0f * 2.0f * glm::pi<float>()), sin(0.0f * 2.0f * glm::pi<float>())));
    lightPosition = glm::vec3(sunDist * sunPath, 0);


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
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, barkTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, leafTexture);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, frameTexture);

    // then connect each texture unit to a sampler2D in the fragment shader
    glUniform1i(glGetUniformLocation(shader,"barkTex"), 2);
    glUniform1i(glGetUniformLocation(shader,"leafTex"), 3);
    glUniform1i(glGetUniformLocation(shader,"groundTex"), 4);
    glUniform1i(glGetUniformLocation(shader,"wallTex"), 5);
    glUniform1i(glGetUniformLocation(shader,"frameTex"), 6);
    
    
    // ... draw our triangles
    // objects
    glBindVertexArray(vao);
    glUniform1i(glGetUniformLocation(shader, "state"), 0); // 0 = objects
    glDrawArrays(GL_TRIANGLES, 0, (final_vertices.size() * sizeof(float)) / (9 * sizeof(float)));

    // terrain
    glBindVertexArray(terrain_vbo);
    glDrawElements(GL_TRIANGLES, ground_indices.size(), GL_UNSIGNED_INT, 0);

    // icosphere
    glUniform1i(glGetUniformLocation(shader, "state"), 1); // 1 = sun
    glBindVertexArray(ico_sphere_vao);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPosition);
    model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader, "model"),
        1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, (sizeof(icosphere) / (3 * sizeof(float))));
    
    // // bounding box
    // glUniform1i(glGetUniformLocation(shader, "is_light"), 1);
    // glBindVertexArray(bounding_box_vao);
    // glDrawArrays(GL_LINES, 0, sizeof(bounding_box) / (6 * sizeof(float)));

    glBindFramebuffer(GL_READ_FRAMEBUFFER, msaa_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(fb_shader);

    glUniform1f(glGetUniformLocation(fb_shader, "focalPlane"), focalPlane);
    glUniform1f(glGetUniformLocation(fb_shader, "focalRadius"), focalRadius);
    glUniform1f(glGetUniformLocation(fb_shader, "maxFilterStren"), maxFilterStren);
    glUniform1f(glGetUniformLocation(fb_shader, "is_vis"), is_visualization);
    std::cout << focalPlane << " " << focalRadius << " " << maxFilterStren << std::endl;

    // glEnable(GL_STENCIL_TEST);
    // glActiveTexture(GL_TEXTURE2);
    // glUniform1i(glGetUniformLocation(fb_shader, "stencilTexture"), 2);

    //glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glUniform1i(glGetUniformLocation(fb_shader, "depthTexture"), 1);


    glDisable(GL_CULL_FACE);
    glBindVertexArray(rect_vao);
    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb_texture);
    glUniform1i(glGetUniformLocation(fb_shader, "screenTexture"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
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

    if (glfwGetKey(pWindow, GLFW_KEY_I) == GLFW_PRESS) {
        focalPlane += 0.0005f;
        focalPlane = std::min(focalPlane, 1.0f);
    }

    if (glfwGetKey(pWindow, GLFW_KEY_K) == GLFW_PRESS) {
        focalPlane -= 0.0005f;
        focalPlane = std::max(focalPlane, 0.0f);
    }

    if (glfwGetKey(pWindow, GLFW_KEY_O) == GLFW_PRESS) {
        focalRadius += 0.0005f;
        focalRadius = std::min(focalRadius, 1.0f);
    }

    if (glfwGetKey(pWindow, GLFW_KEY_L) == GLFW_PRESS) {
        focalRadius -= 0.0005f;
        focalRadius = std::max(focalRadius, 0.0f);
    }

    if (glfwGetKey(pWindow, GLFW_KEY_P) == GLFW_PRESS) {
        std::cout << p_pressed << std::endl;
        if (!p_pressed) {
            if (is_visualization == 1.0f) {
            is_visualization = 0.0f;
            } else {
                is_visualization = 1.0f;
            }
            p_pressed = true;
        }
    }

    if (glfwGetKey(pWindow, GLFW_KEY_P) == GLFW_RELEASE) {
        p_pressed = false;
    }

    if (glfwGetKey(pWindow, GLFW_KEY_1) == GLFW_PRESS) {
        maxFilterStren += 0.001f;
    }

    if (glfwGetKey(pWindow, GLFW_KEY_2) == GLFW_PRESS) {
        maxFilterStren -= 0.001f;
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
    generate_terrain_data();

    // ADD OBJECTS
    std::vector<float> objects; 
    generate_object(objects, window_data, {0, .2f, 0}, 0.4f);
    generate_object(objects, wall_data, {0, .2f, 0}, 0.4f);
    generate_object(objects, tree_data, {0, 0, -2}, 0.7f);
    
    
    // srand(time(0));
    // srand(1747578212); // random generated seed
    // srand(1747578678);
    srand(1747578808);
    // tree generator lol
    int num_trees = 100;
    int xz_range = 49; // 0 to 20 divided by 20 // higher number, more precise floats
    int s_range = 10;
    for (int i = 0; i < num_trees; i++) {
        float x = float(rand() % xz_range*2 - xz_range)/float(xz_range)*3;
        float z = float(rand() % xz_range*2 - xz_range)/float(xz_range)*3;
        float s = float(rand() % s_range+1)/float(s_range*10) + 0.2f; // 0.2 to 0.3 inclusive
       
        generate_object(objects, tree_data, {x, 0, z}, s);
    }


    
    for (size_t i = 0; i < objects.size(); i += 6*3) { // 6 floats for 1 vertex, 3 vertex for 1 triangle
        glm::vec3 a = glm::vec3(objects[i+6]-objects[i+0], objects[i+7]-objects[i+1],objects[i+8]-objects[i+2]);
        glm::vec3 b = glm::vec3(objects[i+12]-objects[i+0], objects[i+13]-objects[i+1],objects[i+14]-objects[i+2]);

        glm::vec3 normal = calculate_normal(a, b);
        

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

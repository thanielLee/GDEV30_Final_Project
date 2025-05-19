/******************************************************************************
 * This is a really simple vertex shader that simply sets the output vertex's
 * position to be the same as the input.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec3 norm;

uniform mat4 projview;
uniform mat4 model;
uniform mat4 normalM;
uniform int is_light;
uniform float is_border;

out vec3 barycoord;
out vec3 normal;
out vec3 trans;
out mat3 rotate;
out vec2 shaderTexCoord;
out float scale_factor;

out vec3 worldSpacePosition;
out vec3 worldSpaceNorm;
out vec3 objectColor;

out vec3 blendWeights;

void main()
{
    worldSpacePosition = (model * vec4(vertexPosition, 1.0f)).xyz;
    worldSpaceNorm = (normalM * vec4(norm, 1.0f)).xyz;
    objectColor = vertexColor;
    
    gl_Position = projview * vec4(worldSpacePosition, 1.0f);

    // vec3 absNormal = abs(norm);
    // if (absNormal.z > absNormal.x && absNormal.z > absNormal.y) {
    //     shaderTexCoord = vertexPosition.xy*2;
    // } else if (absNormal.x > absNormal.y) {
    //     shaderTexCoord = vertexPosition.yz*2;
    // } else {
    //     shaderTexCoord = vertexPosition.xz*2;
    // }

    // get blend weights using world normal
    blendWeights = normalize(abs(worldSpaceNorm));
    blendWeights = pow(blendWeights, vec3(4.0)); // control sharpness (higher value, sharper transition betw axes )
    blendWeights /= (blendWeights.x + blendWeights.y + blendWeights.z);
}


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

out vec3 barycoord;
out vec3 normal;
out vec3 trans;
out mat3 rotate;
out vec2 shaderTexCoord;
out float scale_factor;

out vec3 worldSpacePosition;
out vec3 worldSpaceNorm;
out vec3 objectColor;

void main()
{
    worldSpacePosition = (model * vec4(vertexPosition, 1.0f)).xyz;
    worldSpaceNorm = (normalM * vec4(norm, 1.0f)).xyz;
    objectColor = vertexColor;
    
    gl_Position = projview * vec4(worldSpacePosition, 1.0f);
    // fragPos = vec3(model * vec4(vertexPosition, 1.0f));
    // final_light_pos = vec3(model * vec4(lightPos, 1.0f));

    // scale_factor = sqrt(model[0][0]*model[0][0] + model[1][1]*model[1][1] + model[2][2]*model[2][2]);
    

    // if ((gl_VertexID%3) == 0) {
    //     barycoord = vec3(1.0f, 0.0f, 0.0f);
    // } else if ((gl_VertexID%3) == 1) {
    //     barycoord = vec3(0.0f, 1.0f, 0.0f);
    // } else {
    //     barycoord = vec3(0.0f, 0.0f, 1.0f);
    // }
    
    // rotate[0] = model[0].xyz;
    // rotate[1] = model[1].xyz;
    // rotate[2] = model[2].xyz;
    // trans = vec3(model[0][3], model[1][3], model[2][3]);
    // normal = normalize(rotate * norm);

    vec3 absNormal = abs(norm);
    if (absNormal.z > absNormal.x && absNormal.z > absNormal.y) {
        shaderTexCoord = vertexPosition.xy;
    } else if (absNormal.x > absNormal.y) {
        shaderTexCoord = vertexPosition.yz;
    } else {
        shaderTexCoord = vertexPosition.xz;
    }

    shaderTexCoord = vec2(0.0f, 0.0f);
}


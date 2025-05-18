/******************************************************************************
 * This is a really simple fragment shader that simply sets the output fragment
 * color to yellow.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

in vec3 worldSpacePosition;
in vec3 worldSpaceNorm;

in vec3 objectColor;
in vec3 barycoord;
in vec3 normal;
in vec3 fragPos;
in vec3 trans;
in mat3 rotate;
in vec3 final_light_pos;
in float scale_factor;
out vec4 fragmentColor;
uniform float is_border;

in vec2 shaderTexCoord;
uniform sampler2D woodTex;
uniform sampler2D goldTex;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform int is_light;
uniform float s;
uniform float focalPlane;
uniform float focalRadius;

uniform vec3 eye;

void main()
{
    float grayscale;
    float gray_strength = 0.7;

    if (is_light == 1) {
        fragmentColor = vec4(lightColor, 1.0f);
        return;
    }

    // texturing
    if (objectColor.x == 1.0f || objectColor.z == 0.0f) {
        vec4 gold = texture(goldTex, shaderTexCoord);
        grayscale = mix(1.0f, gold.r, gray_strength);
    } else {
        vec4 wood = texture(woodTex, shaderTexCoord);
        grayscale = mix(1.0f, wood.r, gray_strength);
    }


    // diffuse and ambient lighting
    vec3 lightVector = normalize(lightPosition - worldSpacePosition);
    vec3 normalVector = normalize(worldSpaceNorm);

    vec3 colorDiffuse = clamp(dot(lightVector, normalVector), 0, 1) * lightColor;
    vec3 colorAmbient = objectColor * 1.0f;

    // phong specular lighting
    vec3 reflectionVector = normalize(reflect(-lightVector, normalVector));
    vec3 colorSpecular = pow(max(dot(reflectionVector, normalize(eye)), 0), s) * lightColor;

    vec3 colorFinal = objectColor * (colorDiffuse + colorAmbient) + colorSpecular;

    // fragmentColor = vec4(colorFinal * grayscale, 1.0f);
    fragmentColor = vec4(colorFinal * grayscale, 1.0f);

    // float k = focalPlane-focalRadius;
    // float j = focalPlane+focalRadius;
    // float z = gl_FragCoord.z;
    // if (z <= k) {
    //     fragmentColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    // } else if (k <= z && z <= focalPlane) {
    //     float lerpStren = (z)/(k-focalPlane) + (-focalPlane)/(k-focalPlane);
    //     fragmentColor = vec4(mix(vec3(0.0f), vec3(1.0f, 0.0f, 0.0f), lerpStren), 1.0f);
    // } else if (focalPlane <= z && z <= j) {
    //     float lerpStren = (z)/(j-focalPlane) + (-focalPlane)/(j-focalPlane);
    //     fragmentColor = vec4(mix(vec3(0.0f), vec3(0.0f, 1.0f, 0.0f), lerpStren), 1.0f);
    // } else {
    //     fragmentColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    // }

    // outlines
    // if ((barycoord.x <= 0.0075f || barycoord.y <= 0.0075f || barycoord.z <= 0.0075f) && is_border == 0.0f) {
    //     fragmentColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    // }

}

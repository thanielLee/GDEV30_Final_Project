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
uniform sampler2D barkTex;
uniform sampler2D leafTex;
uniform sampler2D groundTex;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform int is_light;
uniform float s;

uniform vec3 eye;

in vec3 blendWeights;

void main()
{
    float gray_strength = 0.7;

    if (is_light == 1) {
        fragmentColor = vec4(lightColor, 1.0f);
        return;
    }

    vec3 normalVector = normalize(worldSpaceNorm);

    // tripalanar mapping
    // get uvs for each axis in world pos
    vec2 uvX = worldSpacePosition.zy;
    vec2 uvY = worldSpacePosition.xz;
    vec2 uvZ = worldSpacePosition.xy;

    // use right texture for specific object using new uvs
    vec4 texX;
    vec4 texY;
    vec4 texZ;    
    if (objectColor.y < 0.5f) {
        texX = texture(barkTex, uvX);
        texY = texture(barkTex, uvY);
        texZ = texture(barkTex, uvZ);
    } else {
        texX = texture(leafTex, uvX);
        texY = texture(leafTex, uvY);
        texZ = texture(leafTex, uvZ);
    }
    if (is_border > 1.0f){
        texX = texture(groundTex, uvX);
        texY = texture(groundTex, uvY);
        texZ = texture(groundTex, uvZ);
    }

    // triplanar blending for all sampled textures
    vec4 texColor = texX * blendWeights.x + texY * blendWeights.y + texZ * blendWeights.z;

    // diffuse and ambient lighting
    vec3 lightVector = normalize(lightPosition - worldSpacePosition);


    vec3 colorDiffuse = clamp(dot(lightVector, normalVector), 0, 1) * lightColor;
    vec3 colorAmbient = objectColor * 0.50f;

    // phong specular lighting
    vec3 reflectionVector = normalize(reflect(-lightVector, normalVector));
    vec3 colorSpecular = pow(max(dot(reflectionVector, normalize(eye)), 0), s) * lightColor;

    vec3 colorFinal = objectColor * (colorDiffuse + colorAmbient) + colorSpecular;

    // fragmentColor = vec4(colorFinal * grayscale, 1.0f);
    fragmentColor = vec4(colorFinal, 1.0f) * texColor * gray_strength;

    // outlines
    // if ((barycoord.x <= 0.0075f || barycoord.y <= 0.0075f || barycoord.z <= 0.0075f) && is_border == 0.0f) {
    //     fragmentColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    // }

}

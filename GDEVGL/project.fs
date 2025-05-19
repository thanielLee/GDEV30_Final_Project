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
uniform int state;

in vec2 shaderTexCoord;
uniform sampler2D barkTex;
uniform sampler2D leafTex;
uniform sampler2D groundTex;
uniform sampler2D wallTex;
uniform sampler2D frameTex;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform int is_light;
uniform float s;

uniform vec3 eye;

in vec3 blendWeights;

uniform float timeOfDay;

vec3 ambientLight = vec3(0.3, 0.3, 0.5);
float lightIntensity = 0.2;

// for texturing via color
const vec3 barkColor = vec3(0.41, 0.29, 0.21);
const vec3 leafColor = vec3(0.51, 0.61, 0.41);
const vec3 grassColor = vec3(0.49, 0.98, 0.0);
const vec3 frameColor = vec3(0.82, 0.82, 0.82);
const vec3 wallColor = vec3(0.66, 0.29, 0.26);

bool matchColor(vec3 a, vec3 b, float tolerance) {
    return all(lessThan(abs(a - b), vec3(tolerance)));
}

float getDayPhase(float t) {
    return smoothstep(0.0, 0.1, t) - smoothstep(0.9, 1.0, t); // 1 = day, 0 = night
}

float isNight = 1.0 - getDayPhase(timeOfDay);

vec3 getSkyColor(float t) {
    vec3 dawnColor  = vec3(0.3, 0.2, 0.5);  // purple
    vec3 dayColor   = vec3(0.5, 0.7, 1.0);  // sky blue
    vec3 duskColor  = vec3(0.8, 0.3, 0.1);  // orange
    vec3 nightColor = vec3(0.01, 0.02, 0.1);// dark blue

    if (t < 0.25) return mix(dawnColor, dayColor, t * 4.0);
    else if (t < 0.5) return mix(dayColor, duskColor, (t - 0.25) * 4.0);
    else if (t < 0.75) return mix(duskColor, nightColor, (t - 0.5) * 4.0);
    else return mix(nightColor, dawnColor, (t - 0.75) * 4.0);
}

vec3 getLightColor(float t) {
    vec3 sunlight = vec3(1.0, 0.9, 0.7);  // warm
    vec3 moonlight = vec3(0.3, 0.3, 0.5); // cool
    return mix(moonlight, sunlight, getDayPhase(t));
}

void main()
{
    float gray_strength = 1;
    float tol = 0.02;

    if (state == 1) {
        fragmentColor = vec4(getLightColor(timeOfDay), 1.0f);
        return;
    }

    vec3 normalVector = normalize(worldSpaceNorm);

    // tripalanar mapping
    // get uvs for each axis in world pos
    vec2 uvX = worldSpacePosition.zy;
    vec2 uvY = worldSpacePosition.xz;
    vec2 uvZ = worldSpacePosition.xy;

    vec4 texX;
    vec4 texY;
    vec4 texZ;  

    // use right texture for specific object using new uvs
    // if (state == 0){ 
    if (matchColor(objectColor, barkColor, tol)) {
        texX = texture(barkTex, uvX);
        texY = texture(barkTex, uvY);
        texZ = texture(barkTex, uvZ);
    }
    else if (matchColor(objectColor, leafColor, tol)) {
        texX = texture(leafTex, uvX);
        texY = texture(leafTex, uvY);
        texZ = texture(leafTex, uvZ);
    }
    else if (matchColor(objectColor, frameColor, tol)) {
        texX = texture(frameTex, uvX);
        texY = texture(frameTex, uvY);
        texZ = texture(frameTex, uvZ);
    }
    else if (matchColor(objectColor, wallColor, tol)){
        texX = texture(wallTex, uvX);
        texY = texture(wallTex, uvY);
        texZ = texture(wallTex, uvZ);
    }
    else if (matchColor(objectColor, grassColor, tol)) {
        texX = texture(groundTex, uvX);
        texY = texture(groundTex, uvY);
        texZ = texture(groundTex, uvZ);
    }

    // triplanar blending for all sampled textures
    vec4 texColor = texX * blendWeights.x + texY * blendWeights.y + texZ * blendWeights.z;

    // day-night cycle
    vec3 skyColor = getSkyColor(timeOfDay);
    vec3 dynamicLightColor = getLightColor(timeOfDay);


    // diffuse lighting
    vec3 lightVector = normalize(lightPosition - worldSpacePosition);
    // vec3 colorDiffuse = clamp(dot(lightVector, normalVector), 0, 1) * lightColor;
    vec3 colorDiffuse = clamp(dot(lightVector, normalVector), 0, 1) * dynamicLightColor ;
    
    // ambient lighting based on time
    // vec3 colorAmbient = objectColor * 0.50f;
    vec3 colorAmbient = dynamicLightColor * lightIntensity;

    // phong specular lighting
    vec3 reflectionVector = normalize(reflect(-lightVector, normalVector));
    vec3 colorSpecular = pow(max(dot(reflectionVector, normalize(eye)), 0), s) * lightColor;

    vec3 colorFinal = objectColor * (colorDiffuse + colorAmbient) + colorSpecular;

    fragmentColor = vec4(colorFinal, 1.0f) * texColor * gray_strength;

    // outlines
    // if ((barycoord.x <= 0.0075f || barycoord.y <= 0.0075f || barycoord.z <= 0.0075f) && state == 0.0f) {
    //     fragmentColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    // }

}

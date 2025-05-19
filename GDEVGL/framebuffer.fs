#version 330 core

out vec4 fragmentColor;
in vec2 texCoords;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
uniform sampler2D stencilTexture;
uniform float focalPlane;
uniform float focalRadius;
uniform float is_vis;

const vec2 offsets[48] = vec2[48](
	2.0f * vec2(1.000000f, 0.000000f),
	2.0f * vec2(0.707107f, 0.707107f),
	2.0f * vec2(-0.000000f, 1.000000f),
	2.0f * vec2(-0.707107f, 0.707107f),
	2.0f * vec2(-1.000000f, -0.000000f),
	2.0f * vec2(-0.707106f, -0.707107f),
	2.0f * vec2(0.000000f, -1.000000f),
	2.0f * vec2(0.707107f, -0.707107f),
	
	4.0f * vec2(1.000000f, 0.000000f),
	4.0f * vec2(0.923880f, 0.382683f),
	4.0f * vec2(0.707107f, 0.707107f),
	4.0f * vec2(0.382683f, 0.923880f),
	4.0f * vec2(-0.000000f, 1.000000f),
	4.0f * vec2(-0.382684f, 0.923879f),
	4.0f * vec2(-0.707107f, 0.707107f),
	4.0f * vec2(-0.923880f, 0.382683f),
	4.0f * vec2(-1.000000f, -0.000000f),
	4.0f * vec2(-0.923879f, -0.382684f),
	4.0f * vec2(-0.707106f, -0.707107f),
	4.0f * vec2(-0.382683f, -0.923880f),
	4.0f * vec2(0.000000f, -1.000000f),
	4.0f * vec2(0.382684f, -0.923879f),
	4.0f * vec2(0.707107f, -0.707107f),
	4.0f * vec2(0.923880f, -0.382683f),

	6.0f * vec2(1.000000f, 0.000000f),
	6.0f * vec2(0.965926f, 0.258819f),
	6.0f * vec2(0.866025f, 0.500000f),
	6.0f * vec2(0.707107f, 0.707107f),
	6.0f * vec2(0.500000f, 0.866026f),
	6.0f * vec2(0.258819f, 0.965926f),
	6.0f * vec2(-0.000000f, 1.000000f),
	6.0f * vec2(-0.258819f, 0.965926f),
	6.0f * vec2(-0.500000f, 0.866025f),
	6.0f * vec2(-0.707107f, 0.707107f),
	6.0f * vec2(-0.866026f, 0.500000f),
	6.0f * vec2(-0.965926f, 0.258819f),
	6.0f * vec2(-1.000000f, -0.000000f),
	6.0f * vec2(-0.965926f, -0.258820f),
	6.0f * vec2(-0.866025f, -0.500000f),
	6.0f * vec2(-0.707106f, -0.707107f),
	6.0f * vec2(-0.499999f, -0.866026f),
	6.0f * vec2(-0.258819f, -0.965926f),
	6.0f * vec2(0.000000f, -1.000000f),
	6.0f * vec2(0.258819f, -0.965926f),
	6.0f * vec2(0.500000f, -0.866025f),
	6.0f * vec2(0.707107f, -0.707107f),
	6.0f * vec2(0.866026f, -0.499999f),
	6.0f * vec2(0.965926f, -0.258818f)
);

float get_far_field_stren(float depth) {
    float z_ndc = depth * 2.0 - 1.0f;
    float near = 0.1f;
    float far = 100.0f;
    float linear_depth = (2.0 * near * far) / (far + near - z_ndc * (far - near));
    float k = focalPlane-focalRadius;
    float j = focalPlane+focalRadius;
    float z = linear_depth / far;

    return min(max((z)/(j-focalPlane) + (-focalPlane)/(j-focalPlane), 0.0), 1.0);
}

float get_near_field_stren(float depth) {
    float z_ndc = depth * 2.0 - 1.0f;
    float near = 0.1f;
    float far = 100.0f;
    float linear_depth = (2.0 * near * far) / (far + near - z_ndc * (far - near));
    float k = focalPlane-focalRadius;
    float j = focalPlane+focalRadius;
    float z = linear_depth / far;

    return min(max((z)/(k-focalPlane) - (focalPlane)/(k-focalPlane), 0.0), 1.0);
}



void main(){
    //fragmentColor = texture(screenTexture, texCoords);

    float depthValue = texture(depthTexture, texCoords).r;
    float z_ndc = depthValue * 2.0 - 1.0f;
    float near = 0.1f;
    float far = 100.0f;
    float linear_depth = (2.0 * near * far) / (far + near - z_ndc * (far - near));

    //fragmentColor = vec4(vec3(linear_depth / far), 1.0f);
    float k = focalPlane-focalRadius;
    float j = focalPlane+focalRadius;
    float z = linear_depth / far;
    float far_field_stren = min(max((z)/(j-focalPlane) + (-focalPlane)/(j-focalPlane), 0.0), 1.0);

    vec3 total_color = texture(screenTexture, texCoords).rgb;
    vec3 far_field_color = far_field_stren * total_color;
    vec3 near_field_color = total_color;

    if (focalPlane <= z && z <= j) {
        fragmentColor = vec4(mix(vec3(0.0f), vec3(0.0f, 1.0f, 0.0f), far_field_stren), 1.0f);
    } else if (j <= z) {
        fragmentColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }

    //fragmentColor = vec4(vec3(far_field_stren), 1.0f);
    for (int i = 0; i < 48; i++) {
        vec2 cur_offset = offsets[i];
        vec2 new_offset = vec2(cur_offset.x/1600.0, cur_offset.y/900.0);
        float temp_depth = texture(depthTexture, texCoords + new_offset).r;
        float temp_stren = get_far_field_stren(temp_depth);
        far_field_color += temp_stren * texture(screenTexture, texCoords + new_offset).rgb;
    }

    for (int i = 0; i < 48; i++) {
        vec2 cur_offset = offsets[i];
        vec2 new_offset = vec2(cur_offset.x/1600.0, cur_offset.y/900.0);
        float temp_depth = texture(depthTexture, texCoords + new_offset).r;
        float temp_stren = get_far_field_stren(temp_depth);
        near_field_color += texture(screenTexture, texCoords + new_offset).rgb;
    }


    far_field_color /= 49.0;
    near_field_color /= 49.0;
    float near_field_stren = get_near_field_stren(depthValue);
    vec4 far_color = mix(vec4(texture(screenTexture, texCoords).rgb, 1.0f), vec4(far_field_color, 1.0f), far_field_stren);
    vec4 near_color = mix(vec4(texture(screenTexture, texCoords).rgb, 1.0f), vec4(near_field_color, 1.0f), near_field_stren);
    vec4 main_color = vec4(texture(screenTexture, texCoords).rgb, 1.0f);

    if (far_field_stren >= 0.001f && near_field_stren <= 0.001f) {
        fragmentColor = far_color;
    } else if (far_field_stren <= 0.001f && near_field_stren >= 0.001f) {
        fragmentColor = near_color;
    } else {
        fragmentColor = main_color;
    }

    // fragmentColor = vec4(vec3(get_near_field_stren(depthValue), 0.0f, 0.0f), 1.0f);


    // fragmentColor = vec4(far_field_stren * texture(screenTexture, texCoords).rgb, 1.0f);
    // fragmentColor = vec4(fartexture(screenTexture, texCoords).rgb, 1.0f);

    if (is_vis == 1.0f) {
        if (far_field_stren >= 0.001f && near_field_stren <= 0.001f) {
            fragmentColor = mix(vec4(vec3(0.0f), 1.0f), vec4(vec3(0.0f, 1.0f, 0.0f), 1.0f), far_field_stren);
        } else if (far_field_stren <= 0.001f && near_field_stren >= 0.001f) {
            fragmentColor = mix(vec4(vec3(0.0f), 1.0f), vec4(vec3(1.0f, 0.0f, 0.0f), 1.0f), near_field_stren);
        } else {
            fragmentColor = vec4(vec3(0.0f), 1.0f);
        }
    }
}
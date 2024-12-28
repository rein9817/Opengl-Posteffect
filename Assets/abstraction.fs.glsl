#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2 screenSize;
uniform float blurRadius1 = 2.0; 
uniform float blurRadius2 = 4.0; 
uniform float edgeThreshold = 0.1; 
uniform float edgeStrength = 0.7; 

vec4 gaussianBlur(sampler2D tex, vec2 uv, vec2 texelSize, float radius) {
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;
    
    for(float x = -radius; x <= radius; x += 1.0) {
        for(float y = -radius; y <= radius; y += 1.0) {
            float weight = exp(-(x*x + y*y) / (2.0 * radius * radius));
            vec2 offset = vec2(x, y) * texelSize;
            color += texture(tex, uv + offset) * weight;
            totalWeight += weight;
        }
    }
    
    return color / totalWeight;
}

void main() {
    vec2 texelSize = 1.0 / screenSize;
    
    vec4 blur1 = gaussianBlur(screenTexture, TexCoords, texelSize, blurRadius1);
    vec4 blur2 = gaussianBlur(screenTexture, TexCoords, texelSize, blurRadius2);
    
    // ­pºâ DoG (Difference of Gaussians)
    float dog = length(blur1.rgb - blur2.rgb);
    float edge = (dog > edgeThreshold) ? 1.0 : 0.0;

    float levels = 8.0;
    vec4 quantizedColor = floor(blur1 * levels) / levels;

    vec3 finalColor = mix(quantizedColor.rgb, vec3(0.0), edge * edgeStrength);
    
    FragColor = vec4(finalColor, 1.0);
}
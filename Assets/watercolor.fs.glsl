#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float time;
uniform float distortionStrength = 0.02;  
uniform float quantizationLevels = 8.0;  
uniform float noiseScale = 5.0;           
uniform float edgeStrength = 0.3;         
uniform float noiseStrength = 0.1;        

float hash(float n) {
    return fract(sin(n) * 43758.5453123);
}

// 2D noise
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    
    float a = hash(i.x + i.y * 57.0);
    float b = hash(i.x + 1.0 + i.y * 57.0);
    float c = hash(i.x + (i.y + 1.0) * 57.0);
    float d = hash(i.x + 1.0 + (i.y + 1.0) * 57.0);
    
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 3.0;
    
    for(int i = 0; i < 5; i++) {
        value += amplitude * noise(p * frequency);
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    
    return value;
}

vec4 gaussianBlur(sampler2D image, vec2 uv, vec2 resolution, float radius) {
    vec4 color = vec4(0.0);
    float total = 0.0;
    float step = 1.0;
    for(float x = -radius; x <= radius; x += step) {
        for(float y = -radius; y <= radius; y += step) {
            vec2 offset = vec2(x, y) / resolution;
            float weight = exp(-(x*x + y*y) / (2.0 * radius * radius));
            color += texture(image, uv + offset) * weight;
            total += weight;
        }
    }
    
    return color / total;
}

vec3 quantize(vec3 color, float levels) {
    return floor(color * levels) / levels;
}

void main() {
    vec2 noiseCoord = TexCoords * noiseScale;
    float noise1 = fbm(noiseCoord);
    float noise2 = fbm(noiseCoord + 4.0);
    
    vec2 distortion = vec2(noise1, noise2) * distortionStrength;
    vec2 distortedUV = TexCoords + distortion;
    
    vec4 originalColor = texture(screenTexture, TexCoords);
    vec4 blurredColor = gaussianBlur(screenTexture, distortedUV, textureSize(screenTexture, 0), 2.0);
    
    float mixStrength = fbm(noiseCoord * 2.0) * 0.5 + 0.5;
    vec4 mixedColor = mix(originalColor, blurredColor, mixStrength);
    
    float edge = fbm(noiseCoord * 3.0);
    vec3 edgeColor = mix(mixedColor.rgb, blurredColor.rgb, edge * edgeStrength);
    vec3 finalColor = quantize(edgeColor, quantizationLevels);
    
  
    finalColor += (vec3(noise1) - 0.5) * noiseStrength;
    FragColor = vec4(finalColor, 1.0);
}
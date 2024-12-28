#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float threshold;
uniform float softThreshold;
uniform float bloomIntensity;


const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);


vec3 extractBrightness(vec3 color) {
    float brightness = dot(color, vec3(0.299, 0.587, 0.114));
    float redFactor = color.r * 0.4;
    brightness += redFactor;
    
    float knee = threshold * softThreshold;
    float soft = brightness - threshold + knee;
    soft = clamp(soft / (2.0 * knee), 0.0, 1.0);
    
    vec3 brightColor = color * soft;
    brightColor = max(brightColor, color * max(0.0, brightness - threshold));
    
    if(color.r > 0.7 && color.g > 0.4) {
        brightColor *= 1.5;
    }
    
    return brightColor;
}

vec3 gaussianBlur(sampler2D tex, vec2 uv, vec2 resolution) {
    vec2 tex_offset = 2.0 / resolution;
    vec3 result = vec3(0.0);
    
    vec3 horizontal = texture(tex, uv).rgb * weight[0];
    for(int i = 1; i < 5; ++i) {
        horizontal += texture(tex, uv + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        horizontal += texture(tex, uv - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }
    
    vec3 vertical = horizontal * weight[0];
    for(int i = 1; i < 5; ++i) {
        vertical += texture(tex, uv + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        vertical += texture(tex, uv - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
    }
    
    return vertical;
}

void main() {

    vec3 originalColor = texture(screenTexture, TexCoords).rgb;
    

    vec3 brightColor = extractBrightness(originalColor);
    
  
    vec3 blurredBright = gaussianBlur(screenTexture, TexCoords, textureSize(screenTexture, 0));

    vec3 finalColor = originalColor + blurredBright * bloomIntensity;
    
    FragColor = vec4(finalColor, 1.0);
}
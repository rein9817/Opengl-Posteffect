// bloom_bright.fs - 優化岩漿光源提取
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;
uniform float threshold;
uniform float softThreshold;

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;
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
    
    FragColor = vec4(brightColor, 1.0);
}
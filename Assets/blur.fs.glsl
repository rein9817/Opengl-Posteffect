// blur.fs.glsl
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main() {
    vec2 texelSize = 1.0 / textureSize(screenTexture, 0);
    float kernel[9] = float[](
        1.0/16.0, 2.0/16.0, 1.0/16.0,
        2.0/16.0, 4.0/16.0, 2.0/16.0,
        1.0/16.0, 2.0/16.0, 1.0/16.0
    );
    
    vec4 result = vec4(0.0);
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            vec2 offset = vec2(float(i), float(j)) * texelSize;
            result += texture(screenTexture, TexCoords + offset) * 
                     kernel[(i + 1) * 3 + (j + 1)];
        }
    }
    
    FragColor = result;
}
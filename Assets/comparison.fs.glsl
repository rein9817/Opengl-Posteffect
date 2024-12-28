#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D originalTexture;
uniform sampler2D processedTexture;
uniform float barPosition;
uniform float barWidth;

void main()
{
    vec4 original = texture(originalTexture, TexCoords);
    vec4 processed = texture(processedTexture, TexCoords);
    
    float dist = abs(TexCoords.x - barPosition);
    
    if(dist < barWidth) {
        FragColor = vec4(1.0);
    }
    else {
        FragColor = TexCoords.x < barPosition ? original : processed;
    }
}
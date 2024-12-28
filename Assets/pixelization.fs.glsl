#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float pixelSize;

void main()
{
    vec2 texSize = textureSize(screenTexture, 0).xy;
    vec2 pixelated = floor(TexCoords * texSize / pixelSize) * pixelSize / texSize;
    FragColor = texture(screenTexture, pixelated);
}
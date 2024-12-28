#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int levels;

void main()
{
    vec4 color = texture(screenTexture, TexCoords);
    color.rgb = floor(color.rgb * float(levels)) / float(levels);
    FragColor = color;
}
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float time;
uniform float power1; 
uniform float power2;  

const float PI = 3.14159265;

void main()
{
    vec2 uv = TexCoords;
    uv.x += power1 * sin(uv.y * power2 * PI + time);
    
    FragColor = texture(screenTexture, uv);
}
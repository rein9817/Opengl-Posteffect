#version 330 core

out vec4 FragColor;
in vec3 worldNormal;

void main()
{
     vec3 color = normalize(worldNormal) * 0.5 + 0.5;
     FragColor = vec4(color, 1.0)
}
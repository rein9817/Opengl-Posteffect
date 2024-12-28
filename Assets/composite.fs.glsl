// composite.fs.glsl
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D quantizedTexture;
uniform sampler2D edgeTexture;

void main()
{
    vec4 quantized = texture(quantizedTexture, TexCoords);
    vec4 edges = texture(edgeTexture, TexCoords);
    vec4 finalColor = mix(quantized, vec4(1.0, 0.0, 0.0, 1.0), 0.5);  // 添加紅色來測試
    FragColor = finalColor;
}
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D blurTexture1;  // 第一個高斯模糊結果
uniform sampler2D blurTexture2;  // 第二個高斯模糊結果

void main()
{
    vec4 blur1 = texture(blurTexture1, TexCoords);
    vec4 blur2 = texture(blurTexture2, TexCoords);
    
    // 計算兩個高斯模糊結果的差異
    float diff = length(blur1.rgb - blur2.rgb);
    float edge = (diff > 0.1) ? 1.0 : 0.0;  // 使用閾值檢測邊緣
    
    FragColor = vec4(vec3(edge), 1.0);
}
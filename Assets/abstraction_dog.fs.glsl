#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D blur1;  // 小半徑模糊結果
uniform sampler2D blur2;  // 大半徑模糊結果

void main()
{
    vec4 g1 = texture(blur1, TexCoords);
    vec4 g2 = texture(blur2, TexCoords);
    
    // 計算 DOG
    float dog = length(g1.rgb - g2.rgb);
    float edge = (dog > 0.1) ? 1.0 : 0.0;  // 閾值可以調整
    
    // 結合模糊和邊緣
    vec4 blurColor = g1;
    vec3 finalColor = mix(blurColor.rgb, vec3(0.0), edge * 0.7);
    
    FragColor = vec4(finalColor, 1.0);
}
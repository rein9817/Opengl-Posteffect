#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D blur1;  // �p�b�|�ҽk���G
uniform sampler2D blur2;  // �j�b�|�ҽk���G

void main()
{
    vec4 g1 = texture(blur1, TexCoords);
    vec4 g2 = texture(blur2, TexCoords);
    
    // �p�� DOG
    float dog = length(g1.rgb - g2.rgb);
    float edge = (dog > 0.1) ? 1.0 : 0.0;  // �H�ȥi�H�վ�
    
    // ���X�ҽk�M��t
    vec4 blurColor = g1;
    vec3 finalColor = mix(blurColor.rgb, vec3(0.0), edge * 0.7);
    
    FragColor = vec4(finalColor, 1.0);
}
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D blurTexture1;  // �Ĥ@�Ӱ����ҽk���G
uniform sampler2D blurTexture2;  // �ĤG�Ӱ����ҽk���G

void main()
{
    vec4 blur1 = texture(blurTexture1, TexCoords);
    vec4 blur2 = texture(blurTexture2, TexCoords);
    
    // �p���Ӱ����ҽk���G���t��
    float diff = length(blur1.rgb - blur2.rgb);
    float edge = (diff > 0.1) ? 1.0 : 0.0;  // �ϥ��H���˴���t
    
    FragColor = vec4(vec3(edge), 1.0);
}
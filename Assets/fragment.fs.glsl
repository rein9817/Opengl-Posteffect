
// fragment.fs.glsl
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
uniform sampler2D texture_diffuse1;
uniform bool showNormal;

void main()
{
    if(showNormal)
    {
        vec3 normalColor = Normal * 0.5 + 0.5;
        FragColor = vec4(normalColor, 1.0);
    }
    else
    {
        vec3 lightColor = vec3(1.0);
        vec3 lightPos = vec3(1000.0, 2000.0, 1000.0);
        
        vec4 texColor = texture(texture_diffuse1, TexCoords);
        vec3 color = texColor.rgb;
        
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        
        vec3 ambient = 0.2 * lightColor;
        vec3 diffuse = diff * lightColor;
        
        vec3 result = (ambient + diffuse) * color;
        
        FragColor = vec4(result, texColor.a);
    }
}
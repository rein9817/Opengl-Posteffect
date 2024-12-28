#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;
uniform float magnifierRadius;   
uniform float magnifierBorder;    
uniform float zoomFactor;         
uniform vec2 mouse;              

void main()
{
    float dist = distance(TexCoords, mouse);
    
    vec2 zoomedCoords = TexCoords;
    
    if(dist < magnifierRadius - magnifierBorder) {
        vec2 direction = TexCoords - mouse;
        zoomedCoords = mouse + direction / zoomFactor;
    }
    
    vec4 color = texture(screenTexture, zoomedCoords);
    
    float borderDist = magnifierRadius - dist;
    if(borderDist < magnifierBorder && borderDist > 0.0) {
        float borderStrength = smoothstep(0.0, magnifierBorder, borderDist);
        vec4 borderColor = vec4(0.0, 0.0, 0.0, 1.0); 
        color = mix(borderColor, color, borderStrength);
    }
    
    FragColor = color;
}
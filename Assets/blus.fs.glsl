#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2 screenSize;
uniform float blurRadius;


void main()
{
	vec2 texelSize =  1.0 / screeSize;
	vec4 color = vec4(0.0);
	float total = 0.0;

	for (float x = -blurRadius; x <= blurRadius; x+=1.0)
	{
		for (float y = -blurRadius; y <= blurRadius; y+=1.0)
		{
			float weight = exp(-0.5 * (x * x + y * y) / (blurRadius * blurRadius));)
			vec2 offset = vec2(x,y) * texelSize;
			color += texture(screenTexture,TexCoords + offset) * weight;
			total += weight;
		}
	}
	FragColor = color / total;
}
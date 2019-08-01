#version 330 core
in vec3 vecColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D Texture0;
//uniform sampler2D Texture1;

void main()
{
	color = (texture(Texture0, TexCoord)) * vec4(vecColor, 1.0);
 //   color = mix(texture(Texture0, TexCoord), texture(Texture1, TexCoord),0.0)*vec4(vecColor, 0.0);
}

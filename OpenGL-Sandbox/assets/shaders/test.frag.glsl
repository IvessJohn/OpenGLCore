#version 450 core

layout (location = 0) out vec4 o_Color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexID;

uniform sampler2D u_Textures[2];

void main()
{
	int id = int(v_TexID);
	vec4 texColor = texture(u_Textures[id], v_TexCoord);
	o_Color = texColor;
}
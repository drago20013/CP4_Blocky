#shader vertex
#version 330 core

layout(location = 0) in uint verData1;
layout(location = 1) in uint verData2; 
layout(location = 2) in uint verData3;

out uint v_Color;

uniform mat4 u_MVP;

void main() {
	float x = float(verData1 & 0xFu);
	float y = float(((verData1 & 0xF0u) >> 4u) | ((verData2 & 0xFu) << 4u));
	float z = float((verData2 & 0xF0u) >> 4u);

	gl_Position = u_MVP * vec4(x, y, z, 1);
	v_Color = (verData1);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in uint v_Color;

uniform int u_TexIndex;
uniform sampler2D u_Texture[2];

void main() {
 	int index = int(u_TexIndex);
	color = texture(u_Texture[index], vec2(0.0 , 0.0)) * vec4(0.5, 0.7, 0.8, 1.0);
};
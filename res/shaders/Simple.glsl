#shader vertex
#version 460 core

layout(location = 0) in vec2 Pos;
layout(location = 1) in vec2 TexPos;

uniform float u_aspect;

out vec2 v_TexCoord;

void main(void) {
	v_TexCoord = TexPos;
	gl_Position = vec4(Pos.x / u_aspect, Pos.y, 0.0, 1.0);
}

#shader fragment
#version 460 core

in vec2 v_TexCoord;

out vec4 out_Color;
uniform sampler2D u_Texture;

void main(void) {
  	out_Color = texture(u_Texture, v_TexCoord);
}

#shader vertex
#version 330 core

layout(location = 0) in vec2 pos;

uniform float u_aspect;

void main(void) {
	
	gl_Position = vec4(pos.x / u_aspect, pos.y, 0.0, 1.0);
}

#shader fragment
#version 330 core

out vec4 out_Color;

void main(void) {
  	out_Color = vec4(0.8, 0.8, 0.8, 1.0);
}

#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in float directLight;

uniform mat4 u_MVP;

out ivec2 v_TexCoord;
out vec4 v_directLightColor;

void main(void) {
	v_directLightColor = vec4(0.2*directLight/10, 0.5*directLight/10, 0.8*directLight/10, 1.0);
	gl_Position = u_MVP * position;
}

#shader fragment
#version 330

in vec4 v_directLightColor;

void main(void) {
  gl_FragColor =  v_directLightColor;
}
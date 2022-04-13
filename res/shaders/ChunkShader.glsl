#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 u_MVP;

out vec2 v_TexCoord;
out float v_directLight;

void main(void) {
        gl_Position = u_MVP * vec4(position, 1.0);
	//v_directLight = directLight;
}

#shader fragment
#version 330

layout(location = 0) out vec4 color;

in float v_directLight;

void main(void) {
  color = vec4(0.6, 0.5, 0.2, 1.0); 
}
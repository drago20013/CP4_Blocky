#shader vertex
#version 460 core

layout(location = 0) in uint verData1;
layout(location = 1) in uint verData2; 
layout(location = 2) in uint verData3;
layout(location = 3) in uint verData4;

uniform mat4 u_MVP;

out vec2 v_TexCoord;
out vec4 v_directLightColor;

void main(void) {
	float x = float(verData1 & 0x1Fu);
	float y = float(((verData1 & 0xE0u) >> 5u) | ((verData2 & 0xFu) << 3u));
	float z = float(verData3 & 0x1Fu);
	uint blockType = (verData4 & 0xFu);
	uint directLight = (verData2 & 0xC0u) >> 6u;
	uint verPos = (verData2 & 0x30u) >> 4u;

	uint tex_y = blockType / 4u;
	uint tex_x;

	switch(directLight){
	case 0u:
	tex_x = (blockType * 3u) % 12u + 1u;
	v_directLightColor = vec4(0.6, 0.6, 0.6, 1.0);
	break;
	case 1u:
	tex_x = (blockType * 3u) % 12u + 1u;
	v_directLightColor = vec4(0.8, 0.8, 0.8, 1.0);
	break;
	case 2u:
	tex_x = (blockType * 3u) % 12u;
	v_directLightColor = vec4(1.0, 1.0, 1.0, 1.0);
	if(blockType == 2u)
		y -= 0.1;
	break;
	case 3u:
	tex_x = (blockType * 3u) % 12u + 2u;
	v_directLightColor = vec4(0.4, 0.4, 0.4, 1.0);
	break;
	}

	vec2 texCoord;
	float ratio = 16.0 / 192.0;

	switch(verPos){
	case 0u: //bl 0,0
	texCoord = vec2(float(tex_x)*ratio, float(tex_y)*ratio);
	break;
	case 1u: //br 1,0
	texCoord = vec2(float(tex_x+1u)*ratio, float(tex_y)*ratio);
	break;
	case 2u: //tl 0,1
	texCoord = vec2(float(tex_x)*ratio, float(tex_y+1u)*ratio);
	break;
	case 3u: //tr 1,1
	texCoord = vec2(float(tex_x+1u)*ratio, float(tex_y+1u)*ratio);
	break;
	}
	
	v_TexCoord = texCoord;
	gl_Position = u_MVP * vec4(x, y, z, 1.0);
}

#shader fragment
#version 460 core

in vec4 v_directLightColor;
in vec2 v_TexCoord;

const vec4 fogcolor = vec4(0.6, 0.8, 1.0, 1.0);
const float fogdensity = .00003;

out vec4 out_Color;

uniform sampler2D u_Textures[2];
uniform int u_TexIndex;

void main(void) {
	float z = gl_FragCoord.z / gl_FragCoord.w;
  	float fog = clamp(exp(-fogdensity * z * z), 0.2, 1);
	vec4 color = texture(u_Textures[u_TexIndex], v_TexCoord) * v_directLightColor;
  	out_Color = mix(fogcolor, color, fog);
}

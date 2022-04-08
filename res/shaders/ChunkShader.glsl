#shader vertex
#version 330

attribute vec4 coord;
uniform mat4 u_MVP;
varying vec4 texcoord;

void main(void) {
        texcoord = coord;
        gl_Position = u_MVP * vec4(coord.xyz, 1);
}

#shader fragment
#version 330

varying vec4 texcoord;

void main(void) {
  gl_FragColor = vec4(texcoord.w / 128.0, texcoord.w / 256.0, texcoord.w / 512.0, 1.0);
}
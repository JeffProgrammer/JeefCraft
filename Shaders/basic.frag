#version 120

varying vec3 vNormal;

void main() {
	gl_FragColor = vec4((vNormal + 1) / 2, 1);
}
/*
 *
 * Default shader program for drawing lines for the Bullet physics library
 *
 */

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;
uniform float uZFar;

#ifdef _VERTEX_

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

out VertexData {
	vec3 pos;
	vec3 col;
} v_out;

void main() {
	vec4 p = (uModelViewMatrix * vec4(aPosition, 1.0)); // hack
	gl_Position = uProjectionMatrix * p;
	v_out.pos = p.xyz;
	v_out.col = aColor;
}

#endif


#ifdef _FRAGMENT_

in VertexData {
	vec3 pos;
	vec3 col;
} f_in;

out vec3 color;

void writeDepth(float depth) {
	// this has to match with depth buffer settings from shadow shaders
	const float C = 0.01;
	float FC = 1.0 / log(uZFar * C + 1.0);
	gl_FragDepth = log(depth * C + 1.0) * FC;
}

void main() {
	writeDepth(-f_in.pos.z);
	color = f_in.col;
}

#endif
/*
 *
 * Default shader program.
 *
 */

// a   per-vertex attribute from application
// u   uniform variable from application
// v   from vertex shader
// tc  from tessellation control shader
// te  from tessellation evaluation shader
// g   from geometry shader
// f   from fragment shader

uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform float uZFar; //uZFar, uZfar, uZ_Far, uZ_far || zFar, zfar, z_far. z_Far


uniform vec3 uDiffuse;
uniform float uMetalicity;
uniform float uRoughness;
uniform float uSpecular;

uniform sampler2D uDiffuseMap;
uniform sampler2D uMetalicityMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uSpecularMap;
uniform sampler2D uNormalMap;


//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------


#ifdef _VERTEX_

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec2 aUV;


out VertexData {
	vec3 pos;
	vec3 normal;
	vec3 tangent;
	vec2 uv;
} v_out;


void main() {
	vec4 pos = uModelViewMatrix * vec4(aPosition, 1.0);
	v_out.pos = pos.xyz;
	v_out.normal = (uModelViewMatrix * vec4(aNormal, 0.0)).xyz;
	v_out.tangent = (uModelViewMatrix * vec4(aTangent, 0.0)).xyz;
	v_out.uv  = aUV;

	gl_Position = uProjectionMatrix * pos;
}

#endif


//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------


#ifdef _FRAGMENT_

// Viewspace data
in VertexData {
	vec3 pos;
	vec3 normal;
	vec3 tangent;
	vec2 uv;
} f_in;


// (rgb) diffuse, (a) metalicity or (rgb) diffuse, (a) opacity
// (rg) normal, (b) roughness, (a) specularity
//
layout(location = 0) out vec4 fDiffuse;
layout(location = 1) out vec4 fNormalMaterials;


void writeDepth(float);

subroutine void renderMode();
subroutine vec3 diffuseFetch();
subroutine float metalicityFetch();
subroutine float roughnessFetch();
subroutine float specularFetch();
subroutine vec4 normalFetch();

subroutine uniform renderMode doRender;

subroutine uniform diffuseFetch getDiffuse;
subroutine uniform metalicityFetch getMetalicity;
subroutine uniform roughnessFetch getRoughness;
subroutine uniform specularFetch getSpecular;
subroutine uniform normalFetch getNormal;


// Do Render
//
subroutine(renderMode) void depth_only() {
	writeDepth(-f_in.pos.z);
}

subroutine(renderMode) void material() {
	writeDepth(-f_in.pos.z);

	vec4 n = uProjectionMatrix * getNormal();
	n = faceforward(n, vec4(0.0, 0.0, 1.0, 0.0), n);
	fDiffuse = vec4(getDiffuse(), getMetalicity());
	fNormalMaterials = vec4(n.xy, getRoughness(), getSpecular());
}


// Diffuse
//
subroutine(diffuseFetch) vec3 diffuseFromTexture() { return texture(uDiffuseMap, f_in.uv).rgb; }
subroutine(diffuseFetch) vec3 diffuseFromValue() { return uDiffuse; }

// Metalicity
//
subroutine(metalicityFetch) float metalicityFromTexture() { return texture(uMetalicityMap, f_in.uv).r; }
subroutine(metalicityFetch) float metalicityFromValue() { return uMetalicity; }

// Roughness
//
subroutine(roughnessFetch) float roughnessFromTexture() { return texture(uRoughnessMap, f_in.uv).r; }
subroutine(roughnessFetch) float roughnessFromValue() { return uRoughness; }

// Specular
//
subroutine(specularFetch) float specularFromTexture() { return texture(uSpecularMap, f_in.uv).r; }
subroutine(specularFetch) float specularFromValue() { return uSpecular; }

// Normal
//
subroutine(normalFetch) vec4 normalFromTexture() {
	vec3 bitangent = normalize(cross(f_in.normal, f_in.tangent));
	vec3 tangent = normalize(cross(bitangent, f_in.normal));
	mat3 tbn = transpose(mat3(tangent, bitangent, f_in.normal));
	return vec4(tbn * normalize(texture(uNormalMap, f_in.uv).rgb * 2.0 - 1.0), 0.0);
}
subroutine(normalFetch) vec4 normalFromValue() { return vec4(f_in.normal, 0.0); }


void writeDepth(float depth) {
	// this has to match with depth buffer settings from shadow shaders
	const float C = 0.01;
	float FC = 1.0 / log(uZFar * C + 1.0);
	gl_FragDepth = log(depth * C + 1.0) * FC;
}


//
// Main
//
void main() {
	doRender();
}


#endif
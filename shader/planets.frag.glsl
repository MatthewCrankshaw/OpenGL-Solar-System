// OpenGL 4.0
#version 400

// Input from Vertex Shader
in vec4 frag_Pos;
in vec4 frag_UV;
in vec4 frag_Norm;
in vec4 frag_Light_Direction;

//get texture map
uniform sampler2D u_texture_Map;

// Output from Fragment Shader
out vec4 pixel_Colour;

uniform vec4 Ia = vec4(0.02f, 0.02f, 0.02f, 1.0f);
uniform vec4 Id = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform vec4 Is = vec4(1.0f, 1.0f, 1.0f, 1.0f);

vec4 Ka;
vec4 Kd;
vec4 Ks;
uniform float a = 21.264;


void main () {

	Ka = texture(u_texture_Map, frag_UV.xy);
	Kd = texture(u_texture_Map, frag_UV.xy);
	Ks = texture(u_texture_Map, frag_UV.xy);

	// Direction to Light (normalised)
	vec4 l = normalize(-frag_Light_Direction);

	// Surface Normal (normalised)
	vec4 n = normalize(frag_Norm);

	// Reflected Vector
	vec4 r = reflect(-l, n);

	// View Vector
	vec4 v = normalize(-frag_Pos);

	// ---------- Calculate Terms ----------
	// Ambient Term
	vec4 Ta = Ka * Ia;

	// Diffuse Term
	vec4 Td = Kd * max(dot(l, n), 0.0) * Id;

	// Specular Term
	vec4 Ts = Ks * pow((max(dot(r, v), 0.0)), a) * Is;


	//----------------------------------------------
	// Fragment Colour
	//----------------------------------------------
	pixel_Colour = Ta + Td + Ts;
}
// OpenGL 4.0
#version 400

// Input from Vertex Shader
in vec4 frag_UV;
in vec4 frag_colour;

//get texture map
uniform sampler2D u_texture_Map;

// Output from Fragment Shader
out vec4 pixel_Colour;


void main () {
	//----------------------------------------------
	// Fragment Colour
	//----------------------------------------------
	pixel_Colour = texture(u_texture_Map, frag_UV.xy);
}
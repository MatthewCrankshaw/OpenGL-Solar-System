// OpenGL 4.0
#version 400

// Input to Vertex Shader
in vec4 vert_Position;
in vec4 vert_UV;

// Transform Matrices
uniform mat4 u_View;
uniform mat4 u_Model;
uniform mat4 u_Projection;

out vec4 frag_UV;

void main() {
	frag_UV = vert_UV;

	gl_Position = u_Projection * u_View * u_Model * vert_Position;
}
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
out vec4 frag_colour;

void main() {
	frag_UV = vert_UV;
	frag_colour = vec4(int(vert_Position.x * 100)%255, int(vert_Position.y * 100)%255, 0.0, 0.0);
	//----------------------------------------------
	// Vertex Position
	//----------------------------------------------
	gl_Position = u_Projection * u_View * u_Model * vert_Position;
}

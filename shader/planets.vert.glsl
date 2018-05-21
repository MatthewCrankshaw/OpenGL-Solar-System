// OpenGL 4.0
#version 400

// Input to Vertex Shader
in vec4 vert_Position;
in vec4 vert_Norm;
in vec4 vert_UV;

// Transform Matrices
uniform mat4 u_View;
uniform mat4 u_Model;
uniform mat4 u_Projection;

//light source
vec4 u_Light_Direction; //vec4(1.0f, 0.0f, -1.0f, 0.0f);

out vec4 frag_UV;
out vec4 frag_Norm;
out vec4 frag_Light_Direction;

void main() {
	frag_UV = vert_UV;

	frag_Norm = u_View * u_Model * vert_Norm;

	vec4 direction = -vert_Position;

	u_Light_Direction = normalize(direction);

	frag_Light_Direction = u_View * u_Light_Direction;

	gl_Position = u_Projection * u_View * u_Model * vert_Position;
}

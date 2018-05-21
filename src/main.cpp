// System Headers
#include <iostream>
#include <fstream>
#include <cmath>

// OpenGL Headers
#if defined(_WIN32)
	#include <GL/glew.h>
	#if defined(GLEW_EGL)
		#include <GL/eglew.h>
	#elif defined(GLEW_OSMESA)
		#define GLAPI extern
		#include <GL/osmesa.h>
	#elif defined(_WIN32)
		#include <GL/wglew.h>
	#elif !defined(__APPLE__) && !defined(__HAIKU__) || defined(GLEW_APPLE_GLX)
		#include <GL/glxew.h>
	#endif

	// OpenGL Headers
	#define GLFW_INCLUDE_GLCOREARB
	#include <GLFW/glfw3.h>
#elif defined(__APPLE__)
	#define GLFW_INCLUDE_GLCOREARB
	#include <GLFW/glfw3.h>
	#include <OpenGL/gl3.h>
	#include <OpenGL/gl3ext.h>
#endif

// GLM Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Project Headers
#include "shader.h"
#include "utils.h"
#include "geometry.h"
#include "image.h"
#include "camera.h"

using namespace std;

// Camera
Camera *camera;

// --------------------------------------------------------------------------------
// GLFW Callbacks
// --------------------------------------------------------------------------------

// Called on Error Event
void onError(int error, const char *description) {
	// Print Error message
	std::cerr << "Error: " << error << " : " << description << std::endl;
}

// Called on Window Close Event
void onWindowClose(GLFWwindow *window) {
	// Nothing to do right now
	// Do not call glfwDestroyWindow from here
}

// Called on Window Size Event
void onFramebufferSize(GLFWwindow *window, int width, int height) {
	// Set-up the window/screen coordinates
	glfwMakeContextCurrent(window);
	glViewport(0, 0, width, height);
	glfwMakeContextCurrent(NULL);
}

// --------------------------------------------------------------------------------
// Mouse Input
// --------------------------------------------------------------------------------
void onMouseButton(GLFWwindow *window, int button, int action, int mods) {
	// Update Camera
	camera->onMouseButton(window, button, action, mods);
}

void onCursorPosition(GLFWwindow *window, double x, double y) {
	// Update Camera
	camera->onCursorPosition(window, x, y);
}

void translate(float tx, float ty, float tz, float T[16]);
void multiply44(float a[16], float b[16], float c[16]);
void rotateX(float theta, float Rx[16]);
void rotateY(float theta, float Ry[16]);
void rotateZ(float theta, float Rz[16]);
void rotate(float theta, float rx, float ry, float rz, float R[16]);
void scale(float sx, float sy, float sz, float S[16]);

//order goes distrance from sun starting at sun
//Sun, mercury,venus, earth, mars, jupiter, saturn, uranus, neptune
//I have tripled the size of the planets (not the sun) to make it more visible
float planet_sizes[9] =
{
    1.0f,
    0.00349f*5,
    0.00866f*5,
    0.00912f*5,
    0.00485f*5,
    0.10f*5,
    0.08f*5,
    0.0363f*5,
    0.03525f*5
};

float planet_speed[9] =
{
    0.0f,
    0.5f,
    0.45f,
    0.4f,
    0.35f,
    0.3f,
    0.25f,
    0.2f,
    0.15f,
};

const string PLANET_TEXTURE[9] =
{
    "./images/planets/sunmap.jpg",
    "./images/planets/mercurymap.jpg",
    "./images/planets/venusmap.jpg",
    "./images/planets/earthmap.jpg",
    "./images/planets/marsmap.jpg",
    "./images/planets/jupitermap.jpg",
    "./images/planets/saturnmap.jpg",
    "./images/planets/uranusmap.jpg",
    "./images/planets/neptunemap.jpg"
};

int main() {
	// Set Error Callback
	glfwSetErrorCallback(onError);

	// Initialise GLFW
	if (!glfwInit()) {
		// Return Error
		return 1;
	}

	// Set GLFW Window Hint - Full-Screen Antialiasing 16x
	glfwWindowHint(GLFW_SAMPLES, 16);

	// Create Window
	GLFWwindow *window = createWindow(600, 600, "Example 13 - Skybox", 3, 2);

	// Check Window
	if (window == NULL) {
		// Print Error Message
		std::cerr << "Error: create window or context failed." << std::endl;

		// Return Error
		return 1;
	}

	#if defined(_WIN32)
		// Initialise GLEW
		if (glewInit() != GLEW_OK) {
			// Return Error
			return 1;
		}
	#endif
    #if defined(__linux__)
        glewExperimental = true;
        if(glewInit() != GLEW_OK){
            return 1;
        }
    #endif // defined

	// Enable multi-sampling - Antialiasing
	glEnable(GL_MULTISAMPLE);

	// Set window callback functions
	glfwSetFramebufferSizeCallback(window, onFramebufferSize);
	glfwSetWindowCloseCallback(window, onWindowClose);

	// Set mouse input callback functions
	glfwSetMouseButtonCallback(window, onMouseButton);
	glfwSetCursorPosCallback(window, onCursorPosition);

	// ----------------------------------------
	// Initialise OpenGL
	// ----------------------------------------
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// ----------------------------------------
	// Camera
	// ----------------------------------------
	camera = new GimbalFreeLookCamera(window);

	// ----------------------------------------
	// Create GLSL Program and VAOs, VBOs
	// ----------------------------------------

	// Load GLSL Program
	GLuint skybox_program = loadProgram("./shader/skybox.vert.glsl", NULL, NULL, NULL, "./shader/skybox.frag.glsl");
    GLuint sphere_program = loadProgram("./shader/sphere.vert.glsl", NULL, NULL, NULL, "./shader/sphere.frag.glsl");

	// Load Texture Map
	int x, y, n;

	// Teide Volcano on Tenerife Skybox Images
	// Author: Emil Persson, aka Humus.
	// URL: http://www.humus.name
	// License: Creative Commons Attribution 3.0 Unported License.
	// Filenames
	const char *filenames[6] = {"images/posR.png",
								"images/negR.png",
								"images/negT.png",
								"images/posT.png",
								"images/posS.png",
								"images/negS.png"};

	// Load Cubemap
	GLuint cubemap_texture = loadTextureCubeMap(filenames, x, y, n);

    //-------------------------------------------------
    // load sphere textures
    //-------------------------------------------------


    unsigned char *planet_map[9];

    for(int i = 0; i < 1; i++){
        planet_map[i] = loadImage("./images/planets/earthmap.jpg", x, y, n, false);
    }


    if(planet_map[0] == NULL){
        return 0;
    }

    GLuint sphere_textures;

    glGenTextures(1, &sphere_textures);

    glBindTexture(GL_TEXTURE_2D, sphere_textures);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, planet_map[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // No mip-mapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Configure Texture Coordinate Wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D,0);
    delete[] planet_map[0];
    planet_map[0] = NULL;

	//------------------------------------------
	// Create sphere data and vao
	//------------------------------------------
	const int num_spheres = 9;
    glUseProgram(sphere_program);
	//buffer data
    vector<glm::vec4> sphere_buf;
	vector<glm::ivec3> sphere_indices;

	createSphereData(sphere_buf, sphere_indices, 0.1f, 50, 50);

    //set up the vao, vbo and ebo for spheres
	GLuint sphere_vao[num_spheres];
	GLuint sphere_vbo[num_spheres];
	GLuint sphere_ebo[num_spheres];

	for(int i = 0; i < num_spheres; i++){
        glGenVertexArrays(1, &sphere_vao[i]);
        glGenBuffers(1, &sphere_vbo[i]);
        glGenBuffers(1, &sphere_ebo[i]);

        glBindVertexArray(sphere_vao[i]);
        glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_ebo[i]);

        // Load Vertex Data
        glBufferData(GL_ARRAY_BUFFER, sphere_buf.size() * sizeof(glm::vec4), sphere_buf.data(), GL_STATIC_DRAW);

        // Load Element Data
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_indices.size() * sizeof(glm::ivec3), sphere_indices.data(), GL_STATIC_DRAW);

        //set position location
        //TODO change the program
        GLuint sphere_posLoc = glGetAttribLocation(sphere_program, "vert_Position");
        GLuint sphere_texLoc = glGetAttribLocation(sphere_program, "vert_UV");
        // Set Vertex Attribute Pointers
        glVertexAttribPointer(sphere_posLoc, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), NULL);
        glVertexAttribPointer(sphere_texLoc, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(8*sizeof(float)));
        // Enable Vertex Attribute Arrays
        glEnableVertexAttribArray(sphere_posLoc);
        glEnableVertexAttribArray(sphere_texLoc);

        // Unbind VAO, VBO & EBO
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


	// ----------------------------------------
	// Skybox
	// ----------------------------------------

	// Skybox Program
	glUseProgram(skybox_program);

	// Vertex and Index buffers (host)
	std::vector<glm::vec4> skybox_buffer;
	std::vector<glm::ivec3> skybox_indexes;

	// Create Skybox
	createSkybox(skybox_buffer, skybox_indexes);

	// Vertex Array Objects (VAO)
	GLuint skybox_vao = 0;

	// Vertex Buffer Objects (VBO)
	GLuint skybox_vbo = 0;

	// Element Buffer Objects (EBO)
	GLuint skybox_ebo = 0;

	// Create VAO, VBO & EBO
	glGenVertexArrays(1, &skybox_vao);
	glGenBuffers(1, &skybox_vbo);
	glGenBuffers(1, &skybox_ebo);

	// Bind VAO, VBO & EBO
	glBindVertexArray(skybox_vao);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_ebo);

	// Load Vertex Data
	glBufferData(GL_ARRAY_BUFFER, skybox_buffer.size() * sizeof(glm::vec4), skybox_buffer.data(), GL_STATIC_DRAW);

	// Load Element Data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, skybox_indexes.size() * sizeof(glm::ivec3), skybox_indexes.data(), GL_STATIC_DRAW);

	// Get Position Attribute location (must match name in shader)
	GLuint skybox_posLoc = glGetAttribLocation(skybox_program, "vert_Position");

	// Set Vertex Attribute Pointers
	glVertexAttribPointer(skybox_posLoc, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);

	// Enable Vertex Attribute Arrays
	glEnableVertexAttribArray(skybox_posLoc);

	// Unbind VAO, VBO & EBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// ----------------------------------------
	// Set Texture Unit
	glUseProgram(skybox_program);
	glUniform1i(glGetUniformLocation(skybox_program, "u_texture_Map"), 0);

	// ----------------------------------------
	// View Matrix
	// ----------------------------------------
	// Copy Skybox View Matrix to Shader
	glUseProgram(skybox_program);
	glUniformMatrix4fv(glGetUniformLocation(skybox_program, "u_View"),  1, GL_FALSE, glm::value_ptr(camera->getOrientationMatrix()));

	// ----------------------------------------
	// Projection Matrix
	// ----------------------------------------
	glm::mat4 projectionMatrix;

	// Calculate Perspective Projection
	projectionMatrix = glm::perspective(glm::radians(67.0f), 1.0f, 0.001f, 50.0f);

	// Copy Projection Matrix to Shader
	glUseProgram(skybox_program);
	glUniformMatrix4fv(glGetUniformLocation(skybox_program, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUseProgram(sphere_program);
	glUniformMatrix4fv(glGetUniformLocation(sphere_program, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// ----------------------------------------
	// Main Render loop
	// ----------------------------------------
	float time = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		// Make the context of the given window current on the calling thread
		glfwMakeContextCurrent(window);

		// Set clear (background) colour to black
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

		// Clear Screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update Time
		float current_time = glfwGetTime();
		float dt = current_time - time;
		time = current_time;

		// Update Camera (poll keyboard)
		camera->update(dt);


		// Copy Skybox View Matrix to Shader
		glUseProgram(skybox_program);
        glUniformMatrix4fv(glGetUniformLocation(skybox_program, "u_View"),  1, GL_FALSE, glm::value_ptr(camera->getOrientationMatrix()));

		// ----------------------------------------
		// Draw Skybox
		// ----------------------------------------

		// Use Skybox Program
		glUseProgram(skybox_program);

		// Bind Vertex Array Object
		glBindVertexArray(skybox_vao);

		// Disable Depth-Testing
		glDisable(GL_DEPTH_TEST);

		// Set active Texture Unit 0
		glActiveTexture(GL_TEXTURE0);

		// Bind Texture Map
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);

		// Draw Elements (Triangles)
		glDrawElements(GL_TRIANGLES, skybox_indexes.size() * 3, GL_UNSIGNED_INT, NULL);

		// Renable Depth-Testing
		glEnable(GL_DEPTH_TEST);

		// Set active Texture Unit 0
		glActiveTexture(GL_TEXTURE0);

		// Unbind Texture Map
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glBindVertexArray(0);
		// ----------------------------------------

        //---------------------------------------
        //draw spheres
        //---------------------------------------

        for(int i = 0; i < num_spheres; i++){
            float sc[16];
            float translation[16];
            float rotation[16];
            float temp[16];
            float model[16];

            scale(planet_sizes[i], planet_sizes[i], planet_sizes[i], sc);
            //scale(1.0,1.0,1.0,sc);
            translate((0.8f * (0.2 * i)), 0.0f, 0.0f, translation);
            rotateY(glfwGetTime() * planet_speed[i], rotation);

            multiply44(rotation, translation, rotation);
            multiply44(rotation, sc, model);

            glUseProgram(sphere_program);

            glUniformMatrix4fv(glGetUniformLocation(sphere_program, "u_View"),  1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));

            GLint modelLoc = glGetUniformLocation(sphere_program, "u_Model");

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);

            glEnable(GL_DEPTH_TEST);
            glBindVertexArray(sphere_vao[i]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sphere_textures);
            glDrawElements(GL_TRIANGLES, sphere_indices.size() * 3, GL_UNSIGNED_INT, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(0);
        }



		// Swap the back and front buffers
		glfwSwapBuffers(window);

		// Poll window events
		glfwPollEvents();
	}

	// Delete VAO, VBO & EBO
	glDeleteVertexArrays(1, &skybox_vao);
	glDeleteBuffers(1, &skybox_vbo);
	glDeleteBuffers(1, &skybox_ebo);

	for(int i = 0; i < num_spheres; i++){
        glDeleteVertexArrays(1, &sphere_vao[i]);
        glDeleteBuffers(1, &sphere_vbo[i]);
        glDeleteBuffers(1, &sphere_ebo[i]);
	}

	// Delete Program
	glDeleteProgram(skybox_program);
	glDeleteProgram(sphere_program);

	// Stop receiving events for the window and free resources; this must be
	// called from the main thread and should not be invoked from a callback
	glfwDestroyWindow(window);

	// Terminate GLFW
	glfwTerminate();

	return 0;
}

void translate(float tx, float ty, float tz, float T[16]) {
	T[0]  = 1.0f;  T[4]  = 0.0f;  T[8]  = 0.0f;  T[12] = tx;
	T[1]  = 0.0f;  T[5]  = 1.0f;  T[9]  = 0.0f;  T[13] = ty;
	T[2]  = 0.0f;  T[6]  = 0.0f;  T[10] = 1.0f;  T[14] = tz;
	T[3]  = 0.0f;  T[7]  = 0.0f;  T[11] = 0.0f;  T[15] = 1.0f;
}

void multiply44(float a[16], float b[16], float c[16]) {
	// Multiply each row of A with each column of B to give C
	c[0]  = a[0]*b[0]  + a[4]*b[1]  + a[8]*b[2]   + a[12]*b[3];
	c[4]  = a[0]*b[4]  + a[4]*b[5]  + a[8]*b[6]   + a[12]*b[7];
	c[8]  = a[0]*b[8]  + a[4]*b[9]  + a[8]*b[10]  + a[12]*b[11];
	c[12] = a[0]*b[12] + a[4]*b[13] + a[8]*b[14]  + a[12]*b[15];

	c[1]  = a[1]*b[0]  + a[5]*b[1]  + a[9]*b[2]   + a[13]*b[3];
	c[5]  = a[1]*b[4]  + a[5]*b[5]  + a[9]*b[6]   + a[13]*b[7];
	c[9]  = a[1]*b[8]  + a[5]*b[9]  + a[9]*b[10]  + a[13]*b[11];
	c[13] = a[1]*b[12] + a[5]*b[13] + a[9]*b[14]  + a[13]*b[15];

	c[2]  = a[2]*b[0]  + a[6]*b[1]  + a[10]*b[2]  + a[14]*b[3];
	c[6]  = a[2]*b[4]  + a[6]*b[5]  + a[10]*b[6]  + a[14]*b[7];
	c[10] = a[2]*b[8]  + a[6]*b[9]  + a[10]*b[10] + a[14]*b[11];
	c[14] = a[2]*b[12] + a[6]*b[13] + a[10]*b[14] + a[14]*b[15];

	c[3]  = a[3]*b[0]  + a[7]*b[1]  + a[11]*b[2]  + a[15]*b[3];
	c[7]  = a[3]*b[4]  + a[7]*b[5]  + a[11]*b[6]  + a[15]*b[7];
	c[11] = a[3]*b[8]  + a[7]*b[9]  + a[11]*b[10] + a[15]*b[11];
	c[15] = a[3]*b[12] + a[7]*b[13] + a[11]*b[14] + a[15]*b[15];
}

// Create a rotation matrix around the X-axis
void rotateX(float theta, float Rx[16]) {
	// Calculate sin(theta) and cos(theta)
	float sinTheta = sin(theta);
	float cosTheta = cos(theta);

	Rx[0]  = 1.0f;  Rx[4]  = 0.0f;      Rx[8]  =      0.0f;  Rx[12] = 0.0f;
	Rx[1]  = 0.0f;  Rx[5]  = cosTheta;  Rx[9]  = -sinTheta;  Rx[13] = 0.0f;
	Rx[2]  = 0.0f;  Rx[6]  = sinTheta;  Rx[10] =  cosTheta;  Rx[14] = 0.0f;
	Rx[3]  = 0.0f;  Rx[7]  = 0.0f;      Rx[11] =      0.0f;  Rx[15] = 1.0f;
}

// Create a rotation matrix around the Y-axis
void rotateY(float theta, float Ry[16]) {
	// Calculate sin(theta) and cos(theta)
	float sinTheta = sin(theta);
	float cosTheta = cos(theta);

	Ry[0]  =  cosTheta;  Ry[4]  = 0.0f;  Ry[8]  =  sinTheta;  Ry[12] = 0.0f;
	Ry[1]  =  0.0f;      Ry[5]  = 1.0f;  Ry[9]  =      0.0f;  Ry[13] = 0.0f;
	Ry[2]  = -sinTheta;  Ry[6]  = 0.0f;  Ry[10] =  cosTheta;  Ry[14] = 0.0f;
	Ry[3]  =  0.0f;      Ry[7]  = 0.0f;  Ry[11] =      0.0f;  Ry[15] = 1.0f;
}

// Create a rotation matrix around the Z-axis
void rotateZ(float theta, float Rz[16]) {
	// Calculate sin(theta) and cos(theta)
	float sinTheta = sin(theta);
	float cosTheta = cos(theta);

	Rz[0]  = cosTheta;  Rz[4]  = -sinTheta;  Rz[8]  = 0.0f;  Rz[12] = 0.0f;
	Rz[1]  = sinTheta;  Rz[5]  =  cosTheta;  Rz[9]  = 0.0f;  Rz[13] = 0.0f;
	Rz[2]  = 0.0f;      Rz[6]  =  0.0f;      Rz[10] = 1.0f;  Rz[14] = 0.0f;
	Rz[3]  = 0.0f;      Rz[7]  =  0.0f;      Rz[11] = 0.0f;  Rz[15] = 1.0f;
}

// Create a rotation matrix around arbitrary axis (rx, ry, rz)
void rotate(float theta, float rx, float ry, float rz, float R[16]) {
	// Calculate sin(theta) and cos(theta)
	float sinTheta = sin(theta);
	float cosTheta = cos(theta);

	float l = sqrt(rx*rx + ry*ry + rz*rz);
	rx /= l;
	ry /= l;
	rz /= l;

	R[0]  = cosTheta + (1-cosTheta)*rx*rx;     R[4]  = (1-cosTheta)*rx*ry - rz*sinTheta;  R[8]  = (1 - cosTheta)*rx*ry + ry*sinTheta;  R[12] = 0.0f;
	R[1]  = (1-cosTheta)*rx*ry + rz*sinTheta;  R[5]  = cosTheta + (1-cosTheta)*ry*ry;     R[9]  = (1 - cosTheta)*ry*rz - rx*sinTheta;  R[13] = 0.0f;
	R[2]  = (1-cosTheta)*rx*rz - ry*sinTheta;  R[6]  = (1-cosTheta)*ry*rz + rx*sinTheta;  R[10] = cosTheta + (1-cosTheta)*rz*rz;       R[14] = 0.0f;
	R[3]  = 0.0f;                              R[7]  = 0.0f;                              R[11] = 0.0f;                                R[15] = 1.0f;
}

void scale(float sx, float sy, float sz, float S[16]) {
	// Scaling Matrix
	S[0]  = sx;    S[4]  =  0.0f;  S[8]  = 0.0f;  S[12] = 0.0f;
	S[1]  = 0.0f;  S[5]  =  sy;    S[9]  = 0.0f;  S[13] = 0.0f;
	S[2]  = 0.0f;  S[6]  =  0.0f;  S[10] = sz;    S[14] = 0.0f;
	S[3]  = 0.0f;  S[7]  =  0.0f;  S[11] = 0.0f;  S[15] = 1.0f;
}



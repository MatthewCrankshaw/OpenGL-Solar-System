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

// --------------------------------------------------------------------------------
// Example 13 - Skybox
// --------------------------------------------------------------------------------
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
	const char *filenames[6] = {"images/posx.jpg",
								"images/negx.jpg",
								"images/negy.jpg",
								"images/posy.jpg",
								"images/posz.jpg",
								"images/negz.jpg"};

	// Load Cubemap
	GLuint texture = loadTextureCubeMap(filenames, x, y, n);


	//------------------------------------------
	// Sphere
	//------------------------------------------
    glUseProgram(sphere_program);
	//buffer data
    vector<glm::vec4> sphere_buf;
	vector<glm::ivec3> sphere_indices;

	createSphereData(sphere_buf, sphere_indices, 0.1f, 50, 50);

    //set up the vao, vbo and ebo for spheres
	GLuint sphere_vao = 0;
	GLuint sphere_vbo = 0;
	GLuint sphere_ebo = 0;

    glGenVertexArrays(1, &sphere_vao);
	glGenBuffers(1, &sphere_vbo);
	glGenBuffers(1, &sphere_ebo);

    glBindVertexArray(sphere_vao);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_ebo);

    // Load Vertex Data
	glBufferData(GL_ARRAY_BUFFER, sphere_buf.size() * sizeof(glm::vec4), sphere_buf.data(), GL_STATIC_DRAW);

	// Load Element Data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_indices.size() * sizeof(glm::ivec3), sphere_indices.data(), GL_STATIC_DRAW);

	//set position location
	//TODO change the program
	GLuint sphere_posLoc = glGetAttribLocation(sphere_program, "vert_Position");

    // Set Vertex Attribute Pointers
	glVertexAttribPointer(sphere_posLoc, 4, GL_FLOAT, GL_FALSE, 4 * 3 * sizeof(GLfloat), NULL);

    // Enable Vertex Attribute Arrays
	glEnableVertexAttribArray(sphere_posLoc);

	// Unbind VAO, VBO & EBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
	projectionMatrix = glm::perspective(glm::radians(67.0f), 1.0f, 0.1f, 50.0f);

	// Copy Projection Matrix to Shader
	glUseProgram(skybox_program);
	glUniformMatrix4fv(glGetUniformLocation(skybox_program, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	//glUseProgram(sphere_program);
	//glUniformMatrix4fv(glGetUniformLocation(sphere_program, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
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
		glUniformMatrix4fv(glGetUniformLocation(skybox_program, "u_View"),  1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
        //glUniformMatrix4fv(glGetUniformLocation(skybox_program, "u_View"),  1, GL_FALSE, glm::value_ptr(camera->getOrientationMatrix()));

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
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

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


		//glUseProgram(sphere_program);
		//render sphere
		glBindVertexArray(sphere_vao);
		glDrawElements(GL_TRIANGLES, sphere_indices.size() * 3, GL_UNSIGNED_INT, NULL);
		glBindVertexArray(0);

		glDisable(GL_DEPTH_TEST);


		// Swap the back and front buffers
		glfwSwapBuffers(window);

		// Poll window events
		glfwPollEvents();
	}

	// Delete VAO, VBO & EBO
	glDeleteVertexArrays(1, &skybox_vao);
	glDeleteBuffers(1, &skybox_vbo);
	glDeleteBuffers(1, &skybox_ebo);

	glDeleteVertexArrays(1, &sphere_vao);
	glDeleteBuffers(1, &sphere_vbo);
	glDeleteBuffers(1, &sphere_ebo);

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



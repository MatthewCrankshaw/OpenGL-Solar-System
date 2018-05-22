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
#include "transforms.h"

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

//order goes distrance from sun starting at sun
//Sun, mercury,venus, earth, mars, jupiter, saturn, uranus, neptune
//I have tripled the size of the planets (not the sun) to make it more visible
const float PLANET_SIZES[9] =
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

const float PLANET_SPEED[9] =
{
    0.0f,
    0.1f,
    0.09f,
    0.08f,
    0.07f,
    0.06f,
    0.05f,
    0.04f,
    0.03f,
};

const float PLANET_START_LOC[9] =
{
    0.0f,
    10.0f,
    54.0f,
    32.0f,
    90.0f,
    140.0f,
    20.0f,
    66.0f,
    88.0f
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
	GLFWwindow *window = createWindow(600, 600, "Assignment 3", 3, 2);

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

    //CONST VARS
    const int NUM_SPHERES = 9;

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
    GLuint sphere_program = loadProgram("./shader/planets.vert.glsl", NULL, NULL, NULL, "./shader/planets.frag.glsl");
    GLuint sun_program = loadProgram("./shader/sun.vert.glsl", NULL, NULL, NULL, "./shader/sun.frag.glsl");

	// Load Texture Map
	int x, y, n;

    //src https://gifer.com/en/NKrn
	// License: Creative Commons Attribution 3.0 Unported License.
	// Filenames
    const char *filenames[6] = {"images/px.png",
                                "images/nx.png",
                                "images/py.png",
                                "images/ny.png",
                                "images/pz.png",
                                "images/nz.png"};

	// Load Cubemap
	GLuint cubemap_texture = loadTextureCubeMap(filenames, x, y, n);

    //-------------------------------------------------
    // load sphere textures
    //-------------------------------------------------


    unsigned char *planet_map[9];
    GLuint sphere_textures[9];

    for(int i = 0; i < NUM_SPHERES; i++){

        //load texture into var
        planet_map[i] = loadImage(PLANET_TEXTURE[i].c_str(), x, y, n, false);
        //check if
        if(planet_map[i] == NULL){
            cout << "Image: " << PLANET_TEXTURE[i] << " was not found" << endl;
        }

        glGenTextures(1, &sphere_textures[i]);

        glBindTexture(GL_TEXTURE_2D, sphere_textures[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, planet_map[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // No mip-mapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Configure Texture Coordinate Wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D,0);
        delete[] planet_map[i];
        planet_map[i] = NULL;
    }

	//------------------------------------------
	// Create sphere data and vao
	//------------------------------------------
    glUseProgram(sphere_program);
	//buffer data
    vector<glm::vec4> sphere_buf;
	vector<glm::ivec3> sphere_indices;

	createSphereData(sphere_buf, sphere_indices, 0.1f, 50, 50);

    //set up the vao, vbo and ebo for spheres
	GLuint sphere_vao[NUM_SPHERES];
	GLuint sphere_vbo[NUM_SPHERES];
	GLuint sphere_ebo[NUM_SPHERES];

	for(int i = 0; i < NUM_SPHERES; i++){
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

        GLuint sphere_posLoc;
        GLuint sphere_normLoc;
        GLuint sphere_texLoc;
        //set position location
        if(i == 0){//the sun
            sphere_posLoc = glGetAttribLocation(sun_program, "vert_Position");
            sphere_texLoc = glGetAttribLocation(sun_program, "vert_UV");
            // Set Vertex Attribute Pointers
            glVertexAttribPointer(sphere_posLoc, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), NULL);
            glVertexAttribPointer(sphere_texLoc, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(8*sizeof(float)));
            // Enable Vertex Attribute Arrays
            glEnableVertexAttribArray(sphere_posLoc);
            glEnableVertexAttribArray(sphere_texLoc);
        }else{
            sphere_posLoc = glGetAttribLocation(sphere_program, "vert_Position");
            sphere_normLoc = glGetAttribLocation(sphere_program, "vert_Norm");
            sphere_texLoc = glGetAttribLocation(sphere_program, "vert_UV");
            // Set Vertex Attribute Pointers
            glVertexAttribPointer(sphere_posLoc, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), NULL);
            glVertexAttribPointer(sphere_normLoc, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(4*sizeof(float)));
            glVertexAttribPointer(sphere_texLoc, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(8*sizeof(float)));
            // Enable Vertex Attribute Arrays
            glEnableVertexAttribArray(sphere_posLoc);
            glEnableVertexAttribArray(sphere_normLoc);
            glEnableVertexAttribArray(sphere_texLoc);
        }

        // Unbind VAO, VBO & EBO
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // ----------------------------------------
        // Set Texture Unit
        if(i == 0){
            glUseProgram(sun_program);
            glUniform1i(glGetUniformLocation(sun_program, "u_texture_Map"), 0);
        }else{
            glUseProgram(sphere_program);
            glUniform1i(glGetUniformLocation(sphere_program, "u_texture_Map"), 0);
        }
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

	glUseProgram(sun_program);
	glUniformMatrix4fv(glGetUniformLocation(sun_program, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

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

        for(int i = 0; i < NUM_SPHERES; i++){
            //set up all of the transform matrices
            float sc[16];
            float translation[16];
            float rot_around[16], rot_inplace[16];
            float temp[16],temp2[16];
            float model[16];

            //get scale matrix
            scale(PLANET_SIZES[i], PLANET_SIZES[i], PLANET_SIZES[i], sc);
            //get translate matrix
            translate((0.8f * (0.4 * i)), 0.0f, 0.0f, translation);
            //get rotation around sun matrix
            rotateY(glfwGetTime() * PLANET_SPEED[i], rot_around);
            //get rotation around the y axis
            rotateY(glfwGetTime() * 0.5, rot_inplace);

            //rotate and then translate
            multiply44(translation, rot_inplace, temp);
            //rotate around sun
            multiply44(rot_around, temp, temp2);
            //scale size
            multiply44(temp2, sc, model);

            GLint modelLoc;

            if(i == 0){
                glUseProgram(sun_program);
                glUniformMatrix4fv(glGetUniformLocation(sun_program, "u_View"),  1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
                modelLoc = glGetUniformLocation(sun_program, "u_Model");
            }else{
                glUseProgram(sphere_program);
                glUniformMatrix4fv(glGetUniformLocation(sphere_program, "u_View"),  1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
                modelLoc = glGetUniformLocation(sphere_program, "u_Model");
            }



            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);

            //enable depth testing for spheres
            glEnable(GL_DEPTH_TEST);
            //bind vertex array
            glBindVertexArray(sphere_vao[i]);
            //set active texture and bind correct texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sphere_textures[i]);
            //draw the sphere with texture
            glDrawElements(GL_TRIANGLES, sphere_indices.size() * 3, GL_UNSIGNED_INT, NULL);
            //unbind the texture
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE0);
            //unbind vertex array object
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

	for(int i = 0; i < NUM_SPHERES; i++){
        glDeleteVertexArrays(1, &sphere_vao[i]);
        glDeleteBuffers(1, &sphere_vbo[i]);
        glDeleteBuffers(1, &sphere_ebo[i]);
	}

	// Delete Program
	glDeleteProgram(skybox_program);
	glDeleteProgram(sphere_program);
	glDeleteProgram(sun_program);

	// Stop receiving events for the window and free resources; this must be
	// called from the main thread and should not be invoked from a callback
	glfwDestroyWindow(window);

	// Terminate GLFW
	glfwTerminate();

	return 0;
}



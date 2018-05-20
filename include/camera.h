#ifndef CAMERA_H
#define CAMERA_H

// System Headers
#include <iostream>

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
		// OpenGL Headers
	#include <OpenGL/gl3.h>
#elif defined(__LINUX__)
    #include <GL/glew.h>
    #include <GL/glut.h>
    #include <GLFW/glfw3.h>

#elif defined(__unix__)
    #include <GL/glew.h>
    #include <GL/glut.h>
    #include <GLFW/glfw3.h>
#endif

// GLM Core Headers
#include <glm/glm.hpp>

// GLM GTC Extensions (Stable)
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

//GLM GTX Extensions (Experimental)
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

// --------------------------------------------------------------------------------
// Abstract base class
// --------------------------------------------------------------------------------
class Camera {
public:
	// Constructor
	Camera(GLFWwindow *window) : mWindow(window) {}
	virtual ~Camera() {}

	// Camera View Matrix
	virtual glm::mat4 getViewMatrix() = 0;
	virtual glm::mat4 getOrientationMatrix() = 0;

	// GLFW Input
	virtual void onMouseButton(GLFWwindow *window, int button, int action, int mods) = 0;
	virtual void onCursorPosition(GLFWwindow *window, double x, double y) = 0;

	// Update Camera
	virtual void update(float dt) = 0;
protected:
	// GLFW Window
	GLFWwindow *mWindow;
};

// --------------------------------------------------------------------------------
// Free Look Camera
// --------------------------------------------------------------------------------
class FreeLookCamera : public Camera {
public:
	// Constructor
	FreeLookCamera(GLFWwindow *window);
	virtual ~FreeLookCamera() {}

	// Camera View Matrix
	glm::mat4 getViewMatrix();
	glm::mat4 getOrientationMatrix();

	// GLFW Input
	void onMouseButton(GLFWwindow *window, int button, int action, int mods);
	void onCursorPosition(GLFWwindow *window, double x, double y);

	// Update Camera
	void update(float dt);

	// Movement
	virtual void moveForward(float x);
	virtual void moveBackward(float x);
	virtual void moveRight(float x);
	virtual void moveLeft(float x);

	// Rotations
	virtual void pitch(float angle);
	virtual void yaw(float angle);
	virtual void roll(float angle);
protected:
	// Data Members
	glm::vec3 mPosition;
	glm::quat mOrientation;
	float mSpeed;
	glm::vec2 mCursorPosition;
	bool mTrack;
};

// --------------------------------------------------------------------------------
// Gimbal Free Look Camera
// --------------------------------------------------------------------------------
class GimbalFreeLookCamera : public FreeLookCamera {
public:
	// Constructor
	GimbalFreeLookCamera(GLFWwindow *window);
	virtual ~GimbalFreeLookCamera() {}

	// Rotations
	void pitch(float angle);
	void yaw(float angle);
	void roll(float angle);
private:
	// Data Members
	float mViewAngleX, mViewAngleY;
};

#endif // CAMERA_H

// Project Headers 
#include "camera.h"

// --------------------------------------------------------------------------------
// Free Look Camera
// --------------------------------------------------------------------------------
// Constructor
FreeLookCamera::FreeLookCamera(GLFWwindow *window) : Camera(window) {
	// Orientation
	mOrientation = glm::quat(1, 0, 0, 0);

	// Position
	mPosition = glm::vec3(0, 0, 0);

	// Camera Speed
	mSpeed = 10.0;

	// Disable Tracking
	mTrack = false;
}

// Camera View Matrix
glm::mat4 FreeLookCamera::getViewMatrix() {
	// Generate Forward, Right, Up Vectors
	glm::vec3 forward = glm::vec3(mOrientation * glm::vec4( 0,  0, -1,  0));
	glm::vec3 up      = glm::vec3(mOrientation * glm::vec4( 0,  1,  0,  0));

	// Generate View Matrix
	return glm::lookAt(mPosition, mPosition + forward, up);
}

// Camera Orientation Matrix
glm::mat4 FreeLookCamera::getOrientationMatrix() {
	// Generate Forward, Right, Up Vectors
	glm::vec3 forward = glm::vec3(mOrientation * glm::vec4( 0,  0, -1,  0));
	glm::vec3 up      = glm::vec3(mOrientation * glm::vec4( 0,  1,  0,  0));

	// Generate View Matrix
	return glm::lookAt(glm::vec3(0,0,0), forward, up);
}

// GLFW Input
void FreeLookCamera::onMouseButton(GLFWwindow *window, int button, int action, int mods) {
	// Left Mouse Button
	if(button == GLFW_MOUSE_BUTTON_LEFT) {
		// Check Action
		if(action == GLFW_PRESS) {
			// Start Tracking
			mTrack = true;
		} else if(action == GLFW_RELEASE) {
			// Stop Tracking
			mTrack = false;
		}		
	}
}

void FreeLookCamera::onCursorPosition(GLFWwindow *window, double x, double y) {
	// Get Window Size
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Check Tracking
	if(mTrack) {
		// Get difference
		glm::vec2 d = mCursorPosition - glm::vec2(x, y);

		// Update Orientation
		pitch(d.y*2.0f*glm::pi<float>()/width);
		yaw(  d.x*2.0f*glm::pi<float>()/height);

		// Check Quaternion Magnitude
		if((glm::length2(mOrientation) - 1.0f) > 0.00001) {
			// Renormalise Model Quaternion
			mOrientation = glm::normalize(mOrientation);
		}
	}

	// Update Cursor Position
	mCursorPosition = glm::vec2(x,y);
}

// Update Camera
void FreeLookCamera::update(float dt) {
	// Get Keyboard input - Q
	if(glfwGetKey(mWindow, GLFW_KEY_Q) == GLFW_PRESS) {
		// Roll Right
		roll(-1.0f*glm::pi<float>()*dt);
	}
	// Get Keyboard input - E
	if(glfwGetKey(mWindow, GLFW_KEY_E) == GLFW_PRESS) {
		// Roll Left
		roll(1.0f*glm::pi<float>()*dt);
	}

	// Get Keyboard input - W
	if(glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS) {
		moveForward(mSpeed * dt);
	}
	// Get Keyboard input - S
	if(glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS) {
		// Move Backward
		moveBackward(mSpeed * dt);
	}

	// Get Keyboard input - D
	if(glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS) {
		// Move Right
		moveRight(mSpeed * dt);
	}
	// Get Keyboard input - A
	if(glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS) {
		// Move Left
		moveLeft(mSpeed * dt);
	}
}

// Movement
void FreeLookCamera::moveForward(float x) {
	// Generate Forward Vector
	glm::vec3 forward = glm::vec3(mOrientation * glm::vec4( 0,  0, -1,  0));

	// Move Forward
	mPosition += forward * x;
}
void FreeLookCamera::moveBackward(float x) {
	// Generate Forward Vector
	glm::vec3 forward = glm::vec3(mOrientation * glm::vec4( 0,  0, -1,  0));

	// Move Backward
	mPosition -= forward * x;
}
void FreeLookCamera::moveRight(float x) {
	// Generate Right Vector
	glm::vec3 right = glm::vec3(mOrientation * glm::vec4( 1,  0,  0,  0));

	// Move Right
	mPosition += right * x;
}
void FreeLookCamera::moveLeft(float x) {
	// Generate Right Vector
	glm::vec3 right = glm::vec3(mOrientation * glm::vec4( 1,  0,  0,  0));

	// Move Left
	mPosition -= right * x;
}

// Rotations
void FreeLookCamera::pitch(float angle) {
	// Pitch
	mOrientation *= glm::quat(cos(angle/2.0f), glm::sin(angle/2.0f), 0.0f, 0.0f);
}
void FreeLookCamera::yaw(float angle) {
	// Yaw
	mOrientation *= glm::quat(cos(angle/2.0f), 0.0f, glm::sin(angle/2.0f), 0.0f);
}
void FreeLookCamera::roll(float angle) {
	// Roll
	mOrientation *= glm::quat(cos(angle/2.0f), 0.0f, 0.0f, -glm::sin(angle/2.0f));
}

// --------------------------------------------------------------------------------
// Gimbal Free Look Camera
// --------------------------------------------------------------------------------
// Constructor
GimbalFreeLookCamera::GimbalFreeLookCamera(GLFWwindow *window) : FreeLookCamera(window) {
	// View Angles
	mViewAngleX = 0.0f;
	mViewAngleY = 0.0f;
}

// Rotations
void GimbalFreeLookCamera::pitch(float angle) {
	// Update View Angle X
	mViewAngleX += angle;

	// Bounds on View Angle X
	if(mViewAngleX < -glm::pi<float>() / 2.0f) {
		mViewAngleX = -glm::pi<float>() / 2.0f;
	} else if(mViewAngleX > glm::pi<float>() / 2.0f) {
		mViewAngleX = glm::pi<float>() / 2.0f;
	}

	// Generate Orientation
	mOrientation = glm::quat(cos(mViewAngleY/2.0f), glm::vec3(0.0f, glm::sin(mViewAngleY/2.0f), 0.0f)) * 
				   glm::quat(cos(mViewAngleX/2.0f), glm::vec3(glm::sin(mViewAngleX/2.0f), 0.0f, 0.0f));
}
void GimbalFreeLookCamera::yaw(float angle) {
	// Update View Angle Y
	mViewAngleY += angle;

	// Generate Orientation
	mOrientation = glm::quat(cos(mViewAngleY/2.0f), glm::vec3(0.0f, glm::sin(mViewAngleY/2.0f), 0.0f)) * 
				   glm::quat(cos(mViewAngleX/2.0f), glm::vec3(glm::sin(mViewAngleX/2.0f), 0.0f, 0.0f));
}
void GimbalFreeLookCamera::roll(float angle) {
	// No Roll for Gimbal Free Look Camera
}
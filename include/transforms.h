#ifndef TRANSFORMS_H
#define TRANSFORMS_H

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

// --------------------------------------------------------------------------------
// Transform Functions
// --------------------------------------------------------------------------------

// Create an identity matrix
void identity(float I[16]);

// Create a translation matrix with (x,y,z)
void translate(float tx, float ty, float tz, float T[16]);

// Create a rotation matrix around the X-axis
void rotateX(float theta, float Rx[16]);

// Create a rotation matrix around the Y-axis
void rotateY(float theta, float Ry[16]);

// Create a rotation matrix around the Z-axis
void rotateZ(float theta, float Rz[16]);

// Create a rotation matrix around arbitrary axis (rx, ry, rz)
void rotate(float theta, float rx, float ry, float rz, float R[16]);

// Create a scale matrix
void scale(float sx, float sy, float sz, float S[16]);

// Multiply matrix a * b to give c
void multiply44(float a[16], float b[16], float c[16]);

// Multiply a vector by a scalar
void multiply3(float s, float u[3], float v[3]);

// Multiply a vector by a scalar
void multiply4(float s, float u[4], float v[4]);

// Calculate the length of a vector
float length3(float v[3]);

// Calculate the length of a vector
float length4(float v[4]);

// Normalize a vector
void normalize(float v[3], float u[3]);

// Calculate the cross product of two vectors
void cross_product(float a[3], float b[3], float c[3]);

// Calculate the dot product of two vectors
float dot_product3(float a[3], float b[3]);

// Calculate the dot product of two vectors
float dot_product4(float a[4], float b[4]);

// View Transform - forwards and up must be unit vectors
void view(float p[4], float f[3], float u[3], float V[16]);

// Create an Orthographic Projection matrix
void orthographic(float width, float height, float near1, float far1, float matrix[16]);

// Create a Perspective Projection matrix
void perspective(float aspect, float fov, float near1, float far1, float matrix[16]);

// --------------------------------------------------------------------------------

#endif // TRANSFORMS_H

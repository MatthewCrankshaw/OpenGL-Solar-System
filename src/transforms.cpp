#include "transforms.h"

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

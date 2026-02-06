#include "camera.h"

#include "main/includes.h"
#include <iostream>

Camera::Camera()
{
	view_matrix.SetIdentity();
	SetOrthographic(-1,1,1,-1,-1,1);
}

Vector3 Camera::GetLocalVector(const Vector3& v)
{
	Matrix44 iV = view_matrix;
	if (iV.Inverse() == false)
		std::cout << "Matrix Inverse error" << std::endl;
	Vector3 result = iV.RotateVector(v);
	return result;
}

Vector3 Camera::ProjectVector(Vector3 pos)
{
	Vector4 pos4 = Vector4(pos.x, pos.y, pos.z, 1.0);
	Vector4 result = viewprojection_matrix * pos4;
	if (type == ORTHOGRAPHIC)
		return result.GetVector3();
	else
		return result.GetVector3() / result.w;
}

void Camera::Rotate(float angle, const Vector3& axis)
{
	Matrix44 R;
	R.MakeRotationMatrix(angle, axis);
	Vector3 new_front = R * (center - eye);
	center = eye + new_front;
	UpdateViewMatrix();
}

void Camera::Move(Vector3 delta)
{
	Vector3 localDelta = GetLocalVector(delta);
	eye = eye - localDelta;
	center = center - localDelta;
	UpdateViewMatrix();
}

void Camera::SetOrthographic(float left, float right, float top, float bottom, float near_plane, float far_plane)
{
	type = ORTHOGRAPHIC;

	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	UpdateProjectionMatrix();
}

void Camera::SetPerspective(float fov, float aspect, float near_plane, float far_plane)
{
	type = PERSPECTIVE;

	this->fov = fov;
	this->aspect = aspect;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	UpdateProjectionMatrix();
}

void Camera::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	this->eye = eye;
	this->center = center;
	this->up = up;

	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	// we also can use MakeTranslationMatrix and MakeRotationMatrix, in instead of putting each coordinates!
	// Reset Matrix (Identity)
	view_matrix.SetIdentity();

	/*
	Vector3 zc = (eye - center).Normalize(); // Forward (apuntant cap a la càmera)
	Vector3 xc = up.Cross(zc).Normalize();   // Right
	Vector3 yc = zc.Cross(xc).Normalize();   // Up real

	// Orientació (Files)

	view_matrix.M[0][0] = xc.x;  view_matrix.M[0][1] = xc.y;  view_matrix.M[0][2] = xc.z;
	view_matrix.M[1][0] = yc.x;  view_matrix.M[1][1] = yc.y;  view_matrix.M[1][2] = yc.z;
	view_matrix.M[2][0] = zc.x;  view_matrix.M[2][1] = zc.y;  view_matrix.M[2][2] = zc.z;

	// Translació: És el producte escalar negatiu de la posició pels eixos
	view_matrix.M[0][3] = -xc.Dot(eye);
	view_matrix.M[1][3] = -yc.Dot(eye);
	view_matrix.M[2][3] = -zc.Dot(eye);

	view_matrix.M[3][3] = 1.0f;*/

	
	// Comment this line to create your own projection matrix!
		//SetExampleViewMatrix();

	// Remember how to fill a Matrix4x4 (check framework slides)

	// Careful with the order of matrix multiplications, and be sure to use normalized vectors!
	Vector3 oc = eye; // origin point 

	// define the vectors that will construct the Mcam
	Vector3 z = eye - center;
	Vector3 zc = z.Normalize();

	Vector3 x =  zc.Cross(up);
	Vector3 xc = x.Normalize();

	Vector3 yc = zc.Cross(xc);


	// Create the view matrix rotation
	this->view_matrix.M[0][0] = xc.x;
	this->view_matrix.M[1][0] = xc.y;
	this->view_matrix.M[2][0] = xc.z;
	this->view_matrix.M[3][0] = 0; // as it is a vector

	this->view_matrix.M[0][1] = yc.x;
	this->view_matrix.M[1][1] = yc.y;
	this->view_matrix.M[2][1] = yc.z;
	this->view_matrix.M[3][1] = 0; // as it is a vector

	this->view_matrix.M[0][2] = zc.x;
	this->view_matrix.M[1][2] = zc.y;
	this->view_matrix.M[2][2] = zc.z;
	this->view_matrix.M[3][2] = 0; // as it is a vector

	this->view_matrix.M[0][3] = oc.x; //-xc.Dot(eye); ?
	this->view_matrix.M[1][3] = oc.y; //-yc.Dot(eye);
	this->view_matrix.M[2][3] = oc.z; //-zc.Dot(eye);
	this->view_matrix.M[3][3] = 1; // as it is a point

	// Translate view matrix
	////////////////////////////////////////////////
	// CAL REVISAR SIGNES DE oc I LO DE LA INVERSA!!
	/////////////////////////////////////////////////

	this->view_matrix.Inverse();
	
	UpdateViewProjectionMatrix();
}

// Create a projection matrix
void Camera::UpdateProjectionMatrix()
{
	// Reset Matrix (Identity)
	projection_matrix.SetIdentity();

	// Comment this line to create your own projection matrix!
		//SetExampleProjectionMatrix();

	float fRad = fov * (PI / 180.0f); // as it says at the camera.h normally is set in degrees -> so we have to transform it into radians
	float f = 1 / tan(fRad / 2);
	// Remember how to fill a Matrix4x4 (check framework slides)
	if (type == PERSPECTIVE) {

		projection_matrix.M[0][0] = f / aspect;
		projection_matrix.M[1][1] = f;
		projection_matrix.M[2][2] = (far_plane + near_plane) / (near_plane - far_plane);

		projection_matrix.M[2][3] = 2*(far_plane * near_plane) / (near_plane - far_plane);
		projection_matrix.M[3][2] = -1;
		projection_matrix.M[3][3] = 0; // as it initially is set as a identity matrix we should rectify the 1 -> 0

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//CAL FER LA MULTIPLICACIÓ ENTRE Mpers = Morto * P (ON P ES MATRIU AUXILIAR AQUESTA D'AQUI A DALT LITERALMENTT)
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	}
	else if (type == ORTHOGRAPHIC) {
		projection_matrix.M[0][0] = 2 / (right - left);
		projection_matrix.M[1][1] = 2 / (top - bottom);
		projection_matrix.M[2][2] = -2 / (far_plane - near_plane);

		projection_matrix.M[0][3] = - (right + left) / (right - left);
		projection_matrix.M[1][3] = - (top + bottom) / (top - bottom);
		projection_matrix.M[2][3] = - (far_plane + near_plane) / (far_plane - near_plane);

	} 

	UpdateViewProjectionMatrix();
}

void Camera::UpdateViewProjectionMatrix()
{
	viewprojection_matrix = projection_matrix * view_matrix;
}

Matrix44 Camera::GetViewProjectionMatrix()
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();

	return viewprojection_matrix;
}

// The following methods have been created for testing.
// Do not modify them.

void Camera::SetExampleViewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix.m );
}

void Camera::SetExampleProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (type == PERSPECTIVE)
		gluPerspective(fov, aspect, near_plane, far_plane);
	else
		glOrtho(left,right,bottom,top,near_plane,far_plane);

	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix.m );
	glMatrixMode(GL_MODELVIEW);
}

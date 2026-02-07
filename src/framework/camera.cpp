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
    view_matrix.SetIdentity();

    Vector3 f = (center - eye).Normalize();   // forward
    Vector3 s = f.Cross(up).Normalize();      // right
    Vector3 u = s.Cross(f);                   // real up

    // Matrix44::M is indexed as M[column][row]
    // So columns store the basis vectors, and column 3 stores translation

    // Column 0 = right (s)
    view_matrix.M[0][0] =  s.x;
    view_matrix.M[0][1] =  s.y;
    view_matrix.M[0][2] =  s.z;
    view_matrix.M[0][3] = -s.Dot(eye);

    // Column 1 = up (u)
    view_matrix.M[1][0] =  u.x;
    view_matrix.M[1][1] =  u.y;
    view_matrix.M[1][2] =  u.z;
    view_matrix.M[1][3] = -u.Dot(eye);

    // Column 2 = -forward (-f)
    view_matrix.M[2][0] = -f.x;
    view_matrix.M[2][1] = -f.y;
    view_matrix.M[2][2] = -f.z;
    view_matrix.M[2][3] =  f.Dot(eye);

    // Column 3 = (0,0,0,1)
    view_matrix.M[3][0] = 0.f;
    view_matrix.M[3][1] = 0.f;
    view_matrix.M[3][2] = 0.f;
    view_matrix.M[3][3] = 1.f;

    UpdateViewProjectionMatrix();
}



// Create a projection matrix
void Camera::UpdateProjectionMatrix()
{
    projection_matrix.SetIdentity();

    if (type == PERSPECTIVE)
    {
        float fRad = fov * (PI / 180.0f);
        float f = 1.0f / tanf(fRad * 0.5f);

        // Column-major (M[col][row])

        projection_matrix.M[0][0] = f / aspect;
        projection_matrix.M[1][1] = f;

        projection_matrix.M[2][2] = (far_plane + near_plane) / (near_plane - far_plane);
        projection_matrix.M[2][3] = (2.0f * far_plane * near_plane) / (near_plane - far_plane);

        projection_matrix.M[3][2] = -1.0f;
        projection_matrix.M[3][3] = 0.0f;
    }
    else
    {
        projection_matrix.M[0][0] = 2.f / (right - left);
        projection_matrix.M[1][1] = 2.f / (top - bottom);
        projection_matrix.M[2][2] = -2.f / (far_plane - near_plane);

        projection_matrix.M[3][0] = -(right + left) / (right - left);
        projection_matrix.M[3][1] = -(top + bottom) / (top - bottom);
        projection_matrix.M[3][2] = -(far_plane + near_plane) / (far_plane - near_plane);
        projection_matrix.M[3][3] = 1.f;
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

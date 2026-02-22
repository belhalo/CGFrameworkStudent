#include "camera.h"
#include "framework.h"

#include "main/includes.h"
#include <iostream>
#include <cmath>

// lab1
// camera holds view and projection math shared across labs, later labs call projectvector and use viewprojection matrix

Camera::Camera()
{
    // lab1
    // initialize matrices to valid identity, start with orthographic so camera is usable even before perspective set
    view_matrix.SetIdentity();
    SetOrthographic(-1, 1, 1, -1, -1, 1);
}

Vector3 Camera::GetLocalVector(const Vector3& v)
{
    // lab1
    // convert a vector expressed in camera space into world space using inverse view rotation
    Matrix44 iV = view_matrix;
    if (iV.Inverse() == false)
        std::cout << "Matrix Inverse error" << std::endl;

    Vector3 result = iV.RotateVector(v);
    return result;
}

Vector3 Camera::ProjectVector(Vector3 pos)
{
    // lab2
    // project world space position into ndc space using viewprojection, perspective divide for perspective camera
    Vector4 pos4 = Vector4(pos.x, pos.y, pos.z, 1.0f);
    Vector4 result = viewprojection_matrix * pos4;

    if (type == ORTHOGRAPHIC)
        return result.GetVector3();

    return result.GetVector3() / result.w; // NDC in [-1,1]
}

void Camera::Rotate(float angle, const Vector3& axis)
{
    // lab2
    // rotate camera direction by rotating vector from eye to center around axis, eye stays fixed
    Matrix44 R;
    R.MakeRotationMatrix(angle, axis);
    Vector3 new_front = R * (center - eye);
    center = eye + new_front;
    UpdateViewMatrix();
}

void Camera::Move(Vector3 delta)
{
    // lab2
    // move camera in local camera axes, delta is interpreted as camera local and converted to world
    Vector3 localDelta = GetLocalVector(delta);
    eye = eye - localDelta;
    center = center - localDelta;
    UpdateViewMatrix();
}

void Camera::SetOrthographic(float left, float right, float top, float bottom, float near_plane, float far_plane)
{
    // lab1
    // set orthographic parameters and rebuild projection matrix
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
    // lab2
    // set perspective parameters and rebuild projection matrix, used by raster pipeline in lab2 and lab3
    type = PERSPECTIVE;

    this->fov = fov;
    this->aspect = aspect;
    this->near_plane = near_plane;
    this->far_plane = far_plane;

    UpdateProjectionMatrix();
}

void Camera::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
{
    // lab2
    // set camera pose then rebuild view matrix
    this->eye = eye;
    this->center = center;
    this->up = up;

    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    // lab2
    // build view matrix from camera basis and translation, result transforms world to camera space
    view_matrix.SetIdentity();
    rotation_matrix.SetIdentity();
    translation_matrix.SetIdentity();

    // lab2
    // forward points from center to eye in this convention, camera looks along negative forward
    Vector3 forward = eye - center;
    forward.Normalize();

    // lab2
    // right and top define orthonormal basis using cross products, assumes up not parallel to forward
    Vector3 right = up.Cross(forward);
    Vector3 top = forward.Cross(right);

    // lab2
    // store basis into rotation matrix, matrix storage is column major in this framework
    rotation_matrix.M[0][0] = right.x;
    rotation_matrix.M[1][0] = right.y;
    rotation_matrix.M[2][0] = right.z;

    rotation_matrix.M[0][1] = top.x;
    rotation_matrix.M[1][1] = top.y;
    rotation_matrix.M[2][1] = top.z;

    rotation_matrix.M[0][2] = forward.x;
    rotation_matrix.M[1][2] = forward.y;
    rotation_matrix.M[2][2] = forward.z;

    // lab2
    // translate world by negative eye so camera is treated as origin in camera space
    translation_matrix.MakeTranslationMatrix(-eye.x, -eye.y, -eye.z);

    // lab2
    // view matrix is rotation times translation in this convention
    view_matrix = rotation_matrix * translation_matrix;

    UpdateViewProjectionMatrix();
}

void Camera::UpdateProjectionMatrix()
{
    // lab2
    // build projection matrix, perspective is required for lab2 and lab3 pipeline
    projection_matrix.SetIdentity();

    if (type == PERSPECTIVE)
    {
        // lab2
        // f is cotangent of half fov, scales x and y into clip space
        float f = 1.0f / tanf((fov * 0.5f) * (PI / 180.0f));

        // lab2
        // standard perspective projection, maps camera space into clip space, z mapped using near and far
        projection_matrix.Set(
            f / aspect, 0, 0, 0,
            0, f, 0, 0,
            0, 0, (far_plane + near_plane) / (near_plane - far_plane), (2 * far_plane * near_plane) / (near_plane - far_plane),
            0, 0, -1.0f, 0
        );
    }
    else
    {
        // lab1
        // orthographic matrix not implemented here, current labs use perspective in application init
        // orthographic still available via SetExampleProjectionMatrix for reference testing
    }

    UpdateViewProjectionMatrix();
}

void Camera::UpdateViewProjectionMatrix()
{
    // lab2
    // cache combined matrix so projectvector can be a single multiply plus optional perspective divide
    viewprojection_matrix = projection_matrix * view_matrix;
}

Matrix44 Camera::GetViewProjectionMatrix()
{
    // lab2
    // return latest combined matrix, recompute both to stay consistent with current camera parameters
    UpdateViewMatrix();
    UpdateProjectionMatrix();
    return viewprojection_matrix;
}

// lab1
// opengl reference functions used for checking against fixed pipeline results, not part of lab implementation

void Camera::SetExampleViewMatrix()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
    glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix.m);
}

void Camera::SetExampleProjectionMatrix()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (type == PERSPECTIVE)
        gluPerspective(fov, aspect, near_plane, far_plane);
    else
        glOrtho(left, right, bottom, top, near_plane, far_plane);

    glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix.m);
    glMatrixMode(GL_MODELVIEW);
}

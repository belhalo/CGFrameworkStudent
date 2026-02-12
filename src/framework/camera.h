#pragma once

#include "framework.h"

class Camera
{
    // lab1 opengl reference functions used for testing matrix correctness
    void SetExampleViewMatrix();
    void SetExampleProjectionMatrix();

public:
    // lab1 camera projection type selector
    enum { PERSPECTIVE, ORTHOGRAPHIC };
    char type;

    // lab1 camera position orientation vectors in world space
    Vector3 eye;
    Vector3 center;
    Vector3 up;

    // lab1 perspective parameters field of view aspect near far planes
    float fov;
    float aspect;
    float near_plane;
    float far_plane;

    // lab1 orthographic projection bounds
    float left, right, top, bottom;

    // lab1 matrices used in transformation pipeline
    // view matrix transforms world to camera space
    // projection matrix transforms camera to clip space
    // viewprojection matrix combines both
    Matrix44 view_matrix;
    Matrix44 rotation_matrix;
    Matrix44 translation_matrix;
    Matrix44 projection_matrix;
    Matrix44 viewprojection_matrix;

    // lab1 constructor initializes matrices and default projection
    Camera();

    // lab1 update aspect ratio used in projection matrix
    void SetAspectRatio(float aspect) { this->aspect = aspect; }

    // lab2 translate camera in local camera space
    void Move(Vector3 delta);

    // lab2 rotate camera around axis passing through eye
    void Rotate(float angle, const Vector3& axis);

    // lab2 convert camera local vector into world space direction
    Vector3 GetLocalVector(const Vector3& v);

    // lab2 project world position into normalized device coordinates
    Vector3 ProjectVector(Vector3 pos);

    // lab1 configure perspective projection parameters
    void SetPerspective(float fov, float aspect, float near_plane, float far_plane);

    // lab1 configure orthographic projection parameters
    void SetOrthographic(float left, float right, float top, float bottom, float near_plane, float far_plane);

    // lab1 update camera pose using eye center and up vectors
    void LookAt(const Vector3& eye, const Vector3& center, const Vector3& up);

    // lab1 recompute view matrix from camera pose
    void UpdateViewMatrix();

    // lab1 recompute projection matrix from camera parameters
    void UpdateProjectionMatrix();

    // lab1 recompute combined view projection matrix
    void UpdateViewProjectionMatrix();

    // lab2 return combined view projection matrix
    Matrix44 GetViewProjectionMatrix();
};

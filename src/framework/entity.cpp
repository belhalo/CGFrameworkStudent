#include "entity.h"
#include "texture.h"
#include <algorithm>
#include <cmath>

// lab2
// frustum reject helper in ndc space
// triangle rejected only when all three vertices fall outside same clipping plane
// avoids dropping triangles that straddle frustum boundary
bool Entity::TriangleOutsideNDC(const Vector3& a, const Vector3& b, const Vector3& c)
{
    // lab2
    // each test checks one plane of ndc cube, range is -1 to 1 in x y z after projection and divide
    if (a.x < -1 && b.x < -1 && c.x < -1) return true; // left
    if (a.x >  1 && b.x >  1 && c.x >  1) return true; // right
    if (a.y < -1 && b.y < -1 && c.y < -1) return true; // bottom
    if (a.y >  1 && b.y >  1 && c.y >  1) return true; // top
    if (a.z < -1 && b.z < -1 && c.z < -1) return true; // near in ndc
    if (a.z >  1 && b.z >  1 && c.z >  1) return true; // far in ndc
    return false;
}

// lab2
// entity initialization, store shared mesh pointer and initial placement matrix
// baseMatrix kept for animation so translation stays stable
void Entity::EntityAdd(Mesh* m, const Matrix44& M)
{
    mesh = m;
    baseMatrix = M;
    modelMatrix = M;
}

// lab2
// simple animation, accumulate time and rotate around y axis
// phase offsets rotation between entities in multi mode
void Entity::Update(float dt)
{
    totalTime += dt;

    Matrix44 R;
    R.MakeRotationMatrix(totalTime + phase, Vector3(0, 1, 0));

    // lab2
    // rotate relative to base placement, preserves original translation and orientation fix
    modelMatrix = baseMatrix * R;
}

// lab 4
// render the entity mesh using gpu 
void Entity::Render(Camera* camera) {
    // guard against null pointers
    if (!mesh || !camera || !material) return;

    // computing the transformations 
    Matrix44 Ry; Ry.MakeRotationMatrix(userYaw, Vector3(0, 1, 0));
    Matrix44 Rx; Rx.MakeRotationMatrix(userPitch, Vector3(1, 0, 0));
    Matrix44 M = modelMatrix * Ry * Rx;

    // prepare the files to be ready to use by the gpu
    material->Enable();

    // enter the necessary data to the shader to compute the triangles data (vertices, uv, zbuffer, texels)
    material->SetMatrix44("u_model", M);
    material->SetMatrix44("u_viewprojection", camera->viewprojection_matrix);

    if (texture) {
        material->SetTexture("u_texture", texture);
    }

    // gpu renders the entity at the screen
    mesh->Render();

    // deseactive the path 
    material->Disable();
}

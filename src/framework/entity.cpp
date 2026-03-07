#include "entity.h"
#include <cmath>

void Entity::EntityAdd(Mesh* m, const Matrix44& M)
{
    mesh = m;
    baseMatrix = M;
    modelMatrix = M;
}

void Entity::Update(float dt)
{
    totalTime += dt;

    Matrix44 R;
    R.MakeRotationMatrix(totalTime + phase, Vector3(0, 1, 0));

    modelMatrix = baseMatrix * R;
}

void Entity::Render(sUniformData& uniformData)
{
    if (!mesh || !material)
        return;

    Matrix44 Ry;
    Matrix44 Rx;
    Ry.MakeRotationMatrix(userYaw, Vector3(0, 1, 0));
    Rx.MakeRotationMatrix(userPitch, Vector3(1, 0, 0));

    Matrix44 M = modelMatrix * Ry * Rx;

    uniformData.modelMatrix = M;

    material->Enable(uniformData);
    mesh->Render();
    material->Disable();
}

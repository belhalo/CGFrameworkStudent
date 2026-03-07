#pragma once

#include "framework.h"
#include "mesh.h"
#include "material.h"

class Entity
{
public:
    Mesh* mesh = nullptr;

    Matrix44 modelMatrix;
    Matrix44 baseMatrix;

    Material* material = nullptr;

    float userYaw = 0.0f;
    float userPitch = 0.0f;

    float totalTime = 0.0f;
    float phase = 0.0f;

    Entity()
    {
        modelMatrix.SetIdentity();
        baseMatrix.SetIdentity();
    }

    void SetUserRotation(float yaw, float pitch)
    {
        userYaw = yaw;
        userPitch = pitch;
    }

    void EntityAdd(Mesh* m, const Matrix44& M);
    void Render(sUniformData& uniformData);
    void Update(float seconds_elapsed);
};

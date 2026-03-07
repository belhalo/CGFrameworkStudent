#pragma once

#include "main/includes.h"
#include "framework.h"
#include "shader.h"
#include "texture.h"

struct sLight
{
    Vector3 position;
    Vector3 intensity;
};

struct sUniformData
{
    Matrix44 modelMatrix;
    Matrix44 viewProjectionMatrix;
    Vector3 ambientLightIntensity;
    sLight sceneLight;
    Vector3 cameraEye;
};

class Material
{
public:
    Shader* shader = nullptr;

    Texture* colorTexture = nullptr;
    Texture* specularTexture = nullptr;
    Texture* normalTexture = nullptr;

    Vector3 Ka = Vector3(1.0f, 1.0f, 1.0f);
    Vector3 Kd = Vector3(1.0f, 1.0f, 1.0f);
    Vector3 Ks = Vector3(1.0f, 1.0f, 1.0f);

    float shininess = 32.0f;

    void Enable(const sUniformData& uniformData);
    void Disable();
};

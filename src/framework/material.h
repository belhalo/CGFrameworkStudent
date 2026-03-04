#pragma once

#include "main/includes.h"
#include "shader.h"
#include "camera.h"
#include "application.h"

// instance in app.h
typedef struct sUniformData {
	Matrix44 modelMatrix; 
	Matrix44 viewProjectionMatrix;
	Vector3 ambientLightIntensity;
	sLight sceneLights;
	Vector3 cameraEye;
}sUniformData;

class Material {
public:
	Shader* shader = nullptr;
	Texture* texture = nullptr;

	//  phong illumination model
	Vector3 Ka = { 1.0f, 1.0f, 1.0f }; // ambient component
	Vector3  Kd = { 1.0f, 1.0f, 1.0f }; // diffuse component
	Vector3 Ks = { 1.0f, 1.0f, 1.0f }; // specular component

	float shininess = 2.0f;

	// controlers of the shader  
	void Material::Enable(const sUniformData& uniformData);
	void Material::Disable();
};
#include "material.h"

// upload all uniforms to the gpu!
void Material::Enable(const sUniformData& uniformData){
	if (!shader) return;

	this->shader->Enable();

	// set all the attr of sUniformData
	shader->SetMatrix44("u_model", uniformData.modelMatrix);
	shader->SetMatrix44("u_viewProjection", uniformData.viewProjectionMatrix);

	shader->SetVector3("u_ambientLight", uniformData.ambientLightIntensity);

	shader->SetVector3("u_sceneLight_position", uniformData.sceneLights.position);
	shader->SetVector3("u_sceneLight_intensity", uniformData.sceneLights.intensity);

	// set the rest of light attr
		// we do it separatly because we can get only one of them as the others are null
		// so we still want put that k that is != null
	if (Ka.x != 0 && Ka.y != 0 && Ka.z != 0) shader->SetVector3("u_Ka", Ka);
	if (Kd.x != 0 && Kd.y != 0 && Kd.z != 0) shader->SetVector3("u_Kd", Kd);
	if (Ks.x != 0 && Ks.y != 0 && Ks.z != 0) shader->SetVector3("u_Ks", Ks);

	shader->SetVector3("u_camera_position", uniformData.cameraEye);

	shader->SetFloat("u_shininess", shininess);
}

void Material::Disable() {
	this->shader->Disable();
}
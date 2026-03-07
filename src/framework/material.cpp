#include "material.h"

void Material::Enable(const sUniformData& uniformData)
{
    if (!shader)
        return;

    shader->Enable();

    shader->SetMatrix44("u_model", uniformData.modelMatrix);
    shader->SetMatrix44("u_viewprojection", uniformData.viewProjectionMatrix);

    shader->SetVector3("u_ambientLight", uniformData.ambientLightIntensity);
    shader->SetVector3("u_sceneLight_position", uniformData.sceneLight.position);
    shader->SetVector3("u_sceneLight_intensity", uniformData.sceneLight.intensity);
    shader->SetVector3("u_camera_position", uniformData.cameraEye);

    shader->SetVector3("u_Ka", Ka);
    shader->SetVector3("u_Kd", Kd);
    shader->SetVector3("u_Ks", Ks);
    shader->SetFloat("u_shininess", shininess);

    shader->SetInt("u_use_color_texture", useColorTexture ? 1 : 0);
    shader->SetInt("u_use_specular_texture", useSpecularTexture ? 1 : 0);
    shader->SetInt("u_use_normal_texture", useNormalTexture ? 1 : 0);

    if (colorTexture)
        shader->SetTexture("u_texture", colorTexture);

    if (normalTexture)
        shader->SetTexture("u_normal_texture", normalTexture);
}

void Material::Disable()
{
    if (shader)
        shader->Disable();
}

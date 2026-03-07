#pragma once

#include "main/includes.h"
#include "framework.h"
#include "camera.h"
#include "entity.h"
#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include "image.h"
#include "material.h"

class Application
{
public:
    SDL_Window* window = nullptr;

    int window_width = 0;
    int window_height = 0;

    float time = 0.0f;

    Camera* cam = nullptr;

    // lab 4 quad path
    Shader* shader = nullptr;
    Mesh* quad = nullptr;
    Texture* quadTex = nullptr;

    // shared mesh textures
    Texture* annaColorTex = nullptr;
    Texture* annaNormalTex = nullptr;

    // materials
    Material* rasterMaterial = nullptr;
    Material* gouraudMaterial = nullptr;
    Material* phongMaterial = nullptr;

    // lab 4 controls
    int currentTask = 1;
    int currentSubTask = 1;

    // scene switch
    bool lab5Scene = false;

    // lab 5 controls
    enum ShadingMode
    {
        SHADING_GOURAUD,
        SHADING_PHONG
    };

    ShadingMode shadingMode = SHADING_GOURAUD;

    bool useColorTexture = false;
    bool useSpecularTexture = false;
    bool useNormalTexture = false;
    int numLights = 1;

    // scene entities
    std::vector<Entity*> entities;

    // shared uniform data for lab 5
    sUniformData uniformData;
    Vector3 ambientLightIntensity = Vector3(0.15f, 0.15f, 0.15f);
    sLight mainLight = { Vector3(2.0f, 2.0f, 2.0f), Vector3(8.0f, 8.0f, 8.0f) };

    const Uint8* keystate = nullptr;

    int mouse_state = 0;
    Vector2 mouse_position;
    Vector2 mouse_delta;

    float orbitYaw = 0.0f;
    float orbitPitch = 0.0f;
    float orbitDist = 2.2f;

    float orbitSpeed = 0.008f;
    float panSpeed = 1.0f;
    float zoomSpeed = 0.10f;

    float modelYaw = 0.0f;
    float modelPitch = 0.0f;
    float modelRotateSpeed = 0.008f;

    Application(const char* caption, int width, int height);
    ~Application();

    void Init(void);
    void Render(void);
    void Update(float dt);

    void OnKeyPressed(SDL_KeyboardEvent event);
    void OnMouseButtonDown(SDL_MouseButtonEvent event);
    void OnMouseButtonUp(SDL_MouseButtonEvent event);
    void OnMouseMove(SDL_MouseMotionEvent event);
    void OnWheel(SDL_MouseWheelEvent event);

    void OnFileChanged(const char* filename);
    void UpdateCameraFromOrbit();

    void SetWindowSize(int width, int height)
    {
        glViewport(0, 0, width, height);
        window_width = width;
        window_height = height;

        if (cam)
        {
            float aspect = (float)window_width / (float)window_height;
            cam->SetPerspective(cam->fov, aspect, cam->near_plane, cam->far_plane);
        }
    }

    Vector2 GetWindowSize()
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        return Vector2((float)w, (float)h);
    }
};

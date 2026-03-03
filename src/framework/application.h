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
    // Lab 1 window handle for rendering output
    SDL_Window* window = nullptr;

    // current window size in pixels
    int window_width = 0;
    int window_height = 0;

    // accumulated time (used by shader animations)
    float time = 0.f;

    // camera
    Camera* cam = nullptr;

    // lab 4
    Shader* shader = nullptr;       // quad shader
    Mesh* quad = nullptr;           // fullscreen quad mesh

    Texture* quadTex = nullptr;     // fruits texture for tasks 2.3 and 2.4
    Texture* annaTex = nullptr;     // obj texture for task 2.5

    Shader* sourceMat = nullptr;    // obj material shader

    // interactivity controls
    int currentTask = 1;        // 1..4
    int currentSubTask = 1;     // 1..6 (a..f)

    // Lab 4 / Lab 5 scene switch
    bool lab5Scene = false;

    // scene entities (GPU mesh rendering)
    std::vector<Entity*> entities;

    // Lab 5 agroupation of data
    sUniformData uniformData;

    // Lab 5 intensity of ambient light in the scene
    sLight Ia = { {0,0,0},{0,0,0} };

    // keyboard state pointer from SDL
    const Uint8* keystate = nullptr;

    // mouse button state mask
    int mouse_state = 0;

    // last mouse position in window space
    Vector2 mouse_position;

    // mouse delta storage
    Vector2 mouse_delta;

    // orbit camera parameters
    float orbitYaw = 0.f;       // radians
    float orbitPitch = 0.f;     // radians
    float orbitDist = 2.2f;     // distance to target

    // input sensitivity parameters
    float orbitSpeed = 0.008f;
    float panSpeed   = 1.0f;
    float zoomSpeed  = 0.10f;

    // model rotation values driven by middle drag
    float modelYaw = 0.f;
    float modelPitch = 0.f;
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

        // keep camera aspect correct 
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

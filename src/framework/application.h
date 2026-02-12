#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "camera.h"
#include "entity.h"

class Application
{
public:
    // lab1 window handle for rendering output
    SDL_Window* window = nullptr;

    // lab1 current window size in pixels
    int window_width = 0;
    int window_height = 0;

    // lab2 accumulated time used for animation timing
    float time = 0.f;

    // lab2 camera used for view and projection transforms
    Camera* cam = nullptr;

    // lab2 scene entities storing meshes transforms and render state
    std::vector<Entity*> entities;

    // lab1 keyboard state pointer from sdl
    const Uint8* keystate = nullptr;

    // lab1 mouse button state mask
    int mouse_state = 0;

    // lab1 last mouse position in window space
    Vector2 mouse_position;

    // lab1 optional mouse delta storage
    Vector2 mouse_delta;

    // lab2 render mode selector for single or multiple entities
    enum DrawMode { DRAW_SINGLE, DRAW_MULTI };
    DrawMode drawMode = DRAW_SINGLE;

    // lab2 selected camera property controlled by plus minus keys
    enum CameraProperty { PROP_NEAR, PROP_FAR, PROP_FOV };
    CameraProperty currentProp = PROP_FOV;

    // lab2 orbit camera parameters yaw pitch and distance
    float orbitYaw = 0.f;       // radians
    float orbitPitch = 0.f;     // radians
    float orbitDist = 2.2f;     // start closer by default

    // lab2 input sensitivity parameters tuned for trackpads
    float orbitSpeed = 0.008f;       // radians per pixel-ish (scaled in code)
    float panSpeed   = 1.0f;          // applied via world-per-pixel math
    float zoomSpeed  = 0.10f;         // used as exponential strength

    // lab2 model rotation values driven by middle drag
    float modelYaw = 0.f;
    float modelPitch = 0.f;
    float modelRotateSpeed = 0.008f;

    // lab1 cpu framebuffer storing color per pixel
    Image framebuffer;

    // lab3 depth buffer storing float depth per pixel
    FloatImage zbuffer;

    // lab1 construct application create window and buffers
    Application(const char* caption, int width, int height);

    // lab1 destroy camera and entities
    ~Application();

    // lab2 initialize scene assets and camera setup
    void Init(void);

    // lab1 render framebuffer contents to window
    void Render(void);

    // lab2 per frame update used for animation timing
    void Update(float dt);

    // lab1 keyboard handler toggling modes and camera parameters
    void OnKeyPressed(SDL_KeyboardEvent event);

    // lab1 mouse press handler storing initial position
    void OnMouseButtonDown(SDL_MouseButtonEvent event);

    // lab1 mouse release handler kept for interface completeness
    void OnMouseButtonUp(SDL_MouseButtonEvent event);

    // lab2 mouse motion handler for orbit pan and model rotation
    void OnMouseMove(SDL_MouseMotionEvent event);

    // lab2 mouse wheel handler controlling zoom
    void OnWheel(SDL_MouseWheelEvent event);

    // lab3 shader reload entrypoint when file changes
    void OnFileChanged(const char* filename);

    // lab2 recompute camera eye position from orbit parameters
    void UpdateCameraFromOrbit();

    // lab1 resize viewport and cpu buffers update camera aspect ratio
    void SetWindowSize(int width, int height)
    {
        glViewport(0, 0, width, height);
        window_width = width;
        window_height = height;

        framebuffer.Resize(width, height);
        zbuffer.Resize(width, height);

        if (cam)
        {
            float aspect = (float)window_width / (float)window_height;
            cam->SetPerspective(cam->fov, aspect, cam->near_plane, cam->far_plane);
        }
    }

    // lab1 query window size from sdl return as vector2
    Vector2 GetWindowSize()
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        return Vector2((float)w, (float)h);
    }
};

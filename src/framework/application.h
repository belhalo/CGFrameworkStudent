/*  
	+ This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "button.h"
#include "particleSystem.h"
#include "camera.h"
#include "entity.h"

class Application
{
public:
	// Window
	SDL_Window* window = nullptr;
	int window_width;
	int window_height;

	float time;

	// Camera
    Camera* cam = nullptr; // we create a general camera

	// Scene Objects
	std::vector<Entity*> entities; // we create a list of entities to control all of them in the scene

	// Input
	const Uint8* keystate = nullptr;
	int mouse_state = 0; // Tells which buttons are pressed
	Vector2 mouse_position; // Last mouse position
	Vector2 mouse_delta; // Mouse movement in the last frame
    
    //Lab2 Required interactivity
    enum DrawMode { DRAW_SINGLE, DRAW_MULTI };
    DrawMode drawMode = DRAW_SINGLE;

    enum CameraProperty { PROP_NEAR, PROP_FAR, PROP_FOV };
    CameraProperty currentProp = PROP_FOV;

    // orbit camera state
    bool orbiting = false;          // left drag
    float orbitYaw = 0.f;           // radians
    float orbitPitch = 0.f;         // radians
    float orbitDist = 3.f;          // distance from center

    // sensitivities
    float orbitSpeed = 0.005f;
    float zoomSpeed  = 0.2f;

    // Recompute cam->eye from yaw/pitch/dist around cam->center
    void UpdateCameraFromOrbit();

    // CPU framebuffer
    Image framebuffer;
    
    // lab3
    // Z-buffer (depth buffer) on CPU
    // stores 1 float per pixel, smaller = closer
    FloatImage zbuffer;
    
	// Constructor and main methods
	Application(const char* caption, int width, int height);
	~Application();

	void Init( void );
	void Render( void );
	void Update( float dt );
    
    void OnKeyPressed(SDL_KeyboardEvent event);
    void OnMouseButtonDown(SDL_MouseButtonEvent event);
    void OnMouseButtonUp(SDL_MouseButtonEvent event);
    void OnMouseMove(SDL_MouseButtonEvent event);
    void OnWheel(SDL_MouseWheelEvent event);
    void OnFileChanged(const char* filename);
    
    // Window resize hook
    // - Resize framebuffer and zbuffer
    // - Update camera aspect ratio
    void SetWindowSize(int width, int height)
    {
        glViewport(0, 0, width, height);
        window_width = width;
        window_height = height;

        framebuffer.Resize(width, height);

        // lab3
        // zbuffer must always match framebuffer size
        zbuffer.Resize(width, height);

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


#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"

#include <algorithm>
#include <cmath>
#include <iostream>

// helper clamp value inside range
static inline float Clamp(float v, float a, float b)
{
    return std::max(a, std::min(v, b));
}

Application::Application(const char* caption, int width, int height)
{
    // create window and graphics context
    window = createWindow(caption, width, height);

    // query actual window size from system
    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    // initialize input and timing values
    this->mouse_state = 0;
    this->time = 0.f;
    this->window_width = w;
    this->window_height = h;
    this->keystate = SDL_GetKeyboardState(nullptr);

    // resize framebuffer and zbuffer to window resolution
    this->framebuffer.Resize(w, h);
    this->zbuffer.Resize(w, h);

    // store initial mouse position
    mouse_position = Vector2(0.f, 0.f);
}

Application::~Application()
{
    // delete all entities created during init
    for (auto* e : entities) delete e;
    entities.clear();

    // delete camera instance
    delete cam;
    cam = nullptr;
}

void Application::UpdateCameraFromOrbit()
{
    if (!cam) return;

    // clamp pitch and distance to avoid flips and invalid zoom
    orbitPitch = Clamp(orbitPitch, -1.45f, 1.45f);
    orbitDist  = Clamp(orbitDist, 0.6f, 25.0f);

    // compute spherical coordinates offset from yaw pitch distance
    float cp = cosf(orbitPitch);

    Vector3 offset;
    offset.x = orbitDist * cp * cosf(orbitYaw);
    offset.y = orbitDist * sinf(orbitPitch);
    offset.z = orbitDist * cp * sinf(orbitYaw);

    // compute camera eye position around center
    cam->up = Vector3(0, 1, 0);
    cam->eye = cam->center + offset;

    // rebuild camera view matrix
    cam->LookAt(cam->eye, cam->center, Vector3(0, 1, 0));
    cam->UpdateViewMatrix();
}

void Application::Init(void)
{
    std::cout << "Initiating app..." << std::endl;

    // clear framebuffer once at start
    framebuffer.Fill(Color::BLACK);

    // create camera and configure projection
    cam = new Camera();
    float aspect = (float)window_width / (float)window_height;

    // initial camera position looking at origin
    cam->LookAt(
        Vector3(0.0f, 0.35f, 2.2f),   // eye position
        Vector3(0.0f, 0.35f, 0.0f),   // target center
        Vector3(0.0f, 1.0f, 0.0f)     // up direction
    );
    cam->UpdateViewMatrix();

    // configure perspective projection
    cam->SetPerspective(60.0f, aspect, 0.1f, 100.0f);

    // compute orbit parameters from current camera pose
    Vector3 d = cam->eye - cam->center;
    orbitDist = d.Length();
    orbitYaw = atan2f(d.z, d.x);
    orbitPitch = asinf(d.y / std::max(orbitDist, 0.0001f));

    // asset container storing mesh and texture pairs
    struct ModelAsset
    {
        const char* mesh_path;
        const char* tex_path;
        Mesh* mesh;
        Image* tex;
    };

    // list of models and textures to load
    ModelAsset assets[3] = {
        { "meshes/anna.obj", "textures/anna_color_specular.tga", nullptr, nullptr },
        { "meshes/cleo.obj", "textures/cleo_color_specular.tga", nullptr, nullptr },
        { "meshes/lee.obj",  "textures/lee_color_specular.tga",  nullptr, nullptr }
    };

    // load meshes and textures from disk
    for (int i = 0; i < 3; ++i)
    {
        assets[i].mesh = new Mesh();
        if (!assets[i].mesh->LoadOBJ(assets[i].mesh_path))
        {
            std::cout << "Mesh not found: " << assets[i].mesh_path << std::endl;
            delete assets[i].mesh;
            assets[i].mesh = nullptr;
        }

        assets[i].tex = new Image();
        if (!assets[i].tex->LoadTGA(assets[i].tex_path, true))
        {
            std::cout << "Texture not found: " << assets[i].tex_path << std::endl;
            delete assets[i].tex;
            assets[i].tex = nullptr;
        }
    }

    // choose fallback mesh and texture if loading fails
    Mesh*  fallbackMesh = assets[2].mesh ? assets[2].mesh : (assets[0].mesh ? assets[0].mesh : assets[1].mesh);
    Image* fallbackTex  = assets[2].tex  ? assets[2].tex  : (assets[0].tex  ? assets[0].tex  : assets[1].tex);

    int numberEntities = 3;
    entities.reserve(numberEntities);

    // create entities and assign transforms assets
    for (int i = 0; i < numberEntities; ++i)
    {
        Entity* e = new Entity();

        // translate models along x axis
        Matrix44 T;
        T.MakeTranslationMatrix(i * 1.4f - 1.4f, 0.0f, 0.0f);

        // rotate model from z up to y up
        Matrix44 Rx; Rx.MakeRotationMatrix(+PI / 2.0f, Vector3(1, 0, 0));

        // rotate model to face camera
        Matrix44 Ry; Ry.MakeRotationMatrix(PI, Vector3(0, 1, 0));

        // final model transform
        Matrix44 M = T * Ry * Rx;

        // animation phase offset per entity
        e->phase = i * 1.0f;

        // choose mesh and texture or fallback
        Mesh*  useMesh = assets[i].mesh ? assets[i].mesh : fallbackMesh;
        Image* useTex  = assets[i].tex  ? assets[i].tex  : fallbackTex;

        e->EntityAdd(useMesh, M);
        e->texture = useTex;

        entities.push_back(e);
    }

    // default render mode single entity
    drawMode = DRAW_SINGLE;

    // default camera property selection
    currentProp = PROP_FOV;
}

void Application::Render(void)
{
    // clear framebuffer every frame
    framebuffer.Fill(Color::BLACK);

    // clear depth buffer large value means far
    zbuffer.Fill(1e9f);

    if (!cam || entities.empty())
    {
        framebuffer.Render();
        return;
    }

    // render either single or multiple entities
    if (drawMode == DRAW_SINGLE)
    {
        entities[0]->Render(&framebuffer, cam, &zbuffer);
    }
    else
    {
        for (int i = 0; i < (int)entities.size(); ++i)
            entities[i]->Render(&framebuffer, cam, &zbuffer);
    }

    // present framebuffer to window
    framebuffer.Render();
}

void Application::Update(float seconds_elapsed)
{
    // animate entities only in multi mode
    if (drawMode == DRAW_MULTI)
    {
        for (auto* e : entities)
            e->Update(seconds_elapsed / 2);
    }
}

void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    if (!cam) return;

    switch (event.keysym.sym)
    {
        case SDLK_ESCAPE: exit(0); break;

        // switch to single entity mode
        case SDLK_1:
            drawMode = DRAW_SINGLE;
            std::cout << "Mode: SINGLE\n";
            break;

        // switch to multiple entity mode
        case SDLK_2:
            drawMode = DRAW_MULTI;
            std::cout << "Mode: MULTI\n";
            break;

        // select near plane modification
        case SDLK_n:
            currentProp = PROP_NEAR;
            std::cout << "Prop: NEAR\n";
            break;

        // select far plane modification
        case SDLK_f:
            currentProp = PROP_FAR;
            std::cout << "Prop: FAR\n";
            break;

        // select fov modification
        case SDLK_v:
            currentProp = PROP_FOV;
            std::cout << "Prop: FOV\n";
            break;

        // increase selected camera parameter
        case SDLK_PLUS:
        case SDLK_EQUALS:
        {
            if (currentProp == PROP_NEAR)
                cam->near_plane = std::min(cam->near_plane + 0.05f, cam->far_plane - 0.05f);
            else if (currentProp == PROP_FAR)
                cam->far_plane = cam->far_plane + 0.5f;
            else
                cam->fov = std::min(cam->fov + 2.0f, 120.0f);

            float aspect = (float)window_width / (float)window_height;
            cam->SetPerspective(cam->fov, aspect, cam->near_plane, cam->far_plane);
            break;
        }

        // decrease selected camera parameter
        case SDLK_MINUS:
        {
            if (currentProp == PROP_NEAR)
                cam->near_plane = std::max(cam->near_plane - 0.05f, 0.01f);
            else if (currentProp == PROP_FAR)
                cam->far_plane = std::max(cam->far_plane - 0.5f, cam->near_plane + 0.05f);
            else
                cam->fov = std::max(cam->fov - 2.0f, 10.0f);

            float aspect = (float)window_width / (float)window_height;
            cam->SetPerspective(cam->fov, aspect, cam->near_plane, cam->far_plane);
            break;
        }

        // toggle texture usage
        case SDLK_t:
        {
            for (auto* e : entities)
                e->useTexture = !e->useTexture;
            std::cout << "Toggle: useTexture\n";
            break;
        }

        // toggle zbuffer usage
        case SDLK_z:
        {
            for (auto* e : entities)
                e->useZBuffer = !e->useZBuffer;
            std::cout << "Toggle: useZBuffer\n";
            break;
        }

        // toggle uv interpolation
        case SDLK_c:
        {
            for (auto* e : entities)
                e->interpolateUVs = !e->interpolateUVs;
            std::cout << "Toggle: interpolateUVs\n";
            break;
        }

        default:
            break;
    }
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
    if (!cam) return;

    // store mouse position at press time
    mouse_position = Vector2((float)event.x, (float)event.y);
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
    // button state handled inside motion events
    (void)event;
}

void Application::OnMouseMove(SDL_MouseMotionEvent event)
{
    if (!cam) return;

    // relative motion values from input device
    float dx = (float)event.xrel;
    float dy = (float)event.yrel;

    // normalized values kept for future use
    float sx = (window_width  > 0) ? (dx / (float)window_width)  : 0.f;
    float sy = (window_height > 0) ? (dy / (float)window_height) : 0.f;
    (void)sx;
    (void)sy;

    // middle drag rotates models
    if (event.state & SDL_BUTTON_MMASK)
    {
        modelYaw   += dx * modelRotateSpeed;
        modelPitch += dy * modelRotateSpeed;

        for (auto* e : entities)
            e->SetUserRotation(modelYaw, modelPitch);
    }

    // left drag orbits camera
    if (event.state & SDL_BUTTON_LMASK)
    {
        orbitYaw   -= dx * orbitSpeed;
        orbitPitch += dy * orbitSpeed;

        UpdateCameraFromOrbit();
    }

    // right drag pans camera target
    if (event.state & SDL_BUTTON_RMASK)
    {
        Vector3 forward = (cam->center - cam->eye).Normalize();
        Vector3 right   = forward.Cross(cam->up).Normalize();
        Vector3 up      = right.Cross(forward).Normalize();

        // compute world movement per pixel based on distance and fov
        float dist = (cam->eye - cam->center).Length();
        float fovRad = cam->fov * (PI / 180.0f);
        float worldHeight = 2.0f * dist * tanf(fovRad * 0.5f);
        float worldPerPixel = worldHeight / (float)std::max(window_height, 1);

        // move center in camera plane
        Vector3 delta = (right * (-dx) + up * (dy)) * worldPerPixel;
        cam->center = cam->center + delta;

        UpdateCameraFromOrbit();
    }
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
    if (!cam) return;

    // scroll delta clamped to avoid extreme zoom jumps
    float dy = (float)event.preciseY;
    dy = Clamp(dy, -3.0f, 3.0f);

    // exponential zoom factor applied to orbit distance
    float zoomFactor = expf(-dy * zoomSpeed);
    orbitDist *= zoomFactor;

    orbitDist = Clamp(orbitDist, 0.6f, 25.0f);
    UpdateCameraFromOrbit();
}

void Application::OnFileChanged(const char* filename)
{
    // reload shader when file changes
    Shader::ReloadSingleShader(filename);
}

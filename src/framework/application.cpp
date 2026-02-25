#include "application.h"
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

    // initialise input and timing values
    mouse_state = 0;
    time = 0.f;
    window_width = w;
    window_height = h;
    keystate = SDL_GetKeyboardState(nullptr);

    // store initial mouse position
    mouse_position = Vector2(0.f, 0.f);
}

Application::~Application()
{
    // delete all entities created during init
    for (auto* e : entities) delete e;
    entities.clear();

    // delete quad mesh (Shader::Get is cached; NOT delete shader)
    delete quad;
    quad = nullptr;

    delete cam;
    cam = nullptr;
}

void Application::UpdateCameraFromOrbit()
{
    if (!cam) return;

    // clamp pitch and distance to avoid flips and invalid zoom
    orbitPitch = Clamp(orbitPitch, -1.45f, 1.45f);
    orbitDist  = Clamp(orbitDist, 0.6f, 25.0f);

    float cp = cosf(orbitPitch);

    Vector3 offset;
    offset.x = orbitDist * cp * cosf(orbitYaw);
    offset.y = orbitDist * sinf(orbitPitch);
    offset.z = orbitDist * cp * sinf(orbitYaw);

    cam->up = Vector3(0, 1, 0);
    cam->eye = cam->center + offset;

    cam->LookAt(cam->eye, cam->center, Vector3(0, 1, 0));
    cam->UpdateViewMatrix();
}

void Application::Init(void)
{
    std::cout << "Initiating app..." << std::endl;

    // default clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // camera setup
    cam = new Camera();
    float aspect = (float)window_width / (float)window_height;

    cam->LookAt(
        Vector3(0.0f, 0.35f, 2.2f),   // eye position
        Vector3(0.0f, 0.35f, 0.0f),   // target center
        Vector3(0.0f, 1.0f, 0.0f)     // up direction
    );
    cam->UpdateViewMatrix();
    cam->SetPerspective(60.0f, aspect, 0.1f, 100.0f);

    // compute orbit parameters from current camera pose
    Vector3 d = cam->eye - cam->center;
    orbitDist = d.Length();
    orbitYaw = atan2f(d.z, d.x);
    orbitPitch = asinf(d.y / std::max(orbitDist, 0.0001f));

    // Lab 4
    // shader (quad)
    shader = Shader::Get("shaders/quad.vs", "shaders/quad.fs");
    if (!shader)
        std::cout << "Shader could not be loaded: res/shaders/quad.vs + quad.fs\n";

    // fullscreen quad mesh
    quad = new Mesh();
    quad->CreateQuad();
    
    // load fruits texture for quad tasks
    quadTex = Texture::Get("images/fruits.png");
    if (!quadTex)
        std::cout << "could not load images/fruits.png\n";
    

    // Lab 4 - render a mesh using gpu
    // create a single entity
    Entity* anna = new Entity();
    anna->mesh = new Mesh();
    anna->mesh->LoadOBJ("meshes/anna.obj");
    if (!anna->mesh) std::cout << "could not load source object\n";
    
    annaTex = Texture::Get("textures/anna_color_specular.tga");
    if (!annaTex)
        std::cout << "could not load source texture\n";
    anna->texture = annaTex;
    
    // load material shader
    sourceMat = Shader::Get("shaders/raster.vs", "shaders/raster.fs");
    if (!sourceMat)
        std::cout << "could not load source shader material\n";
    anna->material = sourceMat;

    // make some transformations to ajust anna's position and scale
    Matrix44 T; Matrix44 Rx; Matrix44 Ry; Matrix44 S;
    T.MakeTranslationMatrix(0, 0, 0);
    Rx.MakeRotationMatrix(-PI / 2.0f, Vector3(1, 0, 0));
    Ry.MakeRotationMatrix(PI / 2.0f, Vector3(1, 0.5, -0.5));
    S.MakeScaleMatrix(2.5f, 2.5f, 2.5f); 

    // apply those transformation to the model matrix 
    anna->modelMatrix = T * Ry * Rx* S;

    // sum anna's entity to the array entities to have acces to it our of the init()
    entities.push_back(anna);
}

void Application::Render(void)
{
    if (lab5Scene)
        glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
    else
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (lab5Scene)
        return;

    float aspect = (float)window_width / (float)window_height;

    // task 4, 2.5 gpu mesh rendering
    if (currentTask == 4)
    {
        glEnable(GL_DEPTH_TEST);

        if (!entities.empty())
            entities[0]->Render(cam);

        glDisable(GL_DEPTH_TEST);
        return;
    }

    // tasks 1..3, quad shader path
    glDisable(GL_DEPTH_TEST);

    if (shader && quad)
    {
        shader->Enable();

        shader->SetFloat("u_subtask", (float)currentSubTask);
        shader->SetFloat("u_task", (float)currentTask);
        shader->SetFloat("u_aspect", aspect);
        shader->SetFloat("u_time", time);

        // tasks 2.2 2.3 2.4 use fruits png
        if (quadTex)
        {
            shader->SetTexture("u_texture", quadTex);
            shader->SetUniform2("u_texel_size",
                1.0f / quadTex->width,
                1.0f / quadTex->height
            );
        }

        quad->Render();
        shader->Disable();
    }
}

void Application::Update(float seconds_elapsed)
{
    time += seconds_elapsed;
}

void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    switch (event.keysym.sym)
    {
        case SDLK_ESCAPE: exit(0); break;

        // 1..4 map to 2.2 2.3 2.4 2.5
        case SDLK_1: currentTask = 1; currentSubTask = 1; break; // 2.2
        case SDLK_2: currentTask = 2; currentSubTask = 1; break; // 2.3
        case SDLK_3: currentTask = 3; currentSubTask = 1; break; // 2.4
        case SDLK_4: currentTask = 4; currentSubTask = 1; break; // 2.5

        // a..f: select subtask (1..6)
        case SDLK_a: currentSubTask = 1; break;
        case SDLK_b: currentSubTask = 2; break;
        case SDLK_c: currentSubTask = 3; break;
        case SDLK_d: currentSubTask = 4; break;
        case SDLK_e: currentSubTask = 5; break;
        case SDLK_f: currentSubTask = 6; break;

        case SDLK_l:
            lab5Scene = !lab5Scene;
            break;

        default:
            break;
    }
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
    if (!cam) return;
    mouse_position = Vector2((float)event.x, (float)event.y);
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
    (void)event;
}

void Application::OnMouseMove(SDL_MouseMotionEvent event)
{
    if (!cam) return;

    float dx = (float)event.xrel;
    float dy = (float)event.yrel;

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

        float dist = (cam->eye - cam->center).Length();
        float fovRad = cam->fov * (PI / 180.0f);
        float worldHeight = 2.0f * dist * tanf(fovRad * 0.5f);
        float worldPerPixel = worldHeight / (float)std::max(window_height, 1);

        Vector3 delta = (right * (-dx) + up * (dy)) * worldPerPixel;
        cam->center = cam->center + delta;

        UpdateCameraFromOrbit();
    }
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
    if (!cam) return;

    float dy = (float)event.preciseY;
    dy = Clamp(dy, -3.0f, 3.0f);

    float zoomFactor = expf(-dy * zoomSpeed);
    orbitDist *= zoomFactor;

    orbitDist = Clamp(orbitDist, 0.6f, 25.0f);
    UpdateCameraFromOrbit();
}

void Application::OnFileChanged(const char* filename)
{
    Shader::ReloadSingleShader(filename);
}

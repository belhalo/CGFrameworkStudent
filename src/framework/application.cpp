#include "application.h"
#include "utils.h"

#include <algorithm>
#include <cmath>
#include <iostream>

static inline float Clamp(float v, float a, float b)
{
    return std::max(a, std::min(v, b));
}

Application::Application(const char* caption, int width, int height)
{
    window = createWindow(caption, width, height);

    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    mouse_state = 0;
    time = 0.0f;
    window_width = w;
    window_height = h;
    keystate = SDL_GetKeyboardState(nullptr);

    mouse_position = Vector2(0.0f, 0.0f);
}

Application::~Application()
{
    for (auto* e : entities)
        delete e;
    entities.clear();

    delete quad;
    quad = nullptr;

    delete rasterMaterial;
    rasterMaterial = nullptr;

    delete gouraudMaterial;
    gouraudMaterial = nullptr;

    delete phongMaterial;
    phongMaterial = nullptr;

    delete cam;
    cam = nullptr;
}

void Application::UpdateCameraFromOrbit()
{
    if (!cam)
        return;

    orbitPitch = Clamp(orbitPitch, -1.45f, 1.45f);
    orbitDist = Clamp(orbitDist, 0.6f, 25.0f);

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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    cam = new Camera();
    float aspect = (float)window_width / (float)window_height;

    cam->LookAt(
        Vector3(0.0f, 0.35f, 2.2f),
        Vector3(0.0f, 0.35f, 0.0f),
        Vector3(0.0f, 1.0f, 0.0f)
    );
    cam->UpdateViewMatrix();
    cam->SetPerspective(60.0f, aspect, 0.1f, 100.0f);

    Vector3 d = cam->eye - cam->center;
    orbitDist = d.Length();
    orbitYaw = atan2f(d.z, d.x);
    orbitPitch = asinf(d.y / std::max(orbitDist, 0.0001f));

    // lab 4 quad shader
    shader = Shader::Get("shaders/quad.vs", "shaders/quad.fs");
    if (!shader)
        std::cout << "could not load quad shader\n";

    quad = new Mesh();
    quad->CreateQuad();

    quadTex = Texture::Get("images/fruits.png");
    if (!quadTex)
        std::cout << "could not load images/fruits.png\n";

    // shared textures for mesh
    annaColorTex = Texture::Get("textures/anna_color_specular.tga");
    if (!annaColorTex)
        std::cout << "could not load anna color texture\n";

    annaNormalTex = Texture::Get("textures/anna_normal.tga");
    if (!annaNormalTex)
        std::cout << "could not load anna normal texture\n";

    // lab 4 raster material
    rasterMaterial = new Material();
    rasterMaterial->shader = Shader::Get("shaders/raster.vs", "shaders/raster.fs");
    rasterMaterial->colorTexture = annaColorTex;

    if (!rasterMaterial->shader)
        std::cout << "could not load raster shader\n";

    // lab 5 gouraud material
    gouraudMaterial = new Material();
    gouraudMaterial->shader = Shader::Get("shaders/gouraud.vs", "shaders/gouraud.fs");
    gouraudMaterial->Ka = Vector3(0.2f, 0.2f, 0.2f);
    gouraudMaterial->Kd = Vector3(0.8f, 0.7f, 0.5f);
    gouraudMaterial->Ks = Vector3(1.0f, 1.0f, 1.0f);
    gouraudMaterial->shininess = 32.0f;

    if (!gouraudMaterial->shader)
        std::cout << "could not load gouraud shader\n";

    // lab 5 phong material
    phongMaterial = new Material();
    phongMaterial->shader = Shader::Get("shaders/phong.vs", "shaders/phong.fs");
    phongMaterial->Ka = Vector3(0.2f, 0.2f, 0.2f);
    phongMaterial->Kd = Vector3(0.8f, 0.7f, 0.5f);
    phongMaterial->Ks = Vector3(1.0f, 1.0f, 1.0f);
    phongMaterial->shininess = 32.0f;

    if (!phongMaterial->shader)
        std::cout << "could not load phong shader\n";

    // mesh entity
    Entity* anna = new Entity();
    anna->mesh = new Mesh();
    anna->mesh->LoadOBJ("meshes/anna.obj");
    anna->material = rasterMaterial;

    Matrix44 T;
    Matrix44 Rx;
    Matrix44 Ry;
    Matrix44 S;
    T.MakeTranslationMatrix(0.0f, 0.0f, 0.0f);
    Rx.MakeRotationMatrix(-PI / 2.0f, Vector3(1, 0, 0));
    Ry.MakeRotationMatrix(PI / 2.0f, Vector3(1, 0.5f, -0.5f));
    S.MakeScaleMatrix(2.5f, 2.5f, 2.5f);

    anna->modelMatrix = T * Ry * Rx * S;
    anna->baseMatrix = anna->modelMatrix;

    entities.push_back(anna);

    uniformData.ambientLightIntensity = ambientLightIntensity;
    uniformData.sceneLight = mainLight;
    uniformData.viewProjectionMatrix = cam->viewprojection_matrix;
    uniformData.cameraEye = cam->eye;
}

void Application::Render(void)
{
    if (lab5Scene)
        glClearColor(0.85f, 0.85f, 0.85f, 1.0f);
    else
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = (float)window_width / (float)window_height;

    if (lab5Scene)
    {
        glEnable(GL_DEPTH_TEST);

        uniformData.viewProjectionMatrix = cam->viewprojection_matrix;
        uniformData.cameraEye = cam->eye;
        uniformData.ambientLightIntensity = ambientLightIntensity;
        uniformData.sceneLight = mainLight;

        if (!entities.empty())
        {
            if (shadingMode == SHADING_GOURAUD)
                entities[0]->material = gouraudMaterial;
            else
                entities[0]->material = phongMaterial;

            entities[0]->Render(uniformData);
        }

        glDisable(GL_DEPTH_TEST);
        return;
    }

    if (currentTask == 4)
    {
        glEnable(GL_DEPTH_TEST);

        uniformData.viewProjectionMatrix = cam->viewprojection_matrix;
        uniformData.cameraEye = cam->eye;
        uniformData.ambientLightIntensity = ambientLightIntensity;
        uniformData.sceneLight = mainLight;

        if (!entities.empty())
        {
            entities[0]->material = rasterMaterial;
            entities[0]->Render(uniformData);
        }

        glDisable(GL_DEPTH_TEST);
        return;
    }

    glDisable(GL_DEPTH_TEST);

    if (shader && quad)
    {
        shader->Enable();

        shader->SetFloat("u_subtask", (float)currentSubTask);
        shader->SetFloat("u_task", (float)currentTask);
        shader->SetFloat("u_aspect", aspect);
        shader->SetFloat("u_time", time);

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
        case SDLK_ESCAPE:
            exit(0);
            break;

        case SDLK_l:
            lab5Scene = !lab5Scene;
            break;

        default:
            break;
    }

    if (lab5Scene)
    {
        switch (event.keysym.sym)
        {
            case SDLK_g:
                shadingMode = SHADING_GOURAUD;
                break;

            case SDLK_p:
                shadingMode = SHADING_PHONG;
                break;

            case SDLK_c:
                useColorTexture = !useColorTexture;
                break;

            case SDLK_s:
                useSpecularTexture = !useSpecularTexture;
                break;

            case SDLK_n:
                useNormalTexture = !useNormalTexture;
                break;

            case SDLK_1:
                numLights = 1;
                break;

            case SDLK_2:
                numLights = 2;
                break;

            default:
                break;
        }
        return;
    }

    switch (event.keysym.sym)
    {
        case SDLK_1:
            currentTask = 1;
            currentSubTask = 1;
            break;

        case SDLK_2:
            currentTask = 2;
            currentSubTask = 1;
            break;

        case SDLK_3:
            currentTask = 3;
            currentSubTask = 1;
            break;

        case SDLK_4:
            currentTask = 4;
            currentSubTask = 1;
            break;

        case SDLK_a:
            currentSubTask = 1;
            break;

        case SDLK_b:
            currentSubTask = 2;
            break;

        case SDLK_c:
            currentSubTask = 3;
            break;

        case SDLK_d:
            currentSubTask = 4;
            break;

        case SDLK_e:
            currentSubTask = 5;
            break;

        case SDLK_f:
            currentSubTask = 6;
            break;

        default:
            break;
    }
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
    if (!cam)
        return;

    mouse_position = Vector2((float)event.x, (float)event.y);
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
    (void)event;
}

void Application::OnMouseMove(SDL_MouseMotionEvent event)
{
    if (!cam)
        return;

    float dx = (float)event.xrel;
    float dy = (float)event.yrel;

    if (event.state & SDL_BUTTON_MMASK)
    {
        modelYaw += dx * modelRotateSpeed;
        modelPitch += dy * modelRotateSpeed;

        for (auto* e : entities)
            e->SetUserRotation(modelYaw, modelPitch);
    }

    if (event.state & SDL_BUTTON_LMASK)
    {
        orbitYaw -= dx * orbitSpeed;
        orbitPitch += dy * orbitSpeed;
        UpdateCameraFromOrbit();
    }

    if (event.state & SDL_BUTTON_RMASK)
    {
        Vector3 forward = (cam->center - cam->eye).Normalize();
        Vector3 right = forward.Cross(cam->up).Normalize();
        Vector3 up = right.Cross(forward).Normalize();

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
    if (!cam)
        return;

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

#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 
#include "entity.h"

Application::Application(const char* caption, int width, int height)
{
    window = createWindow(caption, width, height);
    
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    
    this->mouse_state = 0;
    this->time = 0.f;
    this->window_width = w;
    this->window_height = h;
    this->keystate = SDL_GetKeyboardState(nullptr);
    
    this->framebuffer.Resize(w, h);
    
    // lab3
    // allocate zbuffer initially (must match framebuffer size)
    zbuffer.Resize(w, h);
    
    mouse_position = Vector2(0.f, 0.f);
}

Application::~Application()
{
    for (auto* e : entities) delete e;
    entities.clear();
    delete cam;
    cam = nullptr;
}

// small clamp helper
static inline float Clamp(float v, float a, float b)
{
    return std::max(a, std::min(v, b));
}

// call after changing orbitYaw/orbitPitch/orbitDist or after moving center
void Application::UpdateCameraFromOrbit()
{
    if (!cam) return;

    orbitPitch = Clamp(orbitPitch, -1.5f, 1.5f);
    orbitDist = std::max(0.1f, orbitDist);

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
    // clear framebuffer once
    framebuffer.Fill(Color::BLACK);

    // init the camera
    cam = new Camera(); // create a new camera, but as we are in the init the only create it once
    float aspect = (float) window_width / (float) window_height;


    // set camera pose (ONLY ONCE)
    cam->LookAt(
        Vector3(2.0f, 0.5f, 0.5f),
        Vector3(-0.2f, -0.8f, 0.8f),
        Vector3(0.0f, 1.0f, 0.0f)
    );
    
    // set projection
    cam->SetPerspective(60.0f, aspect, 0.1f, 100.0f);

    // initialise orbit from current eye/center (AFTER LookAt)
    Vector3 d = cam->eye - cam->center;
    orbitDist = d.Length();
    orbitYaw   = atan2f(d.z, d.x);
    orbitPitch = asinf(d.y / std::max(orbitDist, 0.0001f));
    
    // download the mesh from the resources
    Mesh* m = new Mesh();
    if (!(m->LoadOBJ("meshes/lee.obj"))) {
        std::cout << "Object not found!" << std::endl;
    }
    
    // lab3
    // load texture ONCE (shared by all entities)
    Image* tex = new Image();
    if (!tex->LoadTGA("textures/lee_color_specular.tga", true)) // flip_y=true
    {
        std::cout << "Texture not found!" << std::endl;
        delete tex;
        tex = nullptr;
    }

    // setting the meshes in the entity class
    int numberEntities = 3;         // >= 3 for mode 2
    entities.reserve(numberEntities);
    for (int i = 0; i < numberEntities; ++i)
    {
        Entity* e = new Entity();

        Matrix44 T;
        T.MakeTranslationMatrix(i * 2.0f - 2.0f, 0.0f, 0.0f);

        Matrix44 R;
        R.MakeRotationMatrix(-PI, Vector3(0,0,1));
        //R.MakeRotationMatrix(-PI/2, Vector3(1, 0, 0));
        //R.MakeRotationMatrix(PI * 0.5f, Vector3(0,1,0));

        Matrix44 M = T * R;
        
        R.MakeRotationMatrix(-PI / 2, Vector3(1, 0, 0));

        M = M * R;

        e->EntityAdd(m, M);

        // lab3: give each entity the texture pointer (shared) 
        e->texture = tex;

        entities.push_back(e);
    }

    /*
    // setting the meshes in the entity class
    int numberEntities = 3; //modify it depending the number of entities we want 
    for (int i = 0; i < numberEntities; i++) {
        Entity* e = new Entity;
        Matrix44 matrix;
        matrix.MakeTranslationMatrix(i * 2.0 - 2.0, 0, 0);

        e->EntityAdd(m, matrix);
        this->entities.emplace_back(e);

    }


    // create the entity and assign the loaded mesh
    for (int i = 0; i < entities.size(); i++) {
        // for default the color will be white
        Color choosenColor = Color::WHITE;
        // then, depending of the iteration we will painting in a color on in another -> to have variation
        if (i == 1) choosenColor = Color::PURPLE;
        else if (i == 2) choosenColor = Color::RED;
        entities[i]->Render()
        entities[i]->Render(&framebuffer, cam, choosenColor);
    }

    framebuffer.Render();*/
    
    //drawMode = DRAW_MULTI;
    drawMode = DRAW_SINGLE;
    currentProp = PROP_FOV;
}

// Render one frame
void Application::Render(void)
{
    framebuffer.Fill(Color::BLACK);
    
    // Lab3
    // clear zbuffer every frame
    // smaller z is closer, so fill with a huge value
    zbuffer.Fill(1e9f);

    if (!cam || entities.empty())
    {
        framebuffer.Render();
        return;
    }

    // Lab2: "1" shows only one entity (static)
    if (drawMode == DRAW_SINGLE)
    {
        // Lab3: Render(framebuffer, camera, zbuffer)
        entities[0]->Render(&framebuffer, cam, &zbuffer);
    }
    else // Lab2: "2" shows multiple entities (animated)
    {
        for (int i = 0; i < (int)entities.size(); ++i)
        {
            entities[i]->Render(&framebuffer, cam, &zbuffer);
        }
    }

    framebuffer.Render();
}

// Called once per frame (dt = seconds elapsed since last frame)
void Application::Update(float seconds_elapsed)
{
    // Only animate in mode '2' (multiple animated entities)
    if (drawMode == DRAW_MULTI)
    {
        for (auto* e : entities)
            e->Update(seconds_elapsed);
    }
}

// Keyboard press event
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    if (!cam) return;

    switch (event.keysym.sym)
    {
        case SDLK_ESCAPE: exit(0); break;

        // "1" -> Draw SINGLE entity
        case SDLK_1:
            drawMode = DRAW_SINGLE;
            std::cout << "Mode: SINGLE\n";
            break;

        // "2" -> Draw MULTIPLE animated entities
        case SDLK_2:
            drawMode = DRAW_MULTI;
            std::cout << "Mode: MULTI\n";
            break;

        // "N" -> select NEAR
        case SDLK_n:
            currentProp = PROP_NEAR;
            std::cout << "Prop: NEAR\n";
            break;

        // "F" -> select FAR
        case SDLK_f:
            currentProp = PROP_FAR;
            std::cout << "Prop: FAR\n";
            break;

        // "V" -> select FOV
        case SDLK_v:
            currentProp = PROP_FOV;
            std::cout << "Prop: FOV\n";
            break;

        // "+" -> increase selected property
        case SDLK_PLUS:
        case SDLK_EQUALS: // mac '+' is Shift+'='
        {
            if (currentProp == PROP_NEAR)
            {
                cam->near_plane = std::min(cam->near_plane + 0.05f, cam->far_plane - 0.05f);
            }
            else if (currentProp == PROP_FAR)
            {
                cam->far_plane = cam->far_plane + 0.5f;
            }
            else // PROP_FOV
            {
                cam->fov = std::min(cam->fov + 2.0f, 120.0f);
            }

            float aspect = (float)window_width / (float)window_height;
            cam->SetPerspective(cam->fov, aspect, cam->near_plane, cam->far_plane);
            break;
        }

        // "-" -> decrease selected property
        case SDLK_MINUS:
        {
            if (currentProp == PROP_NEAR)
            {
                cam->near_plane = std::max(cam->near_plane - 0.05f, 0.01f);
            }
            else if (currentProp == PROP_FAR)
            {
                cam->far_plane = std::max(cam->far_plane - 0.5f, cam->near_plane + 0.05f);
            }
            else // PROP_FOV
            {
                cam->fov = std::max(cam->fov - 2.0f, 10.0f);
            }

            float aspect = (float)window_width / (float)window_height;
            cam->SetPerspective(cam->fov, aspect, cam->near_plane, cam->far_plane);
            break;
        }
            
        // lab3 unteractivity
        case SDLK_t:
        {
            for (auto* e : entities)
                e->useTexture = !e->useTexture;

            std::cout << "Toggle: useTexture\n";
            break;
        }

        case SDLK_z:
        {
            for (auto* e : entities)
                e->useZBuffer = !e->useZBuffer;

            std::cout << "Toggle: useZBuffer\n";
            break;
        }

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

    if (event.button == SDL_BUTTON_LEFT)
    {
        // start orbiting
        orbiting = true;

        // set current mouse reference point
        mouse_position = Vector2((float)event.x, (float)event.y);
    }
    else if (event.button == SDL_BUTTON_RIGHT)
    {
        // Right-click: set camera target (center)
        float nx = (2.0f * event.x) / (float)window_width - 1.0f;
        float ny = 1.0f - (2.0f * event.y) / (float)window_height;

        cam->center = Vector3(nx, ny, cam->center.z);
        cam->UpdateViewMatrix();

        // recompute orbit params based on new center
        Vector3 d = cam->eye - cam->center;
        orbitDist  = d.Length();
        orbitYaw   = atan2f(d.z, d.x);
        orbitPitch = asinf(d.y / std::max(orbitDist, 0.0001f));
    }
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_LEFT)
        orbiting = false;
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
    if (!cam) return;
    if (!orbiting) return;

    // delta in pixels (SDL origin top-left)
    float dx = (float)event.x - mouse_position.x;
    float dy = (float)event.y - mouse_position.y;

    mouse_position = Vector2((float)event.x, (float)event.y);

    // Orbit only while left button is held
    orbitYaw -= dx * (orbitSpeed/8); // we divided by 8 to make it a bit slower!
    orbitPitch += dy * (orbitSpeed/8);

    UpdateCameraFromOrbit();

    cam->UpdateViewMatrix();
    cam->viewprojection_matrix = cam->view_matrix * cam->projection_matrix;
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
    if (!cam) return;

    // zoom changes orbit distance
    orbitDist -= event.preciseY * zoomSpeed;
    orbitDist = Clamp(orbitDist, 0.5f, 50.0f);

    UpdateCameraFromOrbit();
}

void Application::OnFileChanged(const char* filename)
{
    Shader::ReloadSingleShader(filename);
}

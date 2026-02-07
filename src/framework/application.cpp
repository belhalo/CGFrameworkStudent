#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 
#include "entity.h"

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w,h;
    SDL_GetWindowSize(window, &w, &h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);

	this->framebuffer.Resize(w, h);
}

Application::~Application()
{

}

static inline float Clamp(float v, float a, float b) { return std::max(a, std::min(v, b)); }

// call after changing orbitYaw/orbitPitch/orbitDist or after moving center
void Application::UpdateCameraFromOrbit()
{
    // clamp pitch to avoid flipping
    orbitPitch = Clamp(orbitPitch, -1.5f, 1.5f);
    orbitDist  = std::max(0.1f, orbitDist);

    // spherical coords around center
    float cp = cosf(orbitPitch);
    Vector3 offset;
    offset.x = orbitDist * cp * cosf(orbitYaw);
    offset.y = orbitDist * sinf(orbitPitch);
    offset.z = orbitDist * cp * sinf(orbitYaw);

    cam->up = Vector3(0,1,0);
    cam->eye = cam->center + offset;
    cam->UpdateViewMatrix();
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;

	// init the window colours
	framebuffer.Fill(Color::BLACK);
	framebuffer.DrawRect(0, 0, 1280, 50, Color::GRAY, 0, true, Color::GRAY);

    // init the camera
    this->cam = new Camera(); // create a new camera, but as we are in the init the only create it once
    float aspect = (float) window_width / (float) window_height;


    // set camera pose (ONLY ONCE)
    cam->LookAt(
        Vector3(1.0f, 0.3f, 0.5f),
        Vector3(0.1f, 0.2f, 0.8f),
        Vector3(0.0f, 1.0f, 0.0f)
    );

    // set projection
    cam->SetPerspective(60.0f, aspect, 0.1f, 100.0f);

    // initialise orbit from current eye/center (AFTER LookAt)
    Vector3 d = cam->eye - cam->center;
    orbitDist = d.Length();
    orbitYaw   = atan2f(d.z, d.x);
    orbitPitch = asinf(d.y / std::max(orbitDist, 0.0001f));

    // yaw from x/z, pitch from y
    orbitYaw   = atan2f(d.z, d.x);
    orbitPitch = asinf(d.y / std::max(orbitDist, 0.0001f));
    
    // download the mesh from the resourses
    Mesh* m = new Mesh();
    if (!(m->LoadOBJ("meshes/lee.obj"))) {
        std::cout << "Object not found!" << std::endl;
    }

    // setting the meshes in the entity class
    int numberEntities = 3; //modify it depending the number of entities we want 
    for (int i = 0; i < numberEntities ; i++) {
        Entity* e = new Entity;
        Matrix44 matrix;
        matrix.MakeTranslationMatrix(i * 2.0 - 2.0, 0, 0);
  
        e->EntityAdd(m, matrix);
        this->entities.emplace_back(e);

    }

	// init the toolbar
	Image clear;
	if (clear.LoadPNG("images/clear.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(clear, 15, 10);

	Image load;
	if (load.LoadPNG("images/load.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(load, 55, 10);

	Image save;
	if (save.LoadPNG("images/save.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(save, 95, 10);

	Image eraser;
	if (eraser.LoadPNG("images/eraser.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(eraser, 135, 10);

	Image line;
	if (line.LoadPNG("images/line.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(line, 175, 10);

	Image rectangle;
	if (rectangle.LoadPNG("images/rectangle.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(rectangle, 215, 10);

	Image circle;
	if (circle.LoadPNG("images/circle.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(circle, 255, 10);

	Image triangle;
	if (triangle.LoadPNG("images/triangle.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(triangle, 295, 10);

	Image black;
	if (black.LoadPNG("images/black.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(black, 335, 10);

	Image white;
	if (white.LoadPNG("images/white.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(white, 375, 10);

	Image pink;
	if (pink.LoadPNG("images/pink.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(pink, 415, 10);

	Image yellow;
	if (yellow.LoadPNG("images/yellow.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(yellow, 455, 10);

	Image red;
	if (red.LoadPNG("images/red.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(red, 495, 10);

	Image blue;
	if (blue.LoadPNG("images/blue.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(blue, 535, 10);

	Image cyan;
	if (cyan.LoadPNG("images/cyan.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}
	framebuffer.DrawImage(cyan, 575, 10);
    
    buttons.clear();

        auto loadIcon = [&](const char* path) {
            Image img;
            if (!img.LoadPNG(path, true))
                std::cout << "Image not found: " << path << std::endl;
            return img;
        };

        // NOTE: use forward slashes (works on Mac + Windows)
        Image clearI = loadIcon("images/clear.png");
        Image loadI  = loadIcon("images/load.png");
        Image saveI  = loadIcon("images/save.png");
        Image eraserI= loadIcon("images/eraser.png");
        Image penI   = loadIcon("images/pencil.png");
        Image lineI  = loadIcon("images/line.png");
        Image rectI  = loadIcon("images/rectangle.png");
        Image triI   = loadIcon("images/triangle.png");
        Image cirI   = loadIcon("images/circle.png");
        Image blackI = loadIcon("images/black.png");
        Image whiteI = loadIcon("images/white.png");
        Image pinkI  = loadIcon("images/pink.png");
        Image yellowI= loadIcon("images/yellow.png");
        Image redI   = loadIcon("images/red.png");
        Image blueI  = loadIcon("images/blue.png");
        Image cyanI  = loadIcon("images/cyan.png");
        Image greenI = loadIcon("images/green.png");

        int y = 10;        // toolbar margin from bottom
        int x = 15;        // starting x
        int step = 40;     // spacing

        buttons.emplace_back(ButtonType::Clear,     Vector2(x, y), clearI);  x += step;
        buttons.emplace_back(ButtonType::Load,      Vector2(x, y), loadI);   x += step;
        buttons.emplace_back(ButtonType::Save,      Vector2(x, y), saveI);   x += step;
        buttons.emplace_back(ButtonType::Eraser,    Vector2(x, y), eraserI); x += step;
        buttons.emplace_back(ButtonType::Pencil,    Vector2(x, y), penI);    x += step;
        buttons.emplace_back(ButtonType::Line,      Vector2(x, y), lineI);   x += step;
        buttons.emplace_back(ButtonType::Rectangle, Vector2(x, y), rectI);   x += step;
        buttons.emplace_back(ButtonType::Triangle,  Vector2(x, y), triI);    x += step;
        buttons.emplace_back(ButtonType::Circle,    Vector2(x, y), cirI);    x += step;

        // colors
        buttons.emplace_back(ButtonType::ColorBlack, Vector2(x, y), blackI);  x += step;
        buttons.emplace_back(ButtonType::ColorWhite, Vector2(x, y), whiteI);  x += step;
        buttons.emplace_back(ButtonType::ColorPink,  Vector2(x, y), pinkI);   x += step;
        buttons.emplace_back(ButtonType::ColorYellow,Vector2(x, y), yellowI); x += step;
        buttons.emplace_back(ButtonType::ColorRed,   Vector2(x, y), redI);    x += step;
        buttons.emplace_back(ButtonType::ColorBlue,  Vector2(x, y), blueI);   x += step;
        buttons.emplace_back(ButtonType::ColorCyan,  Vector2(x, y), cyanI);   x += step;
        buttons.emplace_back(ButtonType::ColorGreen, Vector2(x, y), greenI);  x += step;

        // draw toolbar icons once 
        for (const auto& b : buttons) {
            framebuffer.DrawImage(b.icon, (int)b.pos.x, (int)b.pos.y);
        }

        particleSys.Init(); 
}

// Render one frame
void Application::Render(void)
{
    // create the entity and assign the loaded mesh
    for (int i = 0; i < entities.size(); i++) {
        // for default the color will be white
        Color choosenColor = Color::WHITE;

        // then, depending of the iteration we will painting in a color on in another -> to have variation
        if (i == 1) choosenColor = Color::PURPLE;
        else if (i == 2) choosenColor = Color::RED;

        entities[i]->Render(&framebuffer, cam, choosenColor);
    }
    
    framebuffer.Render();

    //Matrix44 matrix = Matrix44();
    //matrix.MakeScaleMatrix(1, -1, 1);
    //matrix.MakeRotationMatrix(PI/6 , Vector3(1, 3, 1));
    //matrix.MakeTranslationMatrix(0.5, -0.5, 0.5);

    //e.EntityAdd(m1, matrix);
    //e.Render(&framebuffer, c, Color::BLUE);
    //e.Render(&framebuffer, c, Color::WHITE);
    //e.Render(&framebuffer, c, Color::PURPLE);
}

// Called after render
void Application::Update(float seconds_elapsed)
{
    // lab2 animations
    if (mode == MODE_ANIMATION)
    {
        for (auto* e : entities)
            e->Update(seconds_elapsed);
    }
}



// helper to convert coords from mouse to canvas (since SDL and SetPixel use inverted from one another)
// mx,my come from SDL events (origin top left)
// framebuffer expects origin bottom left
Vector2 Application::MouseToCanvas(int mx, int my) const
{
    return Vector2((float)mx, (float)(window_height - 1 - my));
}

// helper for drag to rect
static void DragToRect(const Vector2& a, const Vector2& b, int& x, int& y, int& w, int& h)
{
    x = (int)std::floor(std::min(a.x, b.x));
    y = (int)std::floor(std::min(a.y, b.y));
    w = (int)std::ceil (std::abs(b.x - a.x));
    h = (int)std::ceil (std::abs(b.y - a.y));
    if (w < 1) w = 1;
    if (h < 1) h = 1;
}


//keyboard press event 
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    switch (event.keysym.sym)
    {
        case SDLK_ESCAPE: exit(0); break;

        // Lab2 scene modes
        case SDLK_1:
            mode = MODE_PAINT;
            std::cout << "Mode: Single entity\n";
            break;

        case SDLK_2:
            mode = MODE_ANIMATION;
            std::cout << "Mode: Multiple animated entities\n";
            break;

        // Lab2 camera property selection
        case SDLK_n:
            currentProp = PROP_NEAR;
            std::cout << "Current property: NEAR\n";
            break;

        case SDLK_f:
            currentProp = PROP_FAR;
            std::cout << "Current property: FAR\n";
            break;

        case SDLK_v:
            currentProp = PROP_FOV;
            std::cout << "Current property: FOV\n";
            break;

        // increase/decrease selected property
        case SDLK_PLUS:
        case SDLK_EQUALS: // '+' on many keyboards is shift+'=' (eg mac)
        {
            if (!cam) break;

            if (currentProp == PROP_NEAR)
                cam->near_plane = std::min(cam->near_plane * 1.1f, cam->far_plane - 0.01f);
            else if (currentProp == PROP_FAR)
                cam->far_plane = cam->far_plane * 1.1f;
            else // FOV
                cam->fov = std::min(cam->fov + 2.0f, 170.0f);

            cam->UpdateProjectionMatrix();
            break;
        }

        case SDLK_MINUS:
        {
            if (!cam) break;

            if (currentProp == PROP_NEAR)
                cam->near_plane = std::max(cam->near_plane / 1.1f, 0.001f);
            else if (currentProp == PROP_FAR)
                cam->far_plane = std::max(cam->far_plane / 1.1f, cam->near_plane + 0.01f);
            else // FOV
                cam->fov = std::max(cam->fov - 2.0f, 5.0f);

            cam->UpdateProjectionMatrix();
            break;
        }

        default:
            break;
    }
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
    // Lab2 camera controls should work regardless of paint mode (assignment wants camera always controllable)
    if (event.button == SDL_BUTTON_LEFT)
    {
        orbiting = true;
        return;
    }
    if (event.button == SDL_BUTTON_RIGHT)
    {
        panning = true;
        return;
    }
}


// button handler
void Application::HandleButton(ButtonType t)
{
    switch (t)
    {
        case ButtonType::Line:      currentTool = TOOL_LINE; break;
        case ButtonType::Rectangle: currentTool = TOOL_RECT; break;
        case ButtonType::Triangle:  currentTool = TOOL_TRIANGLE; triClicks = 0; break;
        case ButtonType::Eraser:    currentTool = TOOL_ERASER; break;
        case ButtonType::Circle:    currentTool = TOOL_CIRCLE; break;
        case ButtonType::Pencil:    currentTool = TOOL_PENCIL; triClicks = 0; isDragging = false; break;

        case ButtonType::ColorBlack: borderColor = Color::BLACK; fillColor = Color::BLACK; break;
        case ButtonType::ColorWhite: borderColor = Color::WHITE; fillColor = Color::WHITE; break;
        case ButtonType::ColorRed:   borderColor = Color::RED;   fillColor = Color::RED;   break;
        case ButtonType::ColorBlue:  borderColor = Color::BLUE;  fillColor = Color::BLUE;  break;
        case ButtonType::ColorCyan:  borderColor = Color::CYAN;  fillColor = Color::CYAN;  break;
        case ButtonType::ColorPink:  borderColor = Color::PURPLE;fillColor = Color::PURPLE;break;   // var is purple but image is named pink
        case ButtonType::ColorYellow:borderColor = Color::YELLOW; fillColor = Color::YELLOW; break;
        case ButtonType::ColorGreen: borderColor = Color::GREEN;  fillColor = Color::GREEN;  break;
        // although there is a GRAY var, there is no gray image so we skipped it
            
            
        case ButtonType::Clear:
            framebuffer.Fill(Color::BLACK);
            framebuffer.DrawRect(0, 0, 1280, 50, Color::GRAY, 0, true, Color::GRAY);
            for (const auto& b : buttons) b.Render(framebuffer);
            break;

        case ButtonType::Load:
        {
            Image img;
            if (!img.LoadPNG("images/load.png", true)) {
                std::cout << "Load failed: images/load.png\n";
                break;
            }

            // clear canvas
            framebuffer.Fill(Color::BLACK);

            // draw loaded image above toolbar.
            framebuffer.DrawImage(img, 0, 50);

            // redraw toolbar background + buttons
            framebuffer.DrawRect(0, 0, window_width, 50, Color::GRAY, 0, true, Color::GRAY);
            for (const auto& b : buttons) b.Render(framebuffer);

            break;
        }


        case ButtonType::Save:
        {
            const unsigned int TOOLBAR_H = 50;

            // if window is too small
            if (framebuffer.height <= TOOLBAR_H) {
                std::cout << "Save failed: framebuffer too small\n";
                break;
            }

            Image canvas(framebuffer.width, framebuffer.height - TOOLBAR_H);

            // copy pixels from framebuffer
            for (unsigned int y = 0; y < canvas.height; ++y)
            {
                for (unsigned int x = 0; x < canvas.width; ++x)
                {
                    canvas.SetPixel(x, y, framebuffer.GetPixel(x, y + TOOLBAR_H));
                }
            }

            // save to res path
            canvas.SaveTGA("output.tga");
            break;
        }


        default: break;
    }
}


void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_LEFT)  orbiting = false;
    if (event.button == SDL_BUTTON_RIGHT) panning  = false;
}


void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
    Vector2 newPos((float)event.x, (float)event.y);
    Vector2 prevPos((float)mouse_position.x, (float)(window_height - 1 - mouse_position.y));

    Vector2 canvasNow = MouseToCanvas(event.x, event.y);
    Vector2 delta = canvasNow - mouse_position;
    mouse_position = canvasNow;

    // orbit (left drag)
    if (orbiting)
    {
        orbitYaw   += delta.x * orbitSpeed;
        orbitPitch += delta.y * orbitSpeed;
        UpdateCameraFromOrbit();
        return;
    }

    // pan target (right drag)
    if (panning)
    {
        // move centre along camera right/up
        Vector3 forward = (cam->center - cam->eye).Normalize();
        Vector3 right   = forward.Cross(cam->up).Normalize();
        Vector3 upMove  = right.Cross(forward).Normalize();

        cam->center = cam->center + right * (delta.x * panSpeed) + upMove * (delta.y * panSpeed);
        UpdateCameraFromOrbit(); // recompute eye from orbitDist/yaw/pitch around new center
        return;
    }
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
    float dy = event.preciseY;

    // zoom, change orbit distance
    orbitDist *= (1.0f - dy * 0.1f);
    orbitDist = std::max(0.1f, orbitDist);

    UpdateCameraFromOrbit();
}


void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}

#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w,h;
	SDL_GetWindowSize(window,&w,&h);

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

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;

	// init the window colours
	framebuffer.Fill(Color::WHITE);
	framebuffer.DrawRect(0, 0, 1280, 50, Color::GRAY, 0, true, Color::GRAY);

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
        for (const auto& b : buttons)
            framebuffer.DrawImage(b.icon, (int)b.pos.x, (int)b.pos.y);
}

// Render one frame

	// ancho del rectangulo
	// sin() and cos()
void Application::Render(void)
{
	// remainder that w = 1280, h = 720
	// set up the window
	
	//framebuffer.DrawLineDDA(100, 100, 300, 300, Color::CYAN);
	//framebuffer.DrawLineDDA(230, 300, 230 + 100 * cos(time), 300 + 100 * sin(time), Color::CYAN);
	//framebuffer.DrawRect(200, 300, 400, 400, Color::BLUE, 3, true, Color::BLUE);
	//framebuffer.DrawTriangle({ 100, 100 }, { 200, 200 }, { 300, 100 }, Color::BLUE, true, Color::RED);
	framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed)
{

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
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch(event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
	}
}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
    if (event.button != SDL_BUTTON_LEFT) return;

    Vector2 m = MouseToCanvas(event.x, event.y); // bottom-left coords

    // if click is inside toolbar region (bottom strip), treat as UI click
    if (m.y < 50)
    {
        for (const auto& b : buttons)
        {
            if (b.IsMouseInside(m))
            {
                HandleButton(b.type);
                return;
            }
        }
        return;
    }
    mouse_position = MouseToCanvas(event.x, event.y);   // store coords of mouse position

    if (currentTool == TOOL_PENCIL) {
        pencilDown = true;
        lastPencilPos = mouse_position;
        return;
    }

    
    // lines and rectangles build
    if (currentTool == TOOL_LINE || currentTool == TOOL_RECT) {
        isDragging = true;  // lines and rects work based on drag
        startPos = currentPos = mouse_position;     // set origin point
    } else if (currentTool == TOOL_TRIANGLE){   // triangle build
        if (triClicks == 0) {           // first point
            t0 = mouse_position;
            triClicks = 1;
        } else if (triClicks == 1) {    // second point
            t1 = mouse_position;
            triClicks = 2;
        } else {                        // third point
            Vector2 t2 = mouse_position;
            framebuffer.DrawTriangle(t0, t1, t2, borderColor, fillShapes, fillColor);   // build triangle
            triClicks = 0;  // reset counter since build done
        }
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
            framebuffer.Fill(Color::WHITE);
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
            framebuffer.Fill(Color::WHITE);

            // draw loaded image above toolbar.
            framebuffer.DrawImage(img, 0, 50);

            // redraw toolbar background + buttons
            framebuffer.DrawRect(0, 0, window_width, 50, Color::GRAY, 0, true, Color::GRAY);
            for (const auto& b : buttons) b.Render(framebuffer);

            break;
        }


        case ButtonType::Save:
            // save to disk
            break;

        default: break;
    }
}


void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
    if (event.button != SDL_BUTTON_LEFT) return;
    
    mouse_position = MouseToCanvas(event.x, event.y);
    
    if (currentTool == TOOL_PENCIL) {
        pencilDown = false;
        return;
    }

    if (!isDragging) return;
    isDragging = false;
    if (event.y < 50) return;
    
    currentPos = mouse_position;
    
    if (currentTool == TOOL_LINE) {         // draw line
        framebuffer.DrawLineDDA((int)startPos.x, (int)startPos.y, (int)currentPos.x, (int)currentPos.y, borderColor);
    } else if (currentTool == TOOL_RECT) {  // draw rectangle
        int x, y, w, h;
        DragToRect(startPos, currentPos, x, y, w, h);   // convert into coords and sizes values
        framebuffer.DrawRect(x, y, w, h, borderColor, 1, fillShapes, fillColor);
    }
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
    mouse_delta = Vector2((float)event.x, (float)(-event.y));   // flip y sign
    mouse_position = MouseToCanvas(event.x, event.y);
    
    // don't draw on toolbar
    if (mouse_position.y < 50) return;
    // Pencil: draw small segments following the mouse
    if (currentTool == TOOL_PENCIL && pencilDown)
    {
        Vector2 cur = mouse_position;

        framebuffer.DrawLineDDA(
            (int)lastPencilPos.x, (int)lastPencilPos.y,
            (int)cur.x,          (int)cur.y,
            borderColor
        );

        lastPencilPos = cur;
        return;
    }
    
    if (!isDragging) return;
    currentPos = mouse_position;

    // line
    if (currentTool == TOOL_LINE) {
        framebuffer.DrawLineDDA((int)startPos.x, (int)startPos.y, (int)currentPos.x, (int)currentPos.y, borderColor);    // draw line
    } else if (currentTool == TOOL_RECT) {
        int x, y, w, h; // origin and size      // cant figure out a way to omit toolbar for lines/rect
        DragToRect(startPos, currentPos, x, y, w, h);   // turn into coords and sizes values
        framebuffer.DrawRect(x, y, w, h, borderColor, 1, fillShapes, fillColor);    // draw rectangle
    }
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
	float dy = event.preciseY;

	// ...
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}

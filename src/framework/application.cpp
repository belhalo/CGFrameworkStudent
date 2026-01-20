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
    
    mouse_position = MouseToCanvas(event.x, event.y);   // store coords of mouse position
    
    // toolbar area (bottom)
    if (event.y > window_height - 50) return;   // NEED TO ADD HANDLING OF BUTTONS HERE !!
    
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

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
    if (event.button != SDL_BUTTON_LEFT) return;
    
    mouse_position = MouseToCanvas(event.x, event.y);
    
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
    
    if (!isDragging) return;
    if (event.y < 50) return;
    
    currentPos = mouse_position;
    
    // line
    if (currentTool == TOOL_LINE) {
        framebuffer.DrawLineDDA((int)startPos.x, (int)startPos.y, (int)currentPos.x, (int)currentPos.y, borderColor);    // draw line
    } else if (currentTool == TOOL_RECT) {
        int x, y, w, h; // origin and size
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

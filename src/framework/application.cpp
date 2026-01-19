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
}

// Render one frame

	// ancho del rectangulo
	// sin() and cos()
void Application::Render(void)
{
	// remainder that w = 1280, h = 720
	// set up the window
	framebuffer.Fill(Color::BLACK); 
	framebuffer.DrawRect(0, 0, 1280, 30, Color::GRAY, 0, true, Color::GRAY);

	Image clear; 
	//clear.LoadPNG("|\images\\clear.png", true);
	glRasterPos2f(-1.0f, -1.0f);
	if (clear.LoadPNG("images\\clear.png", true) == false) {
		std::cout << "Image not found!" << std::endl;
	}

	clear.Render();

	/*const char* file = "C:\Users\belha\Documents\GitHub\CGFrameworkStudent\res\images";
	framebuffer.LoadPNG(file, false);
	*/
	

	//framebuffer.DrawLineDDA(100, 100, 300, 300, Color::CYAN);
	//framebuffer.DrawLineDDA(230, 300, 230 + 100 * cos(time), 300 + 100 * sin(time), Color::CYAN);

	//framebuffer.DrawRect(100, 100, 45, 1280, Color::GRAY, 0, true, Color::GRAY);
	//framebuffer.DrawTriangle({ 100, 100 }, { 200, 200 }, { 300, 100 }, Color::BLUE, true, Color::RED);
	framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed)
{

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
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
	
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